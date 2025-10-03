/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "ProcessMesh.hpp"

void process_mesh(MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise)
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
	auto faceMarkedStatus = OpenMesh::FProp<bool>(a_Mesh, "marked_status");
	for (auto f : a_Mesh.faces())
	{
		faceMarkedStatus[f] = a_Mesh.is_boundary(f);
	}
}

std::vector<PatchBuilder> getPatchBuilders(MeshType& a_Mesh)
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
		// Face iteration
		MeshType::FaceIter t_FaceIt, t_FaceEnd(subdividedMesh.faces_end());
		for (auto t_FaceIt = subdividedMesh.faces_begin(); t_FaceIt != t_FaceEnd; ++t_FaceIt)
		{
			PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_FaceIt, subdividedMesh, true);
			if (t_Constructor == nullptr)
			{
				continue;
			}
			auto t_FacePatches = t_Constructor->getPatchBuilder(*t_FaceIt, subdividedMesh, true);
			t_PatchBuilders.push_back(t_FacePatches);
		}

		// Vert iteration
		MeshType::VertexIter t_VertIt, t_VertEnd(subdividedMesh.vertices_end());
		for (auto t_VertIt = subdividedMesh.vertices_begin(); t_VertIt != t_VertEnd; ++t_VertIt)
		{
			PatchConstructor* t_Constructor = t_PatchConstructorPool.getPatchConstructor(*t_VertIt, subdividedMesh, true);
			if (t_Constructor == nullptr)
			{
				continue;
			}

			auto t_VertPatches = t_Constructor->getPatchBuilder(*t_VertIt, subdividedMesh, true);
			t_PatchBuilders.push_back(t_VertPatches);

		}
		std::cout << "Num patch builders: " << t_PatchBuilders.size() << std::endl;
	}

	return t_PatchBuilders;
}

static MeshType copyMesh(MeshType &a_Mesh){
	// Setup vertex mapping if it doesn't exist
	if (!OpenMesh::hasProperty<OpenMesh::VertexHandle, VertexMapping>(a_Mesh, "vertex_mapping")) { 
        // create property if it doesn't exist
        auto t_vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
        for (auto v : a_Mesh.vertices()) {
            t_vertexMapping[v] = VertexMapping();
            t_vertexMapping[v].indices.push_back(v);
            t_vertexMapping[v].mapping.push_back(1.0);
        }
    }
	MeshType t_AugmentedMesh;
	
	// Copy original vertices and their mappings
	auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
	auto augmentedVertexMapping = OpenMesh::VProp<VertexMapping>(t_AugmentedMesh, "vertex_mapping");
	for(auto v : a_Mesh.vertices()){
		auto vh = t_AugmentedMesh.add_vertex(a_Mesh.point(v));
		augmentedVertexMapping[vh] = vertexMapping[v];
	}
	// Add all faces from original mesh
	for(auto f : a_Mesh.faces()){
		// Faces at boundary must be quads
		if(a_Mesh.valence(f) != 4 && a_Mesh.is_boundary(f)){
			return a_Mesh;
		}
		std::vector<MeshType::VertexHandle> faceVerts;
		for(auto fv_it = a_Mesh.fv_begin(f); fv_it != a_Mesh.fv_end(f); ++fv_it){
			faceVerts.push_back(t_AugmentedMesh.vertex_handle(fv_it->idx()));
		}
		t_AugmentedMesh.add_face(faceVerts);
	}
	return t_AugmentedMesh;
}

