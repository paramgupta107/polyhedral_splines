/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchBuilder.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;
typedef MeshType::FaceHandle FaceHandle;

class PatchConstructor
{
public:
    virtual bool isSamePatchType(const VertexHandle&, MeshType& a_Mesh, bool check_marked = false) { return false; };
    virtual bool isSamePatchType(const FaceHandle&, MeshType& a_Mesh, bool check_marked = false) { return false; };
    virtual PatchBuilder getPatchBuilder(const VertexHandle&, MeshType& a_Mesh, bool mark_gathered = false){throw std::runtime_error("getPatchBuilder for VertexHandle not implemented");};
    virtual PatchBuilder getPatchBuilder(const FaceHandle&, MeshType& a_Mesh, bool mark_gathered = false){throw std::runtime_error("getPatchBuilder for FaceHandle not implemented");};
    virtual std::string getGroupName() const = 0;
private:
    
};
