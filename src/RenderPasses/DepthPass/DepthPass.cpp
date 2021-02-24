/***************************************************************************
 # Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#include "DepthPass.h"

// Don't remove this. it's required for hot-reload to function properly
extern "C" falcorexport const char* getProjDir() {
    return PROJECT_DIR;
}

extern "C" falcorexport void getPasses(Falcor::RenderPassLibrary& lib) {
    lib.registerClass("DepthPass", "Creates a depth-buffer using the scene's active camera", DepthPass::create);
}

const char* DepthPass::kDesc = "Creates a depth-buffer using the scene's active camera";

namespace {
    const std::string kProgramFile = "RenderPasses/DepthPass/DepthPass.ps.slang";

    const std::string kDepth = "depth";
    const std::string kDepthFormat = "depthFormat";
}  // namespace

void DepthPass::parseDictionary(const Dictionary& dict) {
    for (const auto& [key, value] : dict)
    {
        if (key == kDepthFormat) setDepthBufferFormat(value);
        else logWarning("Unknown field '" + key + "' in a DepthPass dictionary");
    }
}

Dictionary DepthPass::getScriptingDictionary() {
    Dictionary d;
    d[kDepthFormat] = mDepthFormat;
    return d;
}

DepthPass::SharedPtr DepthPass::create(RenderContext* pRenderContext, const Dictionary& dict) {
    return SharedPtr(new DepthPass(pRenderContext->device(), dict));
}

DepthPass::DepthPass(Device::SharedPtr pDevice, const Dictionary& dict): RenderPass(pDevice) {
    Program::Desc desc;
    desc.addShaderLibrary(kProgramFile).psEntry("main");
    GraphicsProgram::SharedPtr pProgram = GraphicsProgram::create(pDevice, desc);
    mpState = GraphicsState::create(pDevice);
    mpState->setProgram(pProgram);
    mpFbo = Fbo::create(pDevice);

    parseDictionary(dict);
}

RenderPassReflection DepthPass::reflect(const CompileData& compileData) {
    RenderPassReflection reflector;
    reflector.addOutput(kDepth, "Depth-buffer").bindFlags(Resource::BindFlags::DepthStencil).format(mDepthFormat).texture2D(0, 0, 0);
    return reflector;
}

void DepthPass::setScene(RenderContext* pRenderContext, const Scene::SharedPtr& pScene) {
    mpScene = pScene;
    if (mpScene) mpState->getProgram()->addDefines(mpScene->getSceneDefines());
    mpVars = GraphicsVars::create(pRenderContext->device(), mpState->getProgram()->getReflector());
}

void DepthPass::execute(RenderContext* pContext, const RenderData& renderData) {
    const auto& pDepth = renderData[kDepth]->asTexture();
    mpFbo->attachDepthStencilTarget(pDepth);

    mpState->setFbo(mpFbo);
    pContext->clearDsv(pDepth->getDSV().get(), 1, 0);

    if (mpScene) mpScene->render(pContext, mpState.get(), mpVars.get(), Scene::RenderFlags::UserRasterizerState);
}

DepthPass& DepthPass::setDepthBufferFormat(ResourceFormat format) {
    if (isDepthStencilFormat(format) == false) {
        logWarning("DepthPass buffer format must be a depth-stencil format");
    } else {
        mDepthFormat = format;
        mPassChangedCB();
    }
    return *this;
}

DepthPass& DepthPass::setDepthStencilState(const DepthStencilState::SharedPtr& pDsState) {
    mpState->setDepthStencilState(pDsState);
    return *this;
}

DepthPass& DepthPass::setRasterizerState(const RasterizerState::SharedPtr& pRsState) {
    mpRsState = pRsState;
    mpState->setRasterizerState(mpRsState);
    return *this;
}