MeshType interpretGradientHandles(MeshType& a_Mesh){
	MeshType t_AugmentedMesh = copyMesh(a_Mesh);
	auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
	auto augmentedVertexMapping = OpenMesh::VProp<VertexMapping>(t_AugmentedMesh, "vertex_mapping");
	// Set the last layer vertices to correspoinding inner layer

    for (auto he_itr = a_Mesh.halfedges_begin(); he_itr != a_Mesh.halfedges_end(); ++he_itr)
    {
        if (a_Mesh.is_boundary(he_itr) == false)
            continue;

        // boundry found   
		auto boundarVh = a_Mesh.to_vertex_handle(*he_itr);          
		auto handleVh = a_Mesh.from_vertex_handle(a_Mesh.prev_halfedge_handle(a_Mesh.opposite_halfedge_handle(*he_itr)));


		if(a_Mesh.valence(boundarVh) == 3) { // Edge case
			augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundarVh.idx())] = vertexMapping[boundarVh] * 2 - vertexMapping[handleVh];
			// t_AugmentedMesh.set_point(t_AugmentedMesh.vertex_handle(boundarVh.idx()), a_Mesh.point(boundarVh) * 2 - a_Mesh.point(handleVh));
		}
		else if(a_Mesh.valence(boundarVh) == 2) { // Corner case
			auto adjVh = a_Mesh.from_vertex_handle(*he_itr);
			auto acrossVh = a_Mesh.to_vertex_handle(a_Mesh.next_halfedge_handle(a_Mesh.opposite_halfedge_handle(*he_itr)));
			augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundarVh.idx())] = vertexMapping[boundarVh] * 4 - vertexMapping[handleVh] * 2 - vertexMapping[adjVh] * 2 + vertexMapping[acrossVh];
			// t_AugmentedMesh.set_point(t_AugmentedMesh.vertex_handle(boundarVh.idx()), a_Mesh.point(boundarVh) * 4 - a_Mesh.point(handleVh) * 2 - a_Mesh.point(adjVh) * 2 + a_Mesh.point(acrossVh));
		}
		else 
		{
			return a_Mesh; // Not a valid PnS coontrol mesh with handles
		}

        
    }
	return t_AugmentedMesh;
}

MeshType setBoundaryGradient(MeshType& a_Mesh){
	MeshType t_AugmentedMesh = copyMesh(a_Mesh);
	auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
	auto augmentedVertexMapping = OpenMesh::VProp<VertexMapping>(t_AugmentedMesh, "vertex_mapping");
	// Set the last layer vertices to correspoinding inner layer

    for (auto he_itr = a_Mesh.halfedges_begin(); he_itr != a_Mesh.halfedges_end(); ++he_itr)
    {
        if (a_Mesh.is_boundary(he_itr) == false)
            continue;

        // boundry found   
		auto boundarVh = a_Mesh.to_vertex_handle(*he_itr);          
		auto innerVh = a_Mesh.from_vertex_handle(a_Mesh.prev_halfedge_handle(a_Mesh.opposite_halfedge_handle(*he_itr)));


		if(a_Mesh.valence(boundarVh) == 3) { // Edge case
			augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundarVh.idx())] = vertexMapping[innerVh];
			// t_AugmentedMesh.set_point(t_AugmentedMesh.vertex_handle(innerVh.idx()), a_Mesh.point(boundarVh));
		}
		else if(a_Mesh.valence(boundarVh) == 2) { // Corner case
			auto adjVh = a_Mesh.from_vertex_handle(*he_itr);
			auto acrossVh = a_Mesh.to_vertex_handle(a_Mesh.next_halfedge_handle(a_Mesh.opposite_halfedge_handle(*he_itr)));
			augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundarVh.idx())] = vertexMapping[acrossVh];
			// t_AugmentedMesh.set_point(t_AugmentedMesh.vertex_handle(acrossVh.idx()), a_Mesh.point(boundarVh));
		}
		else 
		{
			return a_Mesh; // Not a valid PnS coontrol mesh with handles
		}

        
    }
	return t_AugmentedMesh;
}

