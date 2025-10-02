/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */


#pragma once

#include "Patch/PatchBuilder.hpp"
#include "Patch/Patch.hpp"
#include "ProcessMesh.hpp"
#include <unordered_map>
#include <set>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;


extern "C" {
struct PnSplineImpl{
    MeshType controlMesh;
    std::vector<Patch*> patches;
    std::vector<PatchBuilder> patchBuilders;
    std::unordered_map<uint32_t, std::set<uint32_t>> vertexToPatchBuilder;
    std::unordered_map<uint32_t, uint32_t> patchBuilderToPatchIndex;
    ~PnSplineImpl(){
        for (auto patch : patches) {
            if(patch)delete patch;
        }
    };
};


PnSplineImpl* PnSpline_create_empty() {
    return new PnSplineImpl();
};

static void initialize(PnSplineImpl* impl, bool gradientHandles = false) {
    if (gradientHandles) {
        impl->controlMesh = interpretGradientHandles(impl->controlMesh);
    }
    impl->patchBuilders = getPatchBuilders(impl->controlMesh);
    impl->vertexToPatchBuilder.clear();
    impl->patchBuilderToPatchIndex.clear();
    impl->patches.clear();
    for (uint i = 0; i < impl->patchBuilders.size(); ++i) {
        auto patchBuilder = impl->patchBuilders[i];
        impl->patchBuilderToPatchIndex[i] = impl->patches.size();
        for (const auto& vertex : patchBuilder.getNeighborVerts()){
            impl->vertexToPatchBuilder[vertex.idx()].insert(i);
        }
        auto t_patches = patchBuilder.buildPatches(impl->controlMesh);
        impl->patches.reserve(impl->patches.size() + t_patches.size());

        for (auto& p : t_patches) {
            impl->patches.push_back(new Patch(std::move(p)));
        }
    }
};

void PnSpline_degRaise(PnSplineImpl* impl) {
    for (auto& pb : impl->patchBuilders) {
        pb.degRaise();
    }
};

PnSplineImpl* PnSpline_create_from_points(const double* points, uint64_t numPoints,
                                          const uint32_t* faceIndices, const uint64_t* faceSizes, uint64_t numFaces,
                                          bool degRaise, bool gradientHandles) {

    PnSplineImpl* impl = new PnSplineImpl();

    for (uint64_t i = 0; i < numPoints; ++i) {
        impl->controlMesh.add_vertex({points[i * 3], points[i * 3 + 1], points[i * 3 + 2]});
    }

    uint64_t offset = 0;
    for (uint64_t f = 0; f < numFaces; ++f) {
        std::vector<MeshType::VertexHandle> vhandles;
        for (uint64_t j = 0; j < faceSizes[f]; ++j) {
            vhandles.push_back(impl->controlMesh.vertex_handle(faceIndices[offset+j]));
        }
        impl->controlMesh.add_face(vhandles);
        offset += faceSizes[f];
    }

    initialize(impl, gradientHandles);
    if (degRaise) {
        PnSpline_degRaise(impl);
    }

    return impl;
};

PnSplineImpl* PnSpline_clone(const PnSplineImpl* other) {
    auto* impl = new PnSplineImpl();
    impl->controlMesh = other->controlMesh;

    impl->patches.reserve(other->patches.size());
    for (auto* p : other->patches) {
        impl->patches.push_back(new Patch(*p));
    }

    impl->patchBuilders = other->patchBuilders;
    impl->vertexToPatchBuilder = other->vertexToPatchBuilder;
    impl->patchBuilderToPatchIndex = other->patchBuilderToPatchIndex;

    return impl;
}


void PnSpline_destroy(PnSplineImpl* impl) {
    if(impl) delete impl;
};

uint64_t PnSpline_updateControlMesh(PnSplineImpl* impl,
                                    const double* updatedPoints, uint64_t numPoints,
                                    const uint32_t* updateIndices, uint64_t numIndices,
                                    uint32_t* outPatchIndices, uint64_t maxOut) {
    std::set<uint32_t> affectedPBs;

    for (uint64_t i = 0; i < numIndices; ++i) {
        uint32_t vid = updateIndices[i];
        impl->controlMesh.set_point(
            impl->controlMesh.vertex_handle(vid),
            {updatedPoints[3*i+0], updatedPoints[3*i+1], updatedPoints[3*i+2]}
        );
        auto& affected = impl->vertexToPatchBuilder[vid];
        affectedPBs.insert(affected.begin(), affected.end());
    }

    std::vector<uint32_t> updated;
    for (auto pbIdx : affectedPBs) {
        auto& pb = impl->patchBuilders[pbIdx];
        auto rebuilt = pb.buildPatches(impl->controlMesh);
        for (uint32_t j = 0; j < rebuilt.size(); ++j) {
            uint32_t patchIdx = impl->patchBuilderToPatchIndex[pbIdx] + j;
            delete impl->patches[patchIdx];
            impl->patches[patchIdx] = new Patch(std::move(rebuilt[j]));
            updated.push_back(patchIdx);
        }
    }

    uint64_t n = updated.size();
    std::copy_n(updated.begin(), n, outPatchIndices);
    return n;
};


uint64_t PnSpline_getNumPatches(const PnSplineImpl* impl) {
    return impl->patches.size();
};

Patch* PnSpline_getPatch(const PnSplineImpl* impl, uint32_t index) {
    if (index >= impl->patches.size()) return nullptr;
    return impl->patches[index];
};
}