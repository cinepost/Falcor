#include <algorithm>
#include <chrono>
#include "TexturesResolvePass.h"

#include "Falcor/Utils/Debug/debug.h"
#include "Falcor/Core/API/ResourceManager.h"

// Don't remove this. it's required for hot-reload to function properly
extern "C" falcorexport const char* getProjDir() {
    return PROJECT_DIR;
}

extern "C" falcorexport void getPasses(Falcor::RenderPassLibrary& lib) {
    lib.registerClass("TexturesResolve", "Resolves sparse textures tiles to be loaded", TexturesResolvePass::create);
}

const char* TexturesResolvePass::kDesc = "Creates a depth-buffer using the scene's active camera";

namespace {
    const std::string kProgramFile = "RenderPasses/TexturesResolvePass/TexturesResolvePass.ps.slang";

    const std::string kDepth = "depth";
    const std::string kDebugColor = "debugColor";

    const std::string kTexResolveData = "gTexResolveData";
    const std::string kParameterBlockName = "gResolveData";

}  // namespace

void TexturesResolvePass::parseDictionary(const Dictionary& dict) {
    float3 a;
}

Dictionary TexturesResolvePass::getScriptingDictionary() {
    Dictionary d;
    return d;
}

TexturesResolvePass::SharedPtr TexturesResolvePass::create(RenderContext* pRenderContext, const Dictionary& dict) {
    return SharedPtr(new TexturesResolvePass(pRenderContext->device(), dict));
}

TexturesResolvePass::TexturesResolvePass(Device::SharedPtr pDevice, const Dictionary& dict): RenderPass(pDevice) {
    Program::Desc desc;
    desc.addShaderLibrary(kProgramFile).vsEntry("vsMain").psEntry("psMain");

    mpProgram = GraphicsProgram::create(pDevice, desc);

    mpFbo = Fbo::create(pDevice);

    mpState = GraphicsState::create(pDevice);

    DepthStencilState::Desc dsDesc;
    dsDesc.setDepthWriteMask(false).setDepthFunc(DepthStencilState::Func::LessEqual);
    //dsDesc.setDepthWriteMask(false).setDepthEnabled(true).setDepthFunc(DepthStencilState::Func::Never);
    mpDsNoDepthWrite = DepthStencilState::create(dsDesc);
    mpState->setDepthStencilState(DepthStencilState::create(dsDesc));

    mpState->setProgram(mpProgram);

    parseDictionary(dict);
}

RenderPassReflection TexturesResolvePass::reflect(const CompileData& compileData) {
    RenderPassReflection reflector;

    reflector.addOutput(kDebugColor, "DebugColor-buffer").format(mTileDataDebugFormat).texture2D(0, 0, 0);
    auto& depthField = reflector.addInputOutput(kDepth, "Depth-buffer. Should be pre-initialized or cleared before calling the pass").bindFlags(Resource::BindFlags::DepthStencil);
    return reflector;
}

void TexturesResolvePass::updateTexturesResolveData() {
 
}

void TexturesResolvePass::setScene(RenderContext* pRenderContext, const Scene::SharedPtr& pScene) {
    mpScene = pScene;
    if (mpScene) {
        mpState->getProgram()->addDefines(mpScene->getSceneDefines());
        //updateTexturesResolveData();
    }
    mpVars = GraphicsVars::create(pRenderContext->device(), mpState->getProgram()->getReflector());
}

void TexturesResolvePass::initDepth(RenderContext* pContext, const RenderData& renderData) {
    const auto& pDepth = renderData[kDepth]->asTexture();

    if (pDepth) {
        mpState->setDepthStencilState(mpDsNoDepthWrite);
        mpFbo->attachDepthStencilTarget(pDepth);
    } else {
        LOG_WARN("No required depth channel provided !!!");
    }
}


