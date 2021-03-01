#include "renderer.h"

#include "Falcor/Core/API/ResourceManager.h"
#include "Falcor/Utils/Threading.h"
#include "Falcor/Utils/Scripting/Scripting.h"
#include "Falcor/Utils/Scripting/Dictionary.h"
#include "Falcor/Utils/Scripting/ScriptBindings.h"
#include "Falcor/Utils/ConfigStore.h"
#include "Falcor/Utils/Debug/debug.h"

#include "Falcor/Experimental/Scene/Lights/EnvMap.h"

#include "lava_utils_lib/logging.h"

namespace Falcor {  IFramework* gpFramework = nullptr; } // TODO: probably it's safe to remove now...

namespace lava {

// TODO: handle requred channels (RGB/RGBA)
static Falcor::ResourceFormat resolveShadingResourceFormat(Display::TypeFormat fmt, uint numchannels) {
    assert(numchannels <= 4);

    switch(fmt) {
        case Display::TypeFormat::SIGNED8:
            if( numchannels == 1) return Falcor::ResourceFormat::R8Snorm; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG8Snorm;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB8Snorm;
            return Falcor::ResourceFormat::RGBA8Snorm;

        case Display::TypeFormat::UNSIGNED8:
            if( numchannels == 1) return Falcor::ResourceFormat::R8Unorm; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG8Unorm;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB8Unorm;
            return Falcor::ResourceFormat::RGBA8Unorm;

        case Display::TypeFormat::SIGNED16:
            if( numchannels == 1) return Falcor::ResourceFormat::R16Int; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG16Int;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB16Int;
            return Falcor::ResourceFormat::RGBA16Int;  // TODO: add RGBA16Snorm to Falcor formats
        
        case Display::TypeFormat::UNSIGNED16:
            if( numchannels == 1) return Falcor::ResourceFormat::R16Unorm; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG16Unorm;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB16Unorm;
            return Falcor::ResourceFormat::RGBA16Unorm;
        
        case Display::TypeFormat::FLOAT16:
            if( numchannels == 1) return Falcor::ResourceFormat::R16Float; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG16Float;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB16Float;
            return Falcor::ResourceFormat::RGBA16Float;
        
        case Display::TypeFormat::FLOAT32:
        default:
            if( numchannels == 1) return Falcor::ResourceFormat::R32Float; 
            if( numchannels == 3) return Falcor::ResourceFormat::RG32Float;
            if( numchannels == 3) return Falcor::ResourceFormat::RGB32Float;
            return Falcor::ResourceFormat::RGBA32Float;
    }
}

Renderer::SharedPtr Renderer::create(Device::SharedPtr pDevice) {
    assert(pDevice);
    return SharedPtr(new Renderer(pDevice));
}


Renderer::Renderer(Device::SharedPtr pDevice): mpDevice(pDevice), mIfaceAquired(false), mpClock(nullptr), mpFrameRate(nullptr), mActiveGraph(0), mInited(false) {
	LLOG_DBG << "Renderer::Renderer";
    mpDisplay = nullptr;
}

bool Renderer::init() {
	if(mInited) return true;

	LLOG_DBG << "Renderer::init";

	Falcor::OSServices::start();

	//Falcor::Scripting::start();
    //Falcor::ScriptBindings::registerBinding(Renderer::registerBindings);

    Falcor::Threading::start();

    mpSceneBuilder = lava::SceneBuilder::create(mpDevice);
    mpCamera = Falcor::Camera::create();
    mpCamera->setName("main");
    mpSceneBuilder->addCamera(mpCamera);
    mpSceneBuilder->setCamera("main");

	//mpClock = new Falcor::Clock(mpDevice);
    //mpClock->setTimeScale(config.timeScale);

    //auto pBackBufferFBO = mpDevice->getOffscreenFbo();
    //if (!pBackBufferFBO) {
    //    logError("Unable to get swap chain FBO!!!");
    //}

    //mpFrameRate = new Falcor::FrameRate(mpDevice);

    mInited = true;
    return true;
}

Renderer::~Renderer() {
    LLOG_DBG << "Renderer::~Renderer";

    if(!mInited)
        return;

    mpDevice->resourceManager()->printStats();

    //delete mpClock;
    //delete mpFrameRate;

    mpDevice->flushAndSync();
    mGraphs.clear();

    mpSceneBuilder = nullptr;
    mpSampler = nullptr;

    Falcor::Threading::shutdown();
    //Falcor::Scripting::shutdown();
    //Falcor::RenderPassLibrary::instance(mpDevice).shutdown();

    if(mpDisplay)
        mpDisplay->close();

    mpTargetFBO.reset();

    if(mpDevice)
        mpDevice->cleanup();

    mpDevice.reset();

    Falcor::OSServices::stop();

    LLOG_DBG << "Renderer::~Renderer done";
}

std::unique_ptr<RendererIface> Renderer::aquireInterface() {
	if (!mIfaceAquired) {
		return std::move(std::make_unique<RendererIface>(shared_from_this()));
	}
	LLOG_ERR << "Сan't aquire renderer interface. Relase old first!";
	return nullptr;
}

void Renderer::releaseInterface(std::unique_ptr<RendererIface> pInterface) {
	if(mIfaceAquired) {
		std::move(pInterface).reset();
		mIfaceAquired = false;
	}
}

bool Renderer::addPlane(const RendererIface::PlaneData plane_data) {
    auto it = mPlanes.find(plane_data.channel);
    if( it != mPlanes.end()) {
        LLOG_ERR << "Output plane " << plane_data.name << " already exist !";
        return false;
    }
    mPlanes[plane_data.channel] = plane_data;

    return true;
}

bool Renderer::loadDisplay(Display::DisplayType display_type) {
	mpDisplay = Display::create(display_type);
	if(!mpDisplay) {
        LLOG_ERR << "Unable to create display !!!";
		return false;
    }

	return true;
}

bool Renderer::closeDisplay() {
    if (!mpDisplay) return false;
    return mpDisplay->close();
}


bool isInVector(const std::vector<std::string>& strVec, const std::string& str) {
    return std::find(strVec.begin(), strVec.end(), str) != strVec.end();
}

void Renderer::createRenderGraph(const RendererIface::FrameData& frame_data) {
    assert(mpDevice);

    auto pRenderContext = mpDevice->getRenderContext();
    auto pScene = mpSceneBuilder->getScene();

    assert(pScene);
    
    auto const& confgStore = Falcor::ConfigStore::instance();
    bool vtoff = confgStore.get<bool>("vtoff", true);

    // Pick rendering resource format's according to required Display::TypeFormat
    Falcor::ResourceFormat shadingResourceFormat;
    Falcor::ResourceFormat auxAlbedoResourceFormat;

    for(const auto& [channel, plane]: mPlanes) {
        switch(channel) {
            case RendererIface::PlaneData::Channel::COLOR:
                shadingResourceFormat = resolveShadingResourceFormat(plane.format, 3);
                break;
            case RendererIface::PlaneData::Channel::COLOR_ALPHA:
                shadingResourceFormat = resolveShadingResourceFormat(plane.format, 4);
                break;
            case RendererIface::PlaneData::Channel::G_ALBEDO:
                auxAlbedoResourceFormat = resolveShadingResourceFormat(plane.format, 3);
                break;
            default:
                break;
        }
    }

    //// create env map stuff
    auto pLightProbe = mpSceneBuilder->getLightProbe();
    if(pLightProbe) {
        auto pTexture = pLightProbe->getOrigTexture();
        if (pTexture) {
            auto pEnvMap = Falcor::EnvMap::create(mpDevice, pTexture->getSourceFilename());
            pScene->setEnvMap(pEnvMap);
        }
        //pScene->loadEnvMap("/home/max/Desktop/parking_lot.hdr");
    }
    ////

    // Rasterizer state
    Falcor::RasterizerState::Desc rsDesc;
    const std::string& cull_mode = confgStore.get<std::string>("cull_mode", "none");
    if (cull_mode == "back") {
        rsDesc.setCullMode(RasterizerState::CullMode::Back);
    } else if (cull_mode == "front") {
        rsDesc.setCullMode(RasterizerState::CullMode::Front);
    } else {
        rsDesc.setCullMode(RasterizerState::CullMode::None);
    }
    rsDesc.setFillMode(RasterizerState::FillMode::Solid);

    // Depth pre pass
    auto depthChannelOutputFormat = ResourceFormat::D32Float;
    mpDepthPrePassGraph = RenderGraph::create(pRenderContext->device(), {frame_data.imageWidth, frame_data.imageHeight}, depthChannelOutputFormat, "Depth Pre-Pass");
    mpDepthPrePass = DepthPass::create(pRenderContext);
    mpDepthPrePass->setDepthBufferFormat(ResourceFormat::D32Float);
    mpDepthPrePass->setScene(pRenderContext, pScene);
    mpDepthPrePass->setRasterizerState(Falcor::RasterizerState::create(rsDesc));
    mpDepthPrePassGraph->addPass(mpDepthPrePass, "DepthPrePass");
    mpDepthPrePassGraph->markOutput("DepthPrePass.depth");

    // Virtual textures stuff
    if(!vtoff) {
        auto vtexResolveChannelOutputFormat = ResourceFormat::RGBA8Unorm;
        mpTexturesResolvePassGraph = RenderGraph::create(mpDevice, {frame_data.imageWidth, frame_data.imageHeight}, vtexResolveChannelOutputFormat, "VirtualTexturesGraph");

        mpTexturesResolvePass = TexturesResolvePass::create(pRenderContext);
        mpTexturesResolvePass->setRasterizerState(Falcor::RasterizerState::create(rsDesc));
        mpTexturesResolvePass->setScene(pRenderContext, pScene);

        mpTexturesResolvePassGraph->addPass(mpTexturesResolvePass, "SparseTexturesResolvePrePass");
        mpTexturesResolvePassGraph->markOutput("SparseTexturesResolvePrePass.output");

        //mpTexturesResolvePassGraph->setInput("SparseTexturesResolvePrePass.depth", mpDepthPrePassGraph->getOutput("DepthPrePass.depth"));
    } else {
        mpTexturesResolvePassGraph = nullptr;
        mpTexturesResolvePass = nullptr;
    }

    // Main render graph
    auto mainChannelOutputFormat = ResourceFormat::RGBA32Float;
    mpRenderGraph = RenderGraph::create(mpDevice, {frame_data.imageWidth, frame_data.imageHeight}, mainChannelOutputFormat, "MainImageRenderGraph");

    // Forward lighting
    mpLightingPass = ForwardLightingPass::create(pRenderContext);
    mpLightingPass->setRasterizerState(Falcor::RasterizerState::create(rsDesc));
    mpLightingPass->setScene(pRenderContext, pScene);
    mpLightingPass->setColorFormat(mainChannelOutputFormat);
    auto pass2 = mpRenderGraph->addPass(mpLightingPass, "LightingPass");


    // SkyBox
    mpSkyBoxPass = SkyBox::create(pRenderContext);
    
    if(pLightProbe) {
        mpSkyBoxPass->setIntensity(pLightProbe->getIntensity());
    } else {
        mpSkyBoxPass->setTransparency(0.0f);
    }
    mpSkyBoxPass->setScene(pRenderContext, pScene);
    auto pass3 = mpRenderGraph->addPass(mpSkyBoxPass, "SkyBoxPass");

    // Accumulaion
    mpAccumulatePass = AccumulatePass::create(pRenderContext);
    mpAccumulatePass->enableAccumulation(true);
    mpRenderGraph->addPass(mpAccumulatePass, "AccumulatePass");

    //mpRenderGraph->setInput("SkyBoxPass.depth", mpDepthPrePassGraph->getOutput("DepthPrePass.depth"));
    mpRenderGraph->addEdge("SkyBoxPass.target", "LightingPass.color");
    //mpRenderGraph->setInput("LightingPass.depth", mpDepthPrePassGraph->getOutput("DepthPrePass.depth"));
    mpRenderGraph->addEdge("LightingPass.color", "AccumulatePass.input");

    mpRenderGraph->markOutput("AccumulatePass.output");

}

bool Renderer::loadScript(const std::string& file_name) {
    return true;

	try {
        LLOG_DBG << "Loading frame graph configuration: " << file_name;
        auto ctx = Falcor::Scripting::getGlobalContext();
        ctx.setObject("renderer", this);
        Falcor::Scripting::runScriptFromFile(file_name, ctx);
    } catch (const std::exception& e) {
        LLOG_ERR << "Error when loading configuration file: " << file_name << "\n" + std::string(e.what());
    	return false;
    }

    LLOG_DBG << "Frame graph configuration loaded!";
    return true;
}

std::vector<std::string> Renderer::getGraphOutputs(const Falcor::RenderGraph::SharedPtr& pGraph) {
    std::vector<std::string> outputs;
    for (size_t i = 0; i < pGraph->getOutputCount(); i++) outputs.push_back(pGraph->getOutputName(i));
    return outputs;
}

void Renderer::addGraph(const Falcor::RenderGraph::SharedPtr& pGraph) {
	LLOG_DBG << "Renderer::addGraph";

    if (pGraph == nullptr) {
        LLOG_ERR << "Can't add an empty graph";
        return;
    }

    // If a graph with the same name already exists, remove it
    GraphData* pGraphData = nullptr;
    for (size_t i = 0; i < mGraphs.size(); i++) {
        if (mGraphs[i].pGraph->getName() == pGraph->getName()) {
            LLOG_WRN << "Replacing existing graph \"" << pGraph->getName() << "\" with new graph.";
            pGraphData = &mGraphs[i];
            break;
        }
    }

    // FIXME: put individual graphs initalization down the pipeline. Also cache inited graph until scene changed
    initGraph(pGraph, pGraphData);
}

void Renderer::initGraph(const Falcor::RenderGraph::SharedPtr& pGraph, GraphData* pData) {
    if (!pData) {
        mGraphs.push_back({});
        pData = &mGraphs.back();
    }

    GraphData& data = *pData;
    // Set input image if it exists
    data.pGraph = pGraph;
    //data.pGraph->setScene(mpSceneBuilder->getScene());
    if (data.pGraph->getOutputCount() != 0) data.mainOutput = data.pGraph->getOutputName(0);

    // Store the original outputs
    data.originalOutputs = getGraphOutputs(pGraph);
}

void Renderer::resolvePerFrameSparseResourcesForActiveGraph(Falcor::RenderContext* pRenderContext) {
    if (mGraphs.empty()) return;

    auto& pGraph = mGraphs[mActiveGraph].pGraph;
    LLOG_DBG << "Resolve per frame sparse resources for graph: " << pGraph->getName() << " output name: " << mGraphs[mActiveGraph].mainOutput;

    // Execute graph.
    (*pGraph->getPassesDictionary())[Falcor::kRenderPassRefreshFlags] = Falcor::RenderPassRefreshFlags::None;
    pGraph->resolvePerFrameSparseResources(pRenderContext);

    //mpSceneBuilder->finalize();
}

void Renderer::executeActiveGraph(Falcor::RenderContext* pRenderContext) {
    if (mpRenderGraph)
        mpRenderGraph->execute(pRenderContext);

    return;

    if (mGraphs.empty()) return;

    auto& pGraph = mGraphs[mActiveGraph].pGraph;
    LLOG_DBG << "Execute graph: " << pGraph->getName() << " output name: " << mGraphs[mActiveGraph].mainOutput;

    // Execute graph.
    (*pGraph->getPassesDictionary())[Falcor::kRenderPassRefreshFlags] = Falcor::RenderPassRefreshFlags::None;
    //pGraph->resolvePerSampleSparseResources(pRenderContext);
    pGraph->execute(pRenderContext);
}

static CPUSampleGenerator::SharedPtr createSamplePattern(RendererIface::SamplePattern type, uint32_t sampleCount) {
    switch (type) {
        case RendererIface::SamplePattern::Center:
            return nullptr;
        case RendererIface::SamplePattern::DirectX:
            return DxSamplePattern::create(sampleCount);
        case RendererIface::SamplePattern::Halton:
            return HaltonSamplePattern::create(sampleCount);
        case RendererIface::SamplePattern::Stratified:
            return StratifiedSamplePattern::create(sampleCount);
        default:
            should_not_get_here();
            return nullptr;
    }
}

void Renderer::finalizeScene(const RendererIface::FrameData& frame_data) {
    // finalize camera
    mInvFrameDim = 1.f / float2({frame_data.imageWidth, frame_data.imageHeight});

    mpSampleGenerator = createSamplePattern(frame_data.samplePattern, frame_data.imageSamples);
    if (mpSampleGenerator) {
        mpCamera->setPatternGenerator(mpSampleGenerator, mInvFrameDim);
    }

    mpCamera->setAspectRatio(static_cast<float>(frame_data.imageWidth) / static_cast<float>(frame_data.imageHeight));
    mpCamera->setNearPlane(frame_data.cameraNearPlane);
    mpCamera->setFarPlane(frame_data.cameraFarPlane);
    mpCamera->setViewMatrix(frame_data.cameraTransform);
    mpCamera->setFocalLength(frame_data.cameraFocalLength);
    mpCamera->setFrameHeight(frame_data.cameraFrameHeight);
    //mpCamera->beginFrame(true); // Not sure we need it

    // finalize scene
    auto pScene = mpSceneBuilder->getScene();


    if (pScene) {
        pScene->setCameraAspectRatio(static_cast<float>(frame_data.imageWidth) / static_cast<float>(frame_data.imageHeight));

        if (mpSampler == nullptr) {
            // create common texture sampler
            Sampler::Desc desc;
            desc.setFilterMode(Sampler::Filter::Point, Sampler::Filter::Linear, Sampler::Filter::Linear);
            //desc.setLodParams(0,8,1);
            //desc.setMaxAnisotropy(16);
            mpSampler = Falcor::Sampler::create(mpDevice, desc);
        }
        pScene->bindSamplerToMaterials(mpSampler);
    }

    //const auto& dims = mpRenderGraph->dims();
    //if (dims.x != frame_data.imageWidth || dims.y != frame_data.imageHeight) {
    //    mpRenderGraph->resize(frame_data.imageWidth, frame_data.imageHeight, Falcor::ResourceFormat::RGBA32Float);
    //}
}

void Renderer::renderFrame(const RendererIface::FrameData frame_data) {
	if (!mInited) {
		LLOG_ERR << "Renderer not initialized !!!";
		return;
	}

    if(!mpDisplay) {
        LLOG_ERR << "Renderer display not initialized !!!";
        return;
    }

    if( frame_data.imageSamples == 0) {
        LLOG_WRN << "Not enough image samples specified in frame data !";
    }

    if(mpDisplay->opened()) {
        mpDisplay->close();
    }

    std::vector<Display::Channel> channels;
    channels.push_back({"r", Display::TypeFormat::FLOAT32});
    channels.push_back({"g", Display::TypeFormat::FLOAT32});
    channels.push_back({"b", Display::TypeFormat::FLOAT32});
    channels.push_back({"a", Display::TypeFormat::FLOAT32});
    //channels.push_back({"z", Display::TypeFormat::FLOAT32});
    //channels.push_back({"albedo.r", Display::TypeFormat::FLOAT16});
    //channels.push_back({"albedo.g", Display::TypeFormat::FLOAT16});
    //channels.push_back({"albedo.b", Display::TypeFormat::FLOAT16});
    
    if(!mpDisplay->open(frame_data.imageFileName, frame_data.imageWidth, frame_data.imageHeight, channels)) {
        LLOG_ERR << "Unable to open image " << frame_data.imageFileName << " !!!";
    }

    //this->resizeSwapChain(frame_data.imageWidth, frame_data.imageHeight);

    if (!mpRenderGraph) 
        createRenderGraph(frame_data);

    finalizeScene(frame_data);

    auto pRenderContext = mpDevice->getRenderContext();

    LLOG_DBG << "Renderer::renderFrame";

    //if (mGraphs.size()) {
    if (mpRenderGraph) {    
        LLOG_DBG << "process render graphs";
        
        auto pScene = mpSceneBuilder->getScene();
        if (!pScene) {
            LLOG_ERR << "Unable to get scene from scene builder !!!";
            return;
        }

        // render image samples
        double shutter_length = 0.5;
        double fps = 25.0;
        double time = frame_data.time;
        double sample_time_duration = (1.0 * shutter_length) / frame_data.imageSamples;
        
        //resolvePerFrameSparseResourcesForActiveGraph(pRenderContext);
        pScene->update(pRenderContext, time);

        mpDepthPrePassGraph->execute(pRenderContext);
        auto pDepth = mpDepthPrePassGraph->getOutput("DepthPrePass.depth");

        if(mpTexturesResolvePassGraph) {
            mpTexturesResolvePassGraph->setInput("SparseTexturesResolvePrePass.depth", pDepth);
            mpTexturesResolvePassGraph->execute(pRenderContext);
        }

        mpRenderGraph->setInput("SkyBoxPass.depth", pDepth);
        mpRenderGraph->setInput("LightingPass.depth", pDepth);
        mpRenderGraph->execute(pRenderContext);

        if ( frame_data.imageSamples > 1 ) {
            for (uint i = 1; i < frame_data.imageSamples; i++) {
                LLOG_DBG << "Rendering sample no " << i << " of " << frame_data.imageSamples;
                
                // Update scene and camera.
                time += sample_time_duration;
                pScene->update(pRenderContext, time);
                
                mpDepthPrePassGraph->execute(pRenderContext);
                mpRenderGraph->execute(pRenderContext);
            }
        }

        LLOG_DBG << "Rendering done.";
        

        // capture graph(s) ouput(s).
        if (mpRenderGraph) {    
            LLOG_DBG << "Reading rendered image data...";
            auto& pGraph = mGraphs[mActiveGraph].pGraph;

            //Falcor::Texture::SharedPtr pOutTex = std::dynamic_pointer_cast<Falcor::Texture>(pGraph->getOutput(mGraphs[mActiveGraph].mainOutput));
            Falcor::Texture::SharedPtr pOutTex = std::dynamic_pointer_cast<Falcor::Texture>(mpRenderGraph->getOutput("AccumulatePass.output"));
            assert(pOutTex);

            Falcor::Texture* pTex = pOutTex.get();
            assert(pTex);
            
            {
            
            Falcor::ResourceFormat resourceFormat;
            uint32_t channels;
            std::vector<uint8_t> textureData;
            LLOG_DBG << "readTextureData";
            pTex->readTextureData(0, 0, textureData, resourceFormat, channels);
            LLOG_DBG << "readTextureData done";

            LLOG_DBG << "Texture read data size is: " << textureData.size() << " bytes";
            
            assert(textureData.size() == frame_data.imageWidth * frame_data.imageHeight * channels * 4); // testing only on 32bit RGBA for now

            mpDisplay->sendImage(frame_data.imageWidth, frame_data.imageHeight, textureData.data());

            }

        } else {
        	LLOG_WRN << "Invalid active graph output!";
        }

    } else {
    	LLOG_WRN << "No graphs to render!";
    }

    //endFrame(pRenderContext, mpTargetFBO);
}

void Renderer::beginFrame(Falcor::RenderContext* pRenderContext, const Falcor::Fbo::SharedPtr& pTargetFbo) {
    //for (auto& pe : mpExtensions)  pe->beginFrame(pRenderContext, pTargetFbo);
}

void Renderer::endFrame(Falcor::RenderContext* pRenderContext, const Falcor::Fbo::SharedPtr& pTargetFbo) {
    //for (auto& pe : mpExtensions) pe->endFrame(pRenderContext, pTargetFbo);
}

}  // namespace lava