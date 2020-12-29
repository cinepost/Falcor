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
#include <array>

#include "scene_builder.h"
#include "lava_utils_lib/logging.h"

#include "reader_bgeo/bgeo/Run.h"
#include "reader_bgeo/bgeo/Poly.h"
#include "reader_bgeo/bgeo/PrimType.h"
#include "reader_bgeo/bgeo/parser/Detail.h"

namespace lava {    

SceneBuilder::SceneBuilder(Falcor::Device::SharedPtr pDevice, Flags buildFlags): Falcor::SceneBuilder(pDevice, buildFlags) {

    //mCamera.pObject = Falcor::Camera::create();

    mpDefaultMatreial = Material::create(pDevice, "default");
    
    mpDefaultMatreial->setBaseColor({0.5, 0.5, 0.5, 1.0});
    mpDefaultMatreial->setRoughness(0.3);

    mpDefaultMatreial->setIndexOfRefraction(1.5);

    //mpDefaultMatreial->setEmissiveColor({1.0, 0.5, 0.0});
    mpDefaultMatreial->setEmissiveFactor(0.0);
    
    //mpDefaultMatreial->setBaseColor({1.0, 1.0, 1.0, 1.0});
    //mpDefaultMatreial->loadTexture(Falcor::Material::TextureSlot::BaseColor, "/mnt/work/stock/3d_models/Eagle/textures/pbr_set_3/eagle_01_DefaultMaterial_BaseColor.jpg");
    //mpDefaultMatreial->loadTexture(Falcor::Material::TextureSlot::Specular, "/mnt/work/stock/3d_models/Eagle/textures/pbr_set_3/eagle_01_DefaultMaterial_RoughnessMetallic.jpg");
    //mpDefaultMatreial->loadTexture(Falcor::Material::TextureSlot::Normal, "/mnt/work/stock/3d_models/Eagle/textures/pbr_set_3/eagle_01_DefaultMaterial_Normal.jpg");
}

SceneBuilder::SharedPtr SceneBuilder::create(Falcor::Device::SharedPtr pDevice, Flags buildFlags) {
    return SharedPtr(new SceneBuilder(pDevice, buildFlags));
}

Falcor::Scene::SharedPtr SceneBuilder::getScene() {
    return Falcor::SceneBuilder::getScene();
}

//  int64_t getPointCount() const;
//  int64_t getTotalVertexCount() const;
//  int64_t getPrimitiveCount() const;

/*
    UnknownPrimType = 0,
    PolyPrimType,
    RunPrimType,
    SpherePrimType,
    VolumePrimType
*/

/*
// SceneBuilder::Mesh description

struct Mesh {
    std::string name;                           // The mesh's name
    uint32_t vertexCount = 0;                   // The number of vertices the mesh has
    uint32_t indexCount = 0;                    // The number of indices the mesh has. Can't be zero - the scene doesn't support non-indexed meshes. If you'd like us to support non-indexed meshes, please open an issue
    const uint32_t* pIndices = nullptr;         // Array of indices. The element count must match `indexCount`
    const float3* pPositions = nullptr;         // Array of vertex positions. The element count must match `vertexCount`. This field is required
    const float3* pNormals = nullptr;           // Array of vertex normals. The element count must match `vertexCount`.   This field is required
    const float3* pBitangents = nullptr;        // Array of vertex bitangent. The element count must match `vertexCount`. Optional. If set to nullptr, or if BuildFlags::UseOriginalTangentSpace is not set, the tangents will be generated using MikkTSpace
    const float2* pTexCrd = nullptr;            // Array of vertex texture coordinates. The element count must match `vertexCount`. This field is required
    const uint4* pBoneIDs = nullptr;            // Array of bone IDs. The element count must match `vertexCount`. This field is optional. If it's set, that means that the mesh is animated, in which case pBoneWeights can't be nullptr
    const float4*  pBoneWeights = nullptr;      // Array of bone weights. The element count must match `vertexCount`. This field is optional. If it's set, that means that the mesh is animated, in which case pBoneIDs can't be nullptr
    Vao::Topology topology = Vao::Topology::Undefined; // The primitive topology of the mesh
    Material::SharedPtr pMaterial;              // The mesh's material. Can't be nullptr
};
*/

uint32_t SceneBuilder::addMesh(const ika::bgeo::Bgeo& bgeo, const std::string& name) {
    LLOG_DBG << "adding mesh from bgeo";

    Mesh mesh;
    mesh.name = name;

    const int64_t bgeo_point_count = bgeo.getPointCount();
    const int64_t bgeo_vertex_count = bgeo.getTotalVertexCount();

    LLOG_DBG << "bgeo point count: " << bgeo_point_count;
    LLOG_DBG << "bgeo total vertex count: " << bgeo_vertex_count;
    LLOG_DBG << "bgeo prim count: " << bgeo.getPrimitiveCount();
    LLOG_DBG << "------------------------------------------------";

    // get basic bgeo data P, N, UV

    std::vector<float> P;
    bgeo.getP(P);
    assert(P.size() / 3 == bgeo_point_count && "P positions count not equal to the bgeo points count !!!");
    LLOG_DBG << "P<float> size: " << P.size();

    std::vector<float> N;
    bgeo.getPointN(N);
    LLOG_DBG << "N<float> size: " << N.size();
    
    std::vector<float> UV;
    bgeo.getPointUV(UV);
    LLOG_DBG << "UV<float> size: " << UV.size();

    std::vector<float> vN;
    bgeo.getVertexN(vN);
    LLOG_DBG << "vN<float> size: " << vN.size();
    
    std::vector<float> vUV;
    bgeo.getVertexUV(vUV);
    LLOG_DBG << "vUV<float> size: " << vUV.size();

    bool unique_points = false; // separate points only if we have any vertex data present
    if(vN.size() || vUV.size()) unique_points = true; 

    // separated verices data arrays
    std::vector<float3> positions;
    std::vector<float3> normals;
    std::vector<float2> uv_coords;
    
    // build separated verices data
    if(unique_points) {
        auto pDetail = bgeo.getDetail();
        auto const& vt_map = pDetail->getVertexMap();

        assert(vt_map.vertexCount == bgeo_vertex_count && "Bgeo detail vertices count not equal to the number of bgeo vertices count !!!");

        ika::bgeo::parser::int32 point_idx;
        const ika::bgeo::parser::int32* vt_idx_ptr = vt_map.getVertices();
        
        if( unique_points) {
            positions.resize(bgeo_vertex_count);
            // fill in vertex positions
            for( ika::bgeo::parser::int64 i = 0; i < vt_map.getVertexCount(); i++){
                point_idx = vt_idx_ptr[i] * 3;
                positions[i] = {P[point_idx], P[point_idx+1], P[point_idx+2]};
            }
        }

        // fill in normals
        if (vN.size() || N.size()) normals.resize(bgeo_vertex_count);
        if (vN.size()) {
            size_t ii = 0;
            assert(vN.size() / 3 == bgeo_vertex_count && "Vertex normals count not equal to the number of bgeo verices count !!!");
            // use vertex normals
            for( size_t i = 0; i < bgeo_vertex_count; i++){
                normals[i] = {vN[ii], vN[ii+1], vN[ii+2]};
                ii += 3;
            }
        } else if (N.size() && unique_points) {
            // use point normals
            assert(N.size() == P.size() && "Point normals count not equal to the number of bgeo points !!!");

            for( ika::bgeo::parser::int64 i = 0; i < vt_map.getVertexCount(); i++){
                point_idx = vt_idx_ptr[i] * 3;
                normals[i] = {N[point_idx], N[point_idx+1], N[point_idx+2]};
            }
        }

        // fill in texture coordinates
        if (vUV.size() || UV.size()) uv_coords.resize(bgeo_vertex_count);
        if (vUV.size()) {
            size_t ii = 0;
            assert(vUV.size()/2 == bgeo_vertex_count && "Vertex texture coordinates count not equal to the number of bgeo verices count !!!");
            // use vertex normals
            for( size_t i = 0; i < bgeo_vertex_count; i++){
                uv_coords[i] = {vUV[ii], 1.0 - vUV[ii+1]};
                ii += 2;
            }
        } else if (UV.size() && unique_points) {
            // use point normals
            assert(UV.size()/2 == P.size()/3 && "Point texture coordinates count not equal to the number of bgeo points !!!");

            for( ika::bgeo::parser::int64 i = 0; i < vt_map.getVertexCount(); i++){
                point_idx = vt_idx_ptr[i]*2;
                uv_coords[i] = {UV[point_idx], 1.0 - UV[point_idx+1]};
            }
        } else {
            // no coords provided from bgeo. fill with zeroes as this field is required
            for(auto& uv: uv_coords) {
                uv = {0.0, 0.0};
            }
        }
    }

    // process primitives and build indices

    uint32_t face_count = 0;
    std::vector<uint32_t> indices;
    std::vector<int32_t> vtx_list;
    std::vector<int32_t> prim_start_indices;

    for(uint32_t p_i=0; p_i < bgeo.getPrimitiveCount(); p_i++) {
        std::cout << "Processing primitive number " << p_i << "\n";

        const auto& pPrim = bgeo.getPrimitive(p_i);
        if(!pPrim) {
            LLOG_WRN << "Unable to get primitive number: " << p_i;
            continue;
        }

        const ika::bgeo::Poly* pPoly;
        prim_start_indices.clear();

        switch (pPrim->getType()) {
            case ika::bgeo::PrimType::PolyPrimType:
                pPoly = std::dynamic_pointer_cast<ika::bgeo::Poly>(pPrim).get();

                if (unique_points) {
                    
                    pPoly->getStartIndices(prim_start_indices);

                    // process faces
                    int32_t csi; // face current start index
                    int32_t nsi; // next face start index
                    for( int32_t i = 0; i < (prim_start_indices.size() - 1); i++){
                        csi = prim_start_indices[i];
                        nsi = prim_start_indices[i+1];
                        switch(nsi-csi) { // number of face sides literally
                            case 0:
                            case 1:
                            case 2:
                                LLOG_ERR << "Polygon sides count should be 3 or more !!!";
                                break;
                            case 3:
                                indices.push_back(csi+2);
                                indices.push_back(csi+1);
                                indices.push_back(csi);
                                face_count += 1;
                                break;
                            case 4:
                                indices.push_back(csi+2);
                                indices.push_back(csi+1);
                                indices.push_back(csi);

                                indices.push_back(csi+3);
                                indices.push_back(csi+2);
                                indices.push_back(csi);
                                face_count += 2;
                                break;
                            case 5:
                                indices.push_back(csi+2);
                                indices.push_back(csi+1);
                                indices.push_back(csi);

                                indices.push_back(csi+3);
                                indices.push_back(csi+2);
                                indices.push_back(csi);

                                indices.push_back(csi+4);
                                indices.push_back(csi+3);
                                indices.push_back(csi);
                                face_count += 3;
                                break;
                            default:
                                LLOG_WRN << "Poly sides more than 5 unsupported for now !";
                                break;
                        }
                    }
                    LLOG_DBG << "prim vertex count: " << pPoly->getVertexCount();
                    LLOG_DBG << "prim faces count: " << pPoly->getFaceCount();
                } else {
                    const auto& sides_list = pPoly->getSidesList();

                    // process faces
                    uint64_t cur_i = 0;
                    for(const auto& sides: sides_list){
                        switch(sides) {
                            case 0:
                            case 1:
                            case 2:
                                LLOG_ERR << "Polygon sides count should be 3 or more !!!";
                                break;
                            case 3:
                                indices.push_back(vtx_list[cur_i+2]);
                                indices.push_back(vtx_list[cur_i+1]);
                                indices.push_back(vtx_list[cur_i]);
                                face_count += 1;
                                break;
                            case 4:
                                indices.push_back(vtx_list[cur_i+2]);
                                indices.push_back(vtx_list[cur_i+1]);
                                indices.push_back(vtx_list[cur_i]);

                                indices.push_back(vtx_list[cur_i+3]);
                                indices.push_back(vtx_list[cur_i+2]);
                                indices.push_back(vtx_list[cur_i]);
                                face_count += 2;
                                break;
                            case 5:
                                indices.push_back(vtx_list[cur_i+2]);
                                indices.push_back(vtx_list[cur_i+1]);
                                indices.push_back(vtx_list[cur_i]);

                                indices.push_back(vtx_list[cur_i+3]);
                                indices.push_back(vtx_list[cur_i+2]);
                                indices.push_back(vtx_list[cur_i]);

                                indices.push_back(vtx_list[cur_i+4]);
                                indices.push_back(vtx_list[cur_i+3]);
                                indices.push_back(vtx_list[cur_i]);
                                face_count += 3;
                                break;
                            default:
                                LLOG_WRN << "Poly sides more than 5 unsupported for now !";
                                break;
                        }

                        cur_i += sides;
                    }
                }

                break;
            default:
                LLOG_WRN << "Unsupported prim type \"" + std::string(pPrim->getStrType()) + "\" !!!";
                break;
        }
    }

    mesh.pMaterial = mpDefaultMatreial;
    mesh.faceCount = face_count;

    mesh.positions.frequency = Mesh::AttributeFrequency::Vertex;
    if(unique_points) {
        mesh.vertexCount = positions.size();
        mesh.positions.pData = (float3*)positions.data();
    } else {
        mesh.vertexCount = P.size() / 3;
        mesh.positions.pData = (float3*)P.data();
    }

    mesh.indexCount = indices.size();
    mesh.pIndices = (uint32_t*)indices.data();
    
    mesh.normals.frequency = Mesh::AttributeFrequency::Vertex;
    if(unique_points) {
        mesh.normals.pData = (float3*)normals.data();
    } else {
        mesh.normals.pData = (float3*)N.data();
    }

    mesh.texCrds.frequency = Mesh::AttributeFrequency::Vertex;
    if(unique_points) {
        mesh.texCrds.pData = (float2*)uv_coords.data();
    } else {
        mesh.texCrds.pData = (float2*)UV.data();
    }

    //mesh.pBoneIDs = nullptr;
    //mesh.pBoneWeights = nullptr;
    mesh.topology = Falcor::Vao::Topology::TriangleList;

    return Falcor::SceneBuilder::addMesh(mesh);
}

void SceneBuilder::finalize() {
    mDirty = true;
    getScene();
}


}  // namespace lava
