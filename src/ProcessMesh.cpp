/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "ProcessMesh.hpp"

void process_mesh(const MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise)
{

	a_Consumer->start();

	std::vector<PatchBuilder> t_PatchBuilders = getPatchBuilders(a_Mesh);

	// Face iteration
	for (auto patchBuilder = t_PatchBuilders.begin(); patchBuilder != t_PatchBuilders.end(); ++patchBuilder)
	{
		auto t_FacePatches = patchBuilder->buildPatches(a_Mesh);
		for (auto t_Patch : t_FacePatches)
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

std::vector<PatchBuilder> getPatchBuilders(const MeshType& a_Mesh)
{
	std::vector<PatchBuilder> t_PatchBuilders;
	// Construct the pool which will process the mesh
	PatchConstructorPool t_PatchConstructorPool;

	// Face iteration
	MeshType::FaceIter t_FaceIt, t_FaceEnd(a_Mesh.faces_end());
	for (auto t_FaceIt = a_Mesh.faces_begin(); t_FaceIt != t_FaceEnd; ++t_FaceIt)
	{
		PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_FaceIt, a_Mesh);
		if (t_Constructor == nullptr)
		{
			continue;
		}

		auto t_FacePatches = t_Constructor->getPatchBuilder(*t_FaceIt, a_Mesh);
		t_PatchBuilders.push_back(t_FacePatches);
	}

	// Vert iteration
	MeshType::VertexIter t_VertIt, t_VertEnd(a_Mesh.vertices_end());
	for (auto t_VertIt = a_Mesh.vertices_begin(); t_VertIt != t_VertEnd; ++t_VertIt)
	{
		PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_VertIt, a_Mesh);
		if (t_Constructor == nullptr)
		{
			continue;
		}

		auto t_VertPatches = t_Constructor->getPatchBuilder(*t_VertIt, a_Mesh);
		t_PatchBuilders.push_back(t_VertPatches);
		
	}

	return t_PatchBuilders;
}