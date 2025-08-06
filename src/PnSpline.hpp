/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include "Patch/PatchBuilder.hpp"
#include "Patch/Patch.hpp"
#include "ProcessMesh.hpp"
#include <unordered_map>
#include <set>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

class PnSpline {
    public:
        PnSpline() = default;
        PnSpline(MeshType& controlMesh);
        PnSpline(std::vector<std::array<double,3>>& controlPoints, std::vector<std::vector<uint>>& controlIndices);
        PnSpline(const PnSpline& src);
        PnSpline& operator=(const PnSpline& src);
        
        std::vector<uint> updateControlMesh(std::vector<std::array<double,3>>& updatedControlPoints, std::vector<uint>& updateIndices);

        MeshType controlMesh;
        std::vector<Patch> patches;
        void initialize();
    private:
        std::vector<PatchBuilder> patchBuilders;
        std::unordered_map<uint, std::set<uint>> vertexToPatchBuilder;
        std::unordered_map<uint, uint> patchBuilderToPatchIndex;
};