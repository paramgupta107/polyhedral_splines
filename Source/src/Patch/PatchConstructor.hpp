/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "Patch.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;
typedef MeshType::FaceHandle FaceHandle;

class PatchConstructor
{
public:
    virtual bool isSamePatchType(const VertexHandle&) { return false; };
    virtual bool isSamePatchType(const FaceHandle&) { return false; };
    virtual std::vector<Patch> getPatch(const VertexHandle&) { return {}; };
    virtual std::vector<Patch> getPatch(const FaceHandle&) { return {}; };
};
