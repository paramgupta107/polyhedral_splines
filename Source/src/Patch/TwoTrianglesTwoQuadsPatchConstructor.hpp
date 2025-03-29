/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat9x9d;
typedef Matrix Mat9x3d;

class TwoTrianglesTwoQuadsPatchConstructor : public PatchConstructor
{
public:
    TwoTrianglesTwoQuadsPatchConstructor() {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh) override;
    PatchBuilder getPatchBuilder(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh) override;

    Patch getPatch(const std::vector<VertexHandle>& a_NBVertexHandles, const MeshType& a_Mesh);
    Mat9x3d getPatchMat(const std::vector<VertexHandle>& a_NBVertexHandles, const MeshType& a_Mesh);

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

    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh);

    std::string getGroupName() const;
};
