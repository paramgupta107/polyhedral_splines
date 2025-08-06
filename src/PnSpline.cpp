/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "PnSpline.hpp"

void PnSpline::initialize(){
    patchBuilders = getPatchBuilders(controlMesh);
    vertexToPatchBuilder.clear();
    patchBuilderToPatchIndex.clear();
    patches.clear();
    for (uint i = 0; i < patchBuilders.size(); ++i) {
        auto patchBuilder = patchBuilders[i];
        patchBuilderToPatchIndex[i] = patches.size();
        for (const auto& vertex : patchBuilder.getNeighborVerts()){
            vertexToPatchBuilder[vertex.idx()].insert(i);
        }
        auto t_patches = patchBuilder.buildPatches(controlMesh);
        patches.insert(patches.end(), t_patches.begin(), t_patches.end());
    }
}

PnSpline::PnSpline(MeshType& controlMesh)
    : controlMesh(controlMesh) {
    initialize();
}

PnSpline::PnSpline(std::vector<std::array<double,3>>& controlPoints, std::vector<std::vector<uint>>& controlIndices) {
    for (const auto& point : controlPoints) {
        controlMesh.add_vertex({point[0], point[1], point[2]});
    }
    for (const auto& face : controlIndices) {
        std::vector<MeshType::VertexHandle> face_vhandles;
        for (auto idx : face) {
            face_vhandles.push_back(controlMesh.vertex_handle(idx));
        }
        controlMesh.add_face(face_vhandles);
    }
    initialize();
}
    

PnSpline::PnSpline(const PnSpline& src)
    : controlMesh(src.controlMesh), patches(src.patches), patchBuilders(src.patchBuilders),
      vertexToPatchBuilder(src.vertexToPatchBuilder), patchBuilderToPatchIndex(src.patchBuilderToPatchIndex) {}

PnSpline& PnSpline::operator=(const PnSpline& src) {
    if (this != &src) {
        controlMesh = src.controlMesh;
        patches = src.patches;
        patchBuilders = src.patchBuilders;
        vertexToPatchBuilder = src.vertexToPatchBuilder;
        patchBuilderToPatchIndex = src.patchBuilderToPatchIndex;
    }
    return *this;
}

std::vector<uint> PnSpline::updateControlMesh(std::vector<std::array<double,3>>& updatedControlPoints, std::vector<uint>& updateIndices) {
    std::set<uint> affectedPatchBuilders;
    for (int i = 0; i < updateIndices.size(); ++i) {
        int updateIdx = updateIndices[i];
        controlMesh.set_point(controlMesh.vertex_handle(updateIdx), {updatedControlPoints[i][0], updatedControlPoints[i][1], updatedControlPoints[i][2]});
        auto currAffectedPatchesBuilders = vertexToPatchBuilder[updateIdx];
        affectedPatchBuilders.insert(currAffectedPatchesBuilders.begin(), currAffectedPatchesBuilders.end());
    }
    std::vector<uint> updatedPatches;
    for (auto patchBuilderIdx : affectedPatchBuilders) {
        auto& patchBuilder = patchBuilders[patchBuilderIdx];
        auto t_patches = patchBuilder.buildPatches(controlMesh);
        for (uint i = 0; i < t_patches.size(); ++i) {
            uint updatePatchIdx = patchBuilderToPatchIndex[patchBuilderIdx] + i;
            patches[updatePatchIdx] = t_patches[i];
            updatedPatches.push_back(updatePatchIdx);
        }
    }
    return updatedPatches;
}