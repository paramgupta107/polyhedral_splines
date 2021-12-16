/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat256x20d;

class T2PatchConstructor : public PatchConstructor
{
public:
    T2PatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh), m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle) override;
    std::vector<Patch> getPatch(const FaceHandle& a_FaceHandle) override;

private:
    const MeshType& m_Mesh;
    const Mat256x20d m_Mask;

    Mat256x20d getMask();
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle);

};
