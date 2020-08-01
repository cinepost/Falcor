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
#include "Falcor/stdafx.h"
#include "Sampler.h"

namespace Falcor {

struct SamplerData {
    uint32_t objectCount = 0;
    Sampler::SharedPtr pDefaultSampler;
};
SamplerData gSamplerData;

Sampler::Sampler(std::shared_ptr<Device> device, const Desc& desc) : mDesc(desc), mpDevice(device) {
    gSamplerData.objectCount++;
}

Sampler::~Sampler() {
    gSamplerData.objectCount--;
    if (gSamplerData.objectCount <= 1) gSamplerData.pDefaultSampler = nullptr;
}

Sampler::Desc& Sampler::Desc::setFilterMode(Filter minFilter, Filter magFilter, Filter mipFilter) {
    mMagFilter = magFilter;
    mMinFilter = minFilter;
    mMipFilter = mipFilter;
    return *this;
}

Sampler::Desc& Sampler::Desc::setMaxAnisotropy(uint32_t maxAnisotropy) {
    mMaxAnisotropy = maxAnisotropy;
    return *this;
}

Sampler::Desc& Sampler::Desc::setLodParams(float minLod, float maxLod, float lodBias) {
    mMinLod = minLod;
    mMaxLod = maxLod;
    mLodBias = lodBias;
    return *this;
}

Sampler::Desc& Sampler::Desc::setComparisonMode(ComparisonMode mode) {
    mComparisonMode = mode;
    return *this;
}

Sampler::Desc& Sampler::Desc::setAddressingMode(AddressMode modeU, AddressMode modeV, AddressMode modeW) {
    mModeU = modeU;
    mModeV = modeV;
    mModeW = modeW;
    return *this;
}

Sampler::Desc& Sampler::Desc::setBorderColor(const float4& borderColor) {
    mBorderColor = borderColor;
    return *this;
}

Sampler::SharedPtr Sampler::getDefault(std::shared_ptr<Device> device) {
    if (gSamplerData.pDefaultSampler == nullptr) {
        gSamplerData.pDefaultSampler = create(device, Desc());
    }
    return gSamplerData.pDefaultSampler;
}

SCRIPT_BINDING(Sampler) {
    m.regClass(Sampler);

    auto filter = m.enum_<Sampler::Filter>("SamplerFilter");
    filter.regEnumVal(Sampler::Filter::Linear).regEnumVal(Sampler::Filter::Point);

    auto addressing = m.enum_<Sampler::AddressMode>("AddressMode");
    addressing.regEnumVal(Sampler::AddressMode::Wrap).regEnumVal(Sampler::AddressMode::Mirror).regEnumVal(Sampler::AddressMode::Clamp);
    addressing.regEnumVal(Sampler::AddressMode::Border).regEnumVal(Sampler::AddressMode::MirrorOnce);
}

}  // namespace Falcor
