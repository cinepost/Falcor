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
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
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
#include "stdafx.h"
#include "EnvMap.h"
#include "glm/gtc/integer.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "Core/API/ResourceManager.h"

namespace Falcor {

EnvMap::SharedPtr EnvMap::create(std::shared_ptr<Device> pDevice, const std::string& filename) {
    return SharedPtr(new EnvMap(pDevice, filename));
}

EnvMap::SharedPtr EnvMap::create(std::shared_ptr<Device> pDevice, Texture::SharedPtr pTexture) {
    if(!pTexture) return nullptr;
    return SharedPtr(new EnvMap(pDevice, pTexture));
}

void EnvMap::setRotation(float3 degreesXYZ) {
    if (degreesXYZ != mRotation) {
        mRotation = degreesXYZ;

        auto rotX = glm::eulerAngleX(glm::radians(mRotation.x));
        auto rotY = glm::eulerAngleY(glm::radians(mRotation.y));
        auto rotZ = glm::eulerAngleZ(glm::radians(mRotation.z));

        auto transform = rotZ * rotY * rotX;

        mData.transform = static_cast<float3x4>(transform);
        mData.invTransform = static_cast<float3x4>(glm::inverse(transform));
    }
}

void EnvMap::setIntensity(float intensity) {
    mData.intensity = intensity;
}

void EnvMap::setTint(const float3& tint) {
    mData.tint = tint;
}

void EnvMap::setShaderData(const ShaderVar& var) const {
    assert(var.isValid());

    // Set variables.
    var["data"].setBlob(mData);

    // Bind resources.
    var["envMap"].setTexture(mpEnvMap);
    var["envSampler"].setSampler(mpEnvSampler);
}

EnvMap::Changes EnvMap::beginFrame() {
    mChanges = Changes::None;

    if (mData.transform != mPrevData.transform) mChanges |= Changes::Transform;
    if (mData.intensity != mPrevData.intensity) mChanges |= Changes::Intensity;
    if (mData.tint != mPrevData.tint) mChanges |= Changes::Intensity;

    mPrevData = mData;

    return getChanges();
}

EnvMap::EnvMap(std::shared_ptr<Device> pDevice):mpDevice(pDevice) {
    // Create sampler.
    // The lat-long map wraps around horizontally, but not vertically. Set the sampler to only wrap in U.
    Sampler::Desc samplerDesc;
    samplerDesc.setFilterMode(Sampler::Filter::Linear, Sampler::Filter::Linear, Sampler::Filter::Linear);
    samplerDesc.setAddressingMode(Sampler::AddressMode::Wrap, Sampler::AddressMode::Clamp, Sampler::AddressMode::Clamp);
    mpEnvSampler = Sampler::create(pDevice, samplerDesc);
}

EnvMap::EnvMap(std::shared_ptr<Device> pDevice, Texture::SharedPtr pTexture):EnvMap(pDevice) {
    mpEnvMap = pTexture;
}

EnvMap::EnvMap(std::shared_ptr<Device> pDevice, const std::string& filename):EnvMap(pDevice) {
    // Load environment map from file. Set it to generate mips and use linear color.
    //mpEnvMap = Texture::createFromFile(pDevice, filename, true, false);
    mpEnvMap = nullptr;
    auto pResourceManager = pDevice->resourceManager();
    if (pResourceManager) {
        mpEnvMap = pResourceManager->createTextureFromFile(filename, true, false);
    }

    //mpEnvMap = Texture::createFromFile(pDevice, filename, true, false);
    if (!mpEnvMap) throw std::runtime_error("Failed to load environment map texture");
}

uint64_t EnvMap::getMemoryUsageInBytes() const {
    return mpEnvMap ? mpEnvMap->getTextureSizeInBytes() : 0;
}


#ifdef SCRIPTING
SCRIPT_BINDING(EnvMap) {
    pybind11::class_<EnvMap, EnvMap::SharedPtr> envMap(m, "EnvMap");
    envMap.def_property_readonly("filename", &EnvMap::getFilename);
    envMap.def_property("rotation", &EnvMap::getRotation, &EnvMap::setRotation);
    envMap.def_property("intensity", &EnvMap::getIntensity, &EnvMap::setIntensity);
    envMap.def_property("tint", &EnvMap::getTint, &EnvMap::setTint);
}
#endif

}  // namespace Falcor