MeshType addDegenerateBoundaryLayer(MeshType& a_Mesh){
	MeshType t_AugmentedMesh = copyMesh(a_Mesh);
	auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
	auto augmentedVertexMapping = OpenMesh::VProp<VertexMapping>(t_AugmentedMesh, "vertex_mapping");
	// Add a degenerate layer at boundary for PnS3
	std::map<MeshType::VertexHandle, MeshType::VertexHandle> vertexCorrespondance;
	for(auto he_itr = t_AugmentedMesh.halfedges_begin(); he_itr != t_AugmentedMesh.halfedges_end(); ++he_itr){
		if(t_AugmentedMesh.is_boundary(he_itr) == false)
			continue;
		auto boundaryVh = t_AugmentedMesh.to_vertex_handle(*he_itr);
		if(t_AugmentedMesh.valence(boundaryVh) == 3) { // Edge case
			auto newVh = t_AugmentedMesh.add_vertex(t_AugmentedMesh.point(t_AugmentedMesh.vertex_handle(boundaryVh.idx())));
			augmentedVertexMapping[newVh] = augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundaryVh.idx())];
			vertexCorrespondance[t_AugmentedMesh.vertex_handle(boundaryVh.idx())] = newVh;
		}else if(t_AugmentedMesh.valence(boundaryVh) == 2) { // Corner case(add 3 new vertices)
			auto newVh1 = t_AugmentedMesh.add_vertex(t_AugmentedMesh.point(t_AugmentedMesh.vertex_handle(boundaryVh.idx())));
			auto newVh2 = t_AugmentedMesh.add_vertex(t_AugmentedMesh.point(t_AugmentedMesh.vertex_handle(boundaryVh.idx())));
			auto newVh3 = t_AugmentedMesh.add_vertex(t_AugmentedMesh.point(t_AugmentedMesh.vertex_handle(boundaryVh.idx())));
			augmentedVertexMapping[newVh1] = augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundaryVh.idx())];
			augmentedVertexMapping[newVh2] = augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundaryVh.idx())];
			augmentedVertexMapping[newVh3] = augmentedVertexMapping[t_AugmentedMesh.vertex_handle(boundaryVh.idx())];
			vertexCorrespondance[t_AugmentedMesh.vertex_handle(boundaryVh.idx())] = newVh3;
		}else{
			return a_Mesh; // Not a valid PnS coontrol mesh with handles
		}
		
	}
	// Add faces for the degenerate layer
	for(auto he_itr = a_Mesh.halfedges_begin(); he_itr != a_Mesh.halfedges_end(); ++he_itr){
		if(a_Mesh.is_boundary(he_itr) == false)
			continue;
		auto currVh = a_Mesh.to_vertex_handle(*he_itr);
		auto prevVh = a_Mesh.from_vertex_handle(*he_itr);
		if(a_Mesh.valence(currVh) == 3){
			auto currAddedVh = vertexCorrespondance[t_AugmentedMesh.vertex_handle(currVh.idx())];
			auto prevAddedVh = vertexCorrespondance[t_AugmentedMesh.vertex_handle(prevVh.idx())];
			std::vector<MeshType::VertexHandle> faceVerts = {prevVh, currVh, currAddedVh, prevAddedVh};
			t_AugmentedMesh.add_face(faceVerts);
		}else if(a_Mesh.valence(currVh) == 2){
			auto currAddedVh = vertexCorrespondance[t_AugmentedMesh.vertex_handle(currVh.idx())];
			auto cornerAddVh = t_AugmentedMesh.vertex_handle(currAddedVh.idx() - 1);
			auto otherEdgeVh = t_AugmentedMesh.vertex_handle(currAddedVh.idx() - 2);
			std::vector<MeshType::VertexHandle> faceVerts1 = {currVh, currAddedVh, cornerAddVh, otherEdgeVh};
			t_AugmentedMesh.add_face(faceVerts1);
			auto prevVh = a_Mesh.from_vertex_handle(*he_itr);
			auto prevAddedVh = vertexCorrespondance[t_AugmentedMesh.vertex_handle(prevVh.idx())];
			std::vector<MeshType::VertexHandle> faceVerts2 = {currVh, otherEdgeVh, prevAddedVh, prevVh};
			t_AugmentedMesh.add_face(faceVerts2);
		}else{
			return a_Mesh; // Not a valid PnS coontrol mesh with handles
		}
	}
	return t_AugmentedMesh;
}