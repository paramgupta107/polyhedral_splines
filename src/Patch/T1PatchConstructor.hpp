/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat128x18d;

class T1PatchConstructor : public PatchConstructor
{
public:
    T1PatchConstructor()
        : m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

private:
    const Mat128x18d m_Mask;

    Mat128x18d getMask();
    bool isPentagonTjunction(const FaceHandle& a_FaceHandle, MeshType& a_Mesh);
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, MeshType& a_Mesh);

    std::string getGroupName() const;
};
