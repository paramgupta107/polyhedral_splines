/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat64x14d;

class T0PatchConstructor : public PatchConstructor
{
public:
    T0PatchConstructor()
        : m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh) override;

private:
    const Mat64x14d m_Mask;

    Mat64x14d getMask();
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh);

    std::string getGroupName() const;
};
