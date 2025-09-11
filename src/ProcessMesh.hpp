/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "Helper/Helper.hpp"
#include "Subdivision/subdivision.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

std::vector<PatchBuilder> getPatchBuilders(MeshType& a_Mesh);

void process_mesh(MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise);

std::vector<PatchBuilder> getPatchBuilders(MeshType& a_Mesh);