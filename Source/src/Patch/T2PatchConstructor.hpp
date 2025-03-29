/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat256x20d;

class T2PatchConstructor : public PatchConstructor
{
public:
    T2PatchConstructor()
        : m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh) override;

private:
    const Mat256x20d m_Mask;

    Mat256x20d getMask();
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh);

    std::string getGroupName() const;
};
