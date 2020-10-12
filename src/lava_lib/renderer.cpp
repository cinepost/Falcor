#include "renderer.h"

#include "Falcor/Utils/Threading.h"
#include "Falcor/Utils/Scripting/Scripting.h"
#include "Falcor/Utils/Scripting/ScriptBindings.h"
#include "Falcor/Utils/Debug/debug.h"

#include "lava_utils_lib/logging.h"

namespace Falcor { 

IFramework* gpFramework = nullptr;

}

namespace lava {

Renderer::UniquePtr Renderer::create() {
	return std::move(UniquePtr( new Renderer()));
}

Renderer::Renderer(): mIfaceAquired(false), mpClock(nullptr), mpFrameRate(nullptr), mActiveGraph(0), mInited(false) {
	LLOG_DBG << "Renderer::Renderer";
}

bool Renderer::init() {
	if(mInited) return true;

	LLOG_DBG << "Renderer::init";

	Falcor::OSServices::start();

	Falcor::Scripting::start();
    auto regBinding = [this](Falcor::ScriptBindings::Module& m) {this->registerScriptBindings(m); };
    Falcor::ScriptBindings::registerBinding(regBinding);

    Falcor::Threading::start();

	Falcor::Device::Desc device_desc;
    device_desc.width = 1280;
    device_desc.height = 720;

	mpDevice = Falcor::DeviceManager::instance().createRenderingDevice(0, device_desc);
    mpScene = lava::Scene::create(mpDevice);

	mpClock = new Falcor::Clock(mpDevice);
    //mpClock->setTimeScale(config.timeScale);

    mpFrameRate = new Falcor::FrameRate(mpDevice);

    Falcor::gpFramework = this;

    LLOG_DBG << "Getting offscreen FBO";
    auto pBackBufferFBO = mpDevice->getOffscreenFbo();
    LLOG_DBG << "Offscreen FBO get";
    if (!pBackBufferFBO) {
        LLOG_ERR << "Unable to get rendering device swapchain FBO!!!";
    }

    //LLOG_DBG << "Created pBackBufferFBO size: " << pBackBufferFBO->getWidth() << " " << pBackBufferFBO->getHeight();
    //mpTargetFBO = Falcor::Fbo::create2D(mpDevice, pBackBufferFBO->getWidth(), pBackBufferFBO->getHeight(), pBackBufferFBO->getDesc());
    //LLOG_DBG << "Renderer::init done!";

    mInited = true;
    return true;
}

Renderer::~Renderer() {
	LLOG_DBG << "Renderer::~Renderer";

	if(!mInited)
		return;

	delete mpClock;
    delete mpFrameRate;
	
	Falcor::Threading::shutdown();
	Falcor::Scripting::shutdown();
    Falcor::RenderPassLibrary::instance(mpDevice).shutdown();

	if(mpDisplay) mpDisplay->close();

    //mpTargetFBO.reset();
	if(mpDevice) mpDevice->cleanup();
	mpDevice.reset();
    Falcor::OSServices::stop();

    //Falcor::gpFramework = nullptr;
}

std::unique_ptr<RendererIface> Renderer::aquireInterface() {
	if (!mIfaceAquired) {
		return std::move(std::make_unique<RendererIface>(this));
	}
	LLOG_ERR << "cannot aquire renderer interface. relase old first!";
	return nullptr;
}

void Renderer::releaseInterface(std::unique_ptr<RendererIface> pInterface) {
	if(mIfaceAquired) {
		std::move(pInterface).reset();
		mIfaceAquired = false;
	}
}

bool Renderer::loadDisplayDriver(const std::string& display_name) {
	mpDisplay = Display::create(display_name);
	if(!mpDisplay)
		return false;

	return true;
}

bool Renderer::openDisplay(const std::string& image_name, uint width, uint height) {
    if (!mpDisplay) return false;
    return mpDisplay->open(image_name, width, height);
}

bool Renderer::closeDisplay() {
    if (!mpDisplay) return false;
    return mpDisplay->close();
}


bool isInVector(const std::vector<std::string>& strVec, const std::string& str) {
    return std::find(strVec.begin(), strVec.end(), str) != strVec.end();
}

bool Renderer::loadScript(const std::string& file_name) {
	//auto pGraph = Falcor::RenderGraph::create(mpDevice);

	try {
        LLOG_DBG << "Loading frame graph configuration: " << file_name;
        auto ctx = Falcor::Scripting::getGlobalContext();
        ctx.setObject("renderer", this);
        Falcor::Scripting::runScriptFromFile(file_name, ctx);
    } catch (const std::exception& e) {
        LLOG_ERR << "Error when loading configuration file: " << file_name << "\n" + std::string(e.what());
    	return false;
    }

    /*
    mGraphs.push_back({});
	GraphData* data = &mGraphs.back();

	data->pGraph = pGraph;
	data->pGraph->setScene(mpScene);
	if (data->pGraph->getOutputCount() != 0) data->mainOutput = data->pGraph->getOutputName(0);

	// Store the original outputs
    data->originalOutputs = getGraphOutputs(pGraph);

    //for (auto& e : mpExtensions) e->addGraph(pGraph.get());
	*/

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
    data.pGraph->setScene(mpScene->getScene());
    if (data.pGraph->getOutputCount() != 0) data.mainOutput = data.pGraph->getOutputName(0);

    // Store the original outputs
    data.originalOutputs = getGraphOutputs(pGraph);

    //for (auto& e : mpExtensions) e->addGraph(pGraph.get());
}

void Renderer::executeActiveGraph(Falcor::RenderContext* pRenderContext) {
    if (mGraphs.empty()) return;

    auto& pGraph = mGraphs[mActiveGraph].pGraph;
    LLOG_DBG << "Execute graph: " << pGraph->getName() << " output name: " << mGraphs[mActiveGraph].mainOutput;

    // Execute graph.
    (*pGraph->getPassesDictionary())[Falcor::kRenderPassRefreshFlags] = (uint32_t)Falcor::RenderPassRefreshFlags::None;
    pGraph->execute(pRenderContext);
}

void Renderer::renderFrame(uint samples) {
	if (!mInited) {
		LLOG_ERR << "Renderer not initialized !!!";
		return;
	}

    if(!mpDisplay) {
        LLOG_ERR << "Renderer display not initialized !!!";
        return;
    }

    if(!mpDisplay->opened()) {
        LLOG_ERR << "Renderer display not opened !!!";
        return;
    }

    uint image_width, image_height;
    image_width = mpDisplay->width();
    image_height = mpDisplay->height();

    for (auto const& gData: mGraphs) {
        auto dims = gData.pGraph->dims();
        if (dims.x != image_width || dims.y != image_height) {
            gData.pGraph->resize(image_width, image_height, Falcor::ResourceFormat::RGBA32Float);
        }
    }

    auto pRenderContext = mpDevice->getRenderContext();

    LLOG_DBG << "Renderer::renderFrame";

    //beginFrame(pRenderContext, mpTargetFBO);

    if (mGraphs.size()) {
        LLOG_DBG << "process render graphs";
        auto& pGraph = mGraphs[mActiveGraph].pGraph;

        auto pScene = mpScene->getScene();
        // Update scene and camera.
        if (pScene) {
            pScene->update(pRenderContext, Falcor::gpFramework->getClock().getTime());
        }

        executeActiveGraph(pRenderContext);
        
        // capture graph(s) ouput(s).
        if (mGraphs[mActiveGraph].mainOutput.size()) {
            Falcor::Texture::SharedPtr pOutTex = std::dynamic_pointer_cast<Falcor::Texture>(pGraph->getOutput(mGraphs[mActiveGraph].mainOutput));
            assert(pOutTex);

            // image save test
            Falcor::Texture* pTex = pOutTex.get();//pGraph->getOutput(i)->asTexture().get();
            assert(pTex);

            //std::string filename = "/home/max/test/lava_render_test_2.";
            //auto ext = Falcor::Bitmap::getFileExtFromResourceFormat(pTex->getFormat());
            //filename += ext;
            //auto format = Falcor::Bitmap::getFormatFromFileExtension(ext);
            
            //pTex->captureToFileBlocking(0, 0, filename, format);
            
            {
            
            Falcor::ResourceFormat resourceFormat;
            uint32_t channels;
            std::vector<uint8_t> textureData;
            pTex->readTextureData(0, 0, textureData, resourceFormat, channels);
            
            LLOG_DBG << "Texture read data size is: " << textureData.size() << " bytes";
            assert(textureData.size() == image_width * image_height * channels * 4); // testing only on 32bit RGBA for now
            //mpDisplay->sendBucket(0, 0, image_width, image_height, textureData.data());
            

            for(uint32_t x = 0; x < image_width; x+=4) {
                for(uint32_t y = 0; y < image_height; y++) {
                    textureData[(x + y*image_width)*4] = 255;
                }
            }

            mpDisplay->sendImage(image_width, image_height, textureData.data());

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

bool Renderer::pushDisplayStringParameter(const std::string& name, const std::vector<std::string>& strings) {
    if(!mpDisplay) {
        LLOG_ERR << "No display driver loaded !!!";
        return false;
    }

    return mpDisplay->pushStringParameter(name, strings);
}

bool Renderer::pushDisplayIntParameter(const std::string& name, const std::vector<int>& ints) {
    if(!mpDisplay) {
        LLOG_ERR << "No display driver loaded !!!";
        return false;
    }
    
    return mpDisplay->pushIntParameter(name, ints);
}

bool Renderer::pushDisplayFloatParameter(const std::string& name, const std::vector<float>& floats) {
    if(!mpDisplay) {
        LLOG_ERR << "No display driver loaded !!!";
        return false;
    }
    
    return mpDisplay->pushFloatParameter(name, floats);
}

// IFramework 
Falcor::RenderContext* Renderer::getRenderContext() {
	return mpDevice ? mpDevice->getRenderContext() : nullptr;
}

std::shared_ptr<Falcor::Fbo> Renderer::getTargetFbo() {
	return mpTargetFBO;
}

Falcor::Clock& Renderer::getClock() {
	return *mpClock;
}

Falcor::FrameRate& Renderer::getFrameRate() {
	return *mpFrameRate;
}

void Renderer::resizeSwapChain(uint32_t width, uint32_t height) {

}

Falcor::SampleConfig Renderer::getConfig() {
    Falcor::SampleConfig c;
    c.deviceDesc = mpDevice->getDesc();
    //c.windowDesc = mpWindow->getDesc();
    c.showMessageBoxOnError = false;//Logger::isBoxShownOnError();
    c.timeScale = (float)mpClock->getTimeScale();
    c.pauseTime = mpClock->isPaused();
    c.showUI = false;
    return c;
}

}  // namespace lava