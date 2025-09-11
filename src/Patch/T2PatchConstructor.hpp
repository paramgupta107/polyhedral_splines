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

    bool isSamePatchType(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

private:
    const Mat256x20d m_Mask;

    Mat256x20d getMask();
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, MeshType& a_Mesh);

    std::string getGroupName() const;
};
