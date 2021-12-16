/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "../Helper/Helper.hpp"
#include "PatchConstructor.hpp"


typedef Matrix Mat9x9d;

class RegularPatchConstructor : public PatchConstructor
{
public:
    RegularPatchConstructor(const MeshType& a_Mesh) : m_Mesh(a_Mesh) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle) override;
    std::vector<Patch> getPatch(const VertexHandle& a_VertexHandle) override;

    Patch getPatch(const std::vector<VertexHandle>& a_NBVertexHandles);
    Matrix getPatchMat(const std::vector<VertexHandle>& a_NBVertexHandles);

private:
    const MeshType& m_Mesh;
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

    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle);

};
