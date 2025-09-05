/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "ProcessMesh.hpp"

void process_mesh(const MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise)
{

	a_Consumer->start();

	std::vector<PatchBuilder> t_PatchBuilders = getPatchBuilders(a_Mesh);

	// Face iteration
	for (auto patchBuilder = t_PatchBuilders.begin(); patchBuilder != t_PatchBuilders.end(); ++patchBuilder)
	{
		if(a_IsDegRaise)
		{
			patchBuilder->degRaise();
		}
		auto t_FacePatches = patchBuilder->buildPatches(a_Mesh);
		for (auto t_Patch : t_FacePatches)
		{

			a_Consumer->consume(t_Patch);
		}
	}

	a_Consumer->stop();

	return;
}

static void setupMarkedStatus(MeshType& a_Mesh)
{
	auto markedStatus = OpenMesh::VProp<bool>(a_Mesh, "marked_status");
	for (auto v : a_Mesh.vertices())
	{
		markedStatus[v] = false;
	}
}

std::vector<PatchBuilder> getPatchBuilders(const MeshType& a_Mesh)
{
	const int numSubdivisions = 2;
	
	MeshType subdividedMesh = a_Mesh;
	setupMarkedStatus(subdividedMesh);
	std::vector<PatchBuilder> t_PatchBuilders;
	// Construct the pool which will process the mesh
	PatchConstructorPool t_PatchConstructorPool;
	for(int s = 0; s <= numSubdivisions; ++s)
	{	
		if(s > 0)
		{
			std::cout << "Subdividing mesh at level: " << s << std::endl;
			subdividedMesh = subdividePnsControlMeshDooSabin(subdividedMesh);
		}
		auto markedStatus = OpenMesh::VProp<bool>(subdividedMesh, "marked_status");
		// Face iteration
		MeshType::FaceIter t_FaceIt, t_FaceEnd(subdividedMesh.faces_end());
		for (auto t_FaceIt = subdividedMesh.faces_begin(); t_FaceIt != t_FaceEnd; ++t_FaceIt)
		{
			//Check if any vert in this face is marked
			bool hasMarkedVert = false;
			for (auto v : t_FaceIt->vertices())
			{
				if (markedStatus[v])
				{
					hasMarkedVert = true;
					break;
				}
			}
			if(hasMarkedVert){
				// std::cout << "Skipping face with marked vertices: " << t_FaceIt->idx() << std::endl;
				continue;
			}
			PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_FaceIt, subdividedMesh);
			if (t_Constructor == nullptr)
			{
				continue;
			}
			// Mark all face verts
			for (auto v : t_FaceIt->vertices())
			{
				markedStatus[v] = true;
			}
			auto t_FacePatches = t_Constructor->getPatchBuilder(*t_FaceIt, subdividedMesh);
			t_PatchBuilders.push_back(t_FacePatches);
		}

		// Vert iteration
		MeshType::VertexIter t_VertIt, t_VertEnd(subdividedMesh.vertices_end());
		for (auto t_VertIt = subdividedMesh.vertices_begin(); t_VertIt != t_VertEnd; ++t_VertIt)
		{
			if (markedStatus[*t_VertIt])
			{
				// std::cout << "Skipping vertex with marked status: " << t_VertIt->idx() << std::endl;
				continue;
			}
			PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_VertIt, subdividedMesh);
			if (t_Constructor == nullptr)
			{
				continue;
			}
			// Mark the vertex as tagged
			markedStatus[*t_VertIt] = true;

			auto t_VertPatches = t_Constructor->getPatchBuilder(*t_VertIt, subdividedMesh);
			t_PatchBuilders.push_back(t_VertPatches);

		}
		std::cout << "Num patch builders: " << t_PatchBuilders.size() << std::endl;
	}

	return t_PatchBuilders;
}