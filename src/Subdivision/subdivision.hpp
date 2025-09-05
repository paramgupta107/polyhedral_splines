#pragma once
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <map>
#include <numeric>
#include "VertexMapping.hpp"


typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

MeshType subdividePnsControlMeshCatmullClark(MeshType& a_Mesh);

MeshType subdividePnsControlMeshDooSabin(MeshType& a_Mesh);