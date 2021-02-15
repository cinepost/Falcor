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
#ifndef SRC_FALCOR_RENDERPASSES_SKYBOX_SKYBOX_H_
#define SRC_FALCOR_RENDERPASSES_SKYBOX_SKYBOX_H_

#include "Falcor/Falcor.h"
#include "FalcorExperimental.h"
#include "Falcor/Core/API/Device.h"

using namespace Falcor;

class SkyBox : public RenderPass, public inherit_shared_from_this<RenderPass, SkyBox> {
 public:
    using SharedPtr = std::shared_ptr<SkyBox>;
    using inherit_shared_from_this::shared_from_this;
    static const char* kDesc;

    static SharedPtr create(RenderContext* pRenderContext = nullptr, const Dictionary& dict = {});

    std::string getDesc() override { return kDesc; }
    virtual Dictionary getScriptingDictionary() override;
    virtual RenderPassReflection reflect(const CompileData& compileData) override;
    virtual void execute(RenderContext* pRenderContext, const RenderData& renderData) override;
    virtual void setScene(RenderContext* pRenderContext, const Scene::SharedPtr& pScene) override;
    virtual void renderUI(Gui::Widgets& widget) override;

    void setIntensity(float intensity) { mIntensity = intensity; }
    float getIntensity() const { return mIntensity; }
    void setScale(float scale) { mScale = scale; }
    void setFilter(uint32_t filter);
    float getScale() { return mScale; }
    uint32_t getFilter() { return (uint32_t)mFilter; }
    void setTexture(const std::string& texName, bool loadAsSrgb = true);

 private:
    SkyBox(Device::SharedPtr pDevice);
    void loadImage();
    void setTexture(const Texture::SharedPtr& pTexture);

    float mIntensity = 1.0;
    float mScale = 1;
    bool mLoadSrgb = true;
    Sampler::Filter mFilter = Sampler::Filter::Linear;
    Texture::SharedPtr mpTexture;
    std::string mTexName;

    Scene::SharedPtr mpCubeScene;
    GraphicsProgram::SharedPtr mpProgram;
    GraphicsVars::SharedPtr mpVars;
    GraphicsState::SharedPtr mpState;
    Fbo::SharedPtr mpFbo;
    Scene::SharedPtr mpScene;
    Sampler::SharedPtr mpSampler;
};

#endif  // SRC_FALCOR_RENDERPASSES_SKYBOX_SKYBOX_H_
