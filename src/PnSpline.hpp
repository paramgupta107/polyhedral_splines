/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include "Patch/PatchBuilder.hpp"
#include "Patch/Patch.hpp"
#include "ProcessMesh.hpp"
#include <unordered_map>
#include <set>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

class PnSpline {
    public:
        PnSpline() = default;
        PnSpline(MeshType& controlMesh, bool degRaise = false);
        PnSpline(std::vector<std::array<double,3>>& controlPoints, std::vector<std::vector<uint>>& controlIndices, bool degRaise = false);
        PnSpline(const PnSpline& src);
        PnSpline& operator=(const PnSpline& src);
        
        std::vector<uint> updateControlMesh(std::vector<std::array<double,3>>& updatedControlPoints, std::vector<uint>& updateIndices);

        void degRaise();

        MeshType controlMesh;
        std::vector<Patch> patches;
        void initialize();
    private:
        std::vector<PatchBuilder> patchBuilders;
        std::unordered_map<uint, std::set<uint>> vertexToPatchBuilder;
        std::unordered_map<uint, uint> patchBuilderToPatchIndex;
};