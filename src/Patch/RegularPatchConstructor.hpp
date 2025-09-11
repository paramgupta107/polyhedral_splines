/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "../Helper/Helper.hpp"
#include "PatchConstructor.hpp"


typedef Matrix Mat9x9d;

class RegularPatchConstructor : public PatchConstructor
{
public:
    RegularPatchConstructor() {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

    Patch getPatch(const std::vector<VertexHandle>& a_NBVertexHandles, MeshType& a_Mesh);
    Matrix getPatchMat(const std::vector<VertexHandle>& a_NBVertexHandles, MeshType& a_Mesh);

private:
    const Mat9x9d m_Mask = Matrix({
        {0.25, 0.25, 0, 0.25, 0.25, 0, 0, 0, 0},
        {0, 0.5, 0, 0, 0.5, 0, 0, 0, 0},
        {0, 0.25, 0.25, 0, 0.25, 0.25, 0, 0, 0},
        {0, 0, 0, 0.5, 0.5, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0.5, 0.5, 0, 0, 0},
        {0, 0, 0, 0.25, 0.25, 0, 0.25, 0.25, 0},
        {0, 0, 0, 0, 0.5, 0, 0, 0.5, 0},
        {0, 0, 0, 0, 0.25, 0.25, 0, 0.25, 0.25}
    });

    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh);

    std::string getGroupName() const;
};