void TexturesResolvePass::execute(RenderContext* pContext, const RenderData& renderData) {
    initDepth(pContext, renderData);

    const auto& pDebugData = renderData[kDebugColor]->asTexture();
    mpFbo->attachColorTarget(pDebugData, 0);

    mpState->setFbo(mpFbo);
    pContext->clearRtv(pDebugData->getRTV().get(), {0, 0, 0, 0});

    if (!mpScene)
        return;

    auto exec_started = std::chrono::high_resolution_clock::now();

    uint32_t totalPagesToUpdateCount = 0;
    uint32_t currPagesStartOffset = 0;
    uint32_t currTextureResolveID = 0; // texture id used to identify texture inside pass. always starts from 0.

    std::vector<MaterialResolveData> materialsResolveBuffer;
    std::map<uint32_t, Texture::SharedPtr> texturesMap; // maps real texture ID to textures
    std::map<uint32_t, VirtualTextureData> virtualTexturesDataMap; //

    uint32_t materialsCount = mpScene->getMaterialCount();

    for( uint32_t m_i = 0; m_i < materialsCount; m_i++ ) {
        auto pMaterial =  mpScene->getMaterial(m_i);
        auto materialResources = pMaterial->getResources();

        std::vector<Texture::SharedPtr> materialSparseTextures;

        if(materialResources.baseColor) {
            if (materialResources.baseColor->isSparse()) {
                materialSparseTextures.push_back(materialResources.baseColor);
            }
        }

        if(materialResources.specular) {
            if (materialResources.specular->isSparse()) {
                materialSparseTextures.push_back(materialResources.specular);
            }
        }

        if(materialResources.roughness) {
            if (materialResources.roughness->isSparse()) {
                materialSparseTextures.push_back(materialResources.roughness);
            }
        }

        if(materialResources.normalMap) {
            if (materialResources.normalMap->isSparse()) {
                materialSparseTextures.push_back(materialResources.normalMap);
            }
        }

        MaterialResolveData materialResolveData = {};

        size_t virtualTexturesCount = std::min((size_t)MAX_VTEX_PER_MATERIAL_COUNT, materialSparseTextures.size());
        materialResolveData.virtualTexturesCount = virtualTexturesCount;
        
        // pre-fill some data
        for( size_t t_i = 0; t_i < static_cast<size_t>(MAX_VTEX_PER_MATERIAL_COUNT); t_i++)
            materialResolveData.virtualTextures[t_i].empty = true;

        // fill data for active(used) textures
        for( size_t t_i = 0; t_i < virtualTexturesCount; t_i++) {
            auto &pTexture = materialSparseTextures[t_i];
            uint32_t textureID = pTexture->id();
                
            auto &textureData = materialResolveData.virtualTextures[t_i];

            // Check if this sparse texture data not stored for resolving
            if (virtualTexturesDataMap.find(textureID) == virtualTexturesDataMap.end() ) {
                // Fill vitrual texture data
                textureData.empty = false;
                textureData.textureID = textureID;
                textureData.textureResolveID = currTextureResolveID;
                textureData.width = pTexture->getWidth();
                textureData.height = pTexture->getHeight();
                textureData.mipLevelsCount = pTexture->getMipCount();
                textureData.mipTailStart = pTexture->getMipTailStart();
                textureData.pagesStartOffset = currPagesStartOffset;

                auto pageRes = pTexture->getSparsePageRes();
                textureData.pageSizeW = pageRes.x;
                textureData.pageSizeH = pageRes.y;
                textureData.pageSizeD = pageRes.z;
                
                auto const& mipBases = pTexture->getMipBases();

                memcpy(&textureData.mipBases, mipBases.data(), mipBases.size() * sizeof(uint32_t));

                currTextureResolveID++;
                currPagesStartOffset += pTexture->getSparsePagesCount();
                virtualTexturesDataMap[textureID] = textureData; 
                texturesMap[textureID] = pTexture;
            
                // --- debug info 
                LOG_WARN("Texture id: %u pages offset: %u width: %u height: %u", textureData.textureID, textureData.pagesStartOffset, textureData.width, textureData.height);
                LOG_WARN("Texture id: %u mip levels: %u tail start: %u", textureData.textureID, textureData.mipLevelsCount, textureData.mipTailStart);
                std::cout << "Mip bases : \n";
                for( uint i = 0; i < 16; i++) std::cout << textureData.mipBases[i] << " ";
                std::cout << "\n";

            } else {
                // Virtual texture data cached in map, reuse it
                textureData = virtualTexturesDataMap[textureID];
            }
        }

        materialsResolveBuffer.push_back(materialResolveData);
    }

    totalPagesToUpdateCount = currPagesStartOffset;
    totalPagesToUpdateCount += 16;

    auto pDataToResolveBuffer = Buffer::createStructured(mpDevice, sizeof(MaterialResolveData), materialsCount, Resource::BindFlags::ShaderResource, Buffer::CpuAccess::None, materialsResolveBuffer.data(), true);
    mpVars->setBuffer("materialsResolveData", pDataToResolveBuffer);

    uint32_t resolvedTexturesCount = currTextureResolveID;

    LOG_WARN("Total pages to update for %u textures is %u", resolvedTexturesCount, totalPagesToUpdateCount);

    std::vector<int8_t> pagesInitDataBuffer(totalPagesToUpdateCount, 0);
    auto pPagesBuffer = Buffer::create(mpDevice, totalPagesToUpdateCount, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess, Buffer::CpuAccess::None, pagesInitDataBuffer.data());
    mpVars->setBuffer("resolvedPagesBuff", pPagesBuffer);

    mpVars["PerFrameCB"]["gRenderTargetDim"] = float2(mpFbo->getWidth(), mpFbo->getHeight());
    mpVars["PerFrameCB"]["materialsToResolveCount"] = materialsResolveBuffer.size();
    mpVars["PerFrameCB"]["resolvedTexturesCount"] = resolvedTexturesCount;

    LOG_WARN("%u textures needs to be resolved", resolvedTexturesCount);

    mpScene->render(pContext, mpState.get(), mpVars.get());
    pContext->flush(true);

    // Test resolved data
    const int8_t* pOutPagesData = reinterpret_cast<const int8_t*>(pPagesBuffer->map(Buffer::MapType::Read));

    // dev print pages
    //std::cout << "Page ids: \n";
    //for( uint32_t i = 0; i < totalPagesToUpdateCount; i++) {
    //    std::cout << static_cast<int16_t>(pOutPagesData[i]) << " ";
    //}
    //std::cout << "\n\n";

    // test count pages
    std::vector<int8_t> page_flags(pOutPagesData, pOutPagesData + sizeof(int8_t) * totalPagesToUpdateCount);
    uint32_t pagesToLoadCount = std::count(page_flags.begin(), page_flags.end(), 1);
    LOG_WARN("%u pages needs to be loaded", pagesToLoadCount);

    // test load pages
    auto started = std::chrono::high_resolution_clock::now();

    uint32_t pagesStartOffset = 0;
    for (auto const& [textureID, pTexture] :texturesMap) {
        uint32_t texturePagesCount = pTexture->getSparsePagesCount();
        LOG_DBG("Analyzing %u pages for texture %u", texturePagesCount, textureID);

        std::vector<uint32_t> pageIDs;

        // index 'i' is a page index relative to the texture. starts with 0
        for(uint32_t i = 0; i < texturePagesCount; i++) {

            if (pOutPagesData[i + pagesStartOffset] != 0)
                pageIDs.push_back(i + pagesStartOffset);
        }
        mpDevice->resourceManager()->loadPages(pTexture, pageIDs); 

        pagesStartOffset += texturePagesCount;
    }
    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Pages loading done in: " << std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count() << std::endl;
    std::cout << "TexturesResolvePass::execute done in: " << std::chrono::duration_cast<std::chrono::milliseconds>(done-exec_started).count() << std::endl;

    LOG_DBG("TexturesResolvePass::execute done");
}

TexturesResolvePass& TexturesResolvePass::setDepthStencilState(const DepthStencilState::SharedPtr& pDsState) {
    mpState->setDepthStencilState(pDsState);
    return *this;
}

TexturesResolvePass& TexturesResolvePass::setRasterizerState(const RasterizerState::SharedPtr& pRsState) {
    mpRsState = pRsState;
    mpState->setRasterizerState(mpRsState);
    return *this;
}

void TexturesResolvePass::renderUI(Gui::Widgets& widget) {

}
