/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "Helper/Helper.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

void process_mesh(const MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise)
{
	// Construct the pool which will process the mesh
	PatchConstructorPool t_PatchConstructorPool(a_Mesh);

	a_Consumer->start();

	// Face iteration
	MeshType::FaceIter t_FaceIt, t_FaceEnd(a_Mesh.faces_end());
	for (auto t_FaceIt = a_Mesh.faces_begin(); t_FaceIt != t_FaceEnd; ++t_FaceIt)
	{
		auto* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_FaceIt);
		if (t_Constructor == nullptr)
		{
			continue;
		}

		auto t_FacePatches = t_Constructor->getPatch(*t_FaceIt);
		for (auto t_Patch : t_FacePatches)
		{
			if(a_IsDegRaise)
			{
				t_Patch.degRaise();
			}

			a_Consumer->consume(t_Patch);
		}
	}

	// Vert iteration
	MeshType::VertexIter t_VertIt, t_VertEnd(a_Mesh.vertices_end());
	for (auto t_VertIt = a_Mesh.vertices_begin(); t_VertIt != t_VertEnd; ++t_VertIt)
	{
		auto* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_VertIt);
		if (t_Constructor == nullptr)
		{
			continue;
		}

		auto t_VertPatches = t_Constructor->getPatch(*t_VertIt);
		for (auto t_Patch : t_VertPatches)
		{
			if(a_IsDegRaise)
			{
				t_Patch.degRaise();
			}

			a_Consumer->consume(t_Patch);
		}
	}

	a_Consumer->stop();

	return;
}
