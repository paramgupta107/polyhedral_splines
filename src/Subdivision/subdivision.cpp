#include "subdivision.hpp"

MeshType subdividePnsControlMeshCatmullClark(MeshType& a_Mesh){
    MeshType t_SubdividedMesh;

    // setup vertex mapping
    auto subdividedVertexMapping = OpenMesh::VProp<VertexMapping>(t_SubdividedMesh, "vertex_mapping");
    auto markedStatus = OpenMesh::FProp<bool>(a_Mesh, "marked_status");
    auto subdividedMarkedStatus = OpenMesh::FProp<bool>(t_SubdividedMesh, "marked_status");
    if (!OpenMesh::hasProperty<OpenMesh::VertexHandle, VertexMapping>(a_Mesh, "vertex_mapping")) { 
        // create property if it doesn't exist
        auto t_vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
        for (auto v : a_Mesh.vertices()) {
            t_vertexMapping[v] = VertexMapping();
            t_vertexMapping[v].indices.push_back(v);
            t_vertexMapping[v].mapping.push_back(1.0);
        }
    }
    auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");

    // Cutmull Clark subdivision

    // Face points
    std::map<MeshType::FaceHandle, MeshType::VertexHandle> facePoints;
    for (auto f : a_Mesh.faces()) {
        MeshType::Point facePoint = std::accumulate(
            a_Mesh.fv_begin(f), a_Mesh.fv_end(f), MeshType::Point(0, 0, 0),
            [&a_Mesh](const MeshType::Point& sum, const MeshType::VertexHandle& v) {
                return sum + a_Mesh.point(v);
            }) / static_cast<double>(a_Mesh.valence(f));
        auto facePointHandle = t_SubdividedMesh.add_vertex(facePoint);
        subdividedVertexMapping[facePointHandle] = std::accumulate(
            a_Mesh.fv_begin(f), a_Mesh.fv_end(f), VertexMapping(),
            [&vertexMapping](const VertexMapping& sum, const MeshType::VertexHandle& v) {
                return sum + vertexMapping[v];
            }) / static_cast<double>(a_Mesh.valence(f));
        facePoints[f] = facePointHandle;
    }

    // Edge points
    std::map<MeshType::EdgeHandle, MeshType::VertexHandle> edgePoints;
    for (auto e : a_Mesh.edges()) {
        auto halfEdge = a_Mesh.halfedge_handle(e);
        VertexMapping vm = vertexMapping[a_Mesh.to_vertex_handle(halfEdge)] + vertexMapping[a_Mesh.from_vertex_handle(halfEdge)];
        MeshType::Point edgePoint = (a_Mesh.point(a_Mesh.to_vertex_handle(halfEdge)) + a_Mesh.point(a_Mesh.from_vertex_handle(halfEdge)));
        if (a_Mesh.is_boundary(e)) {
            edgePoint /= 2.0; // For boundary edges, just average the two vertices
            vm = vm / 2.0; // Average the vertex mappings
        } else {
            edgePoint += t_SubdividedMesh.point(facePoints[a_Mesh.face_handle(halfEdge)]) + 
                         t_SubdividedMesh.point(facePoints[a_Mesh.opposite_face_handle(halfEdge)]);
            edgePoint /= 4.0;
            vm = (vm + subdividedVertexMapping[facePoints[a_Mesh.face_handle(halfEdge)]]
                + subdividedVertexMapping[facePoints[a_Mesh.opposite_face_handle(halfEdge)]]) / 4.0;
        }
        auto edgePointHandle = t_SubdividedMesh.add_vertex(edgePoint);
        subdividedVertexMapping[edgePointHandle] = vm;
        edgePoints[e] = edgePointHandle;
    }

    // Vertex points
    std::map<MeshType::VertexHandle, MeshType::VertexHandle> vertexPoints;
    for (auto v : a_Mesh.vertices()) {
        int n = a_Mesh.valence(v);
        bool isHole = a_Mesh.is_boundary(v);
        if (!isHole){
            MeshType::Point F(0, 0, 0); // Average of face points
            VertexMapping Fm; // Average of face point mappings
            for (auto vf_it = a_Mesh.vf_begin(v); vf_it != a_Mesh.vf_end(v); ++vf_it) {
                F += t_SubdividedMesh.point(facePoints[*vf_it]);
                Fm = Fm + subdividedVertexMapping[facePoints[*vf_it]];
            }
            F /= static_cast<double>(n);
            Fm = Fm / static_cast<double>(n);

            MeshType::Point R(0, 0, 0); // Average of midpoints of edges
            VertexMapping Rm; // Average of edge midpoints mappings
            for (auto voh_it = a_Mesh.voh_begin(v); voh_it != a_Mesh.voh_end(v); ++voh_it) {
                R += a_Mesh.point(a_Mesh.to_vertex_handle(*voh_it));
                Rm = Rm + vertexMapping[a_Mesh.to_vertex_handle(*voh_it)];
            }
            R /= static_cast<double>(n);
            Rm = Rm / static_cast<double>(n);

            MeshType::Point vertexPoint = (F + R + (n - 2.0) * a_Mesh.point(v)) / static_cast<double>(n);
            VertexMapping vm = (Fm + Rm + vertexMapping[v] * (n - 2.0)) / static_cast<double>(n);
            auto vertexPointHandle = t_SubdividedMesh.add_vertex(vertexPoint);
            subdividedVertexMapping[vertexPointHandle] = vm;
            vertexPoints[v] = vertexPointHandle;
        } else {
            int numHoleEdges = 1;
            MeshType::Point holePoint = a_Mesh.point(v);
            VertexMapping holeMapping = vertexMapping[v];
            for (auto ve_it = a_Mesh.ve_begin(v); ve_it != a_Mesh.ve_end(v); ++ve_it) {
                if (a_Mesh.is_boundary(*ve_it)) {
                    holePoint += t_SubdividedMesh.point(edgePoints[*ve_it]);
                    holeMapping = holeMapping + subdividedVertexMapping[edgePoints[*ve_it]];
                    ++numHoleEdges;
                }
            }
            holePoint /= static_cast<double>(numHoleEdges);
            holeMapping = holeMapping / static_cast<double>(numHoleEdges);
            auto vertexPointHandle = t_SubdividedMesh.add_vertex(holePoint);
            subdividedVertexMapping[vertexPointHandle] = holeMapping;
            vertexPoints[v] = vertexPointHandle;
        }
        
    }
    
    // Create faces in the new mesh
    for (auto f : a_Mesh.faces()) {
        bool currMarkedStatus = markedStatus[f];
        std::vector<MeshType::VertexHandle> faceVertices;
        for (auto fv_it = a_Mesh.fv_begin(f); fv_it != a_Mesh.fv_end(f); ++fv_it) {
            faceVertices.push_back(*fv_it);
        }
        for(int i = 0; i < faceVertices.size(); ++i) {
            std::vector<MeshType::VertexHandle> newFace;
            newFace.push_back(vertexPoints[faceVertices[i]]);
            newFace.push_back(edgePoints[a_Mesh.edge_handle(a_Mesh.find_halfedge(faceVertices[i], faceVertices[(i + 1) % faceVertices.size()]))]);
            newFace.push_back(facePoints[f]);
            newFace.push_back(edgePoints[a_Mesh.edge_handle(a_Mesh.find_halfedge(faceVertices[(i - 1 + faceVertices.size()) % faceVertices.size()], faceVertices[i]))]);
            auto fh = t_SubdividedMesh.add_face(newFace);
            if(currMarkedStatus) {
                subdividedMarkedStatus[fh] = true;
            }
        }
    }
    return t_SubdividedMesh;


}


MeshType subdividePnsControlMeshDooSabin(MeshType& a_Mesh)
{
    MeshType t_SubdividedMesh;

    auto subdividedVertexMapping = OpenMesh::VProp<VertexMapping>(t_SubdividedMesh, "vertex_mapping");
    auto markedStatus = OpenMesh::VProp<bool>(a_Mesh, "marked_status");
    auto subdividedMarkedStatus = OpenMesh::VProp<bool>(t_SubdividedMesh, "marked_status");

    if (!OpenMesh::hasProperty<OpenMesh::VertexHandle, VertexMapping>(a_Mesh, "vertex_mapping")) {
        auto t_vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
        for (auto v : a_Mesh.vertices()) {
            t_vertexMapping[v] = VertexMapping();
            t_vertexMapping[v].indices.push_back(v);
            t_vertexMapping[v].mapping.push_back(1.0);
        }
    }
    auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");

    // corner vertices keyed by halfedge (corner = from_vertex(h) in face f(h))
    std::map<MeshType::HalfedgeHandle, MeshType::VertexHandle> cornerVertices;

    for (auto f : a_Mesh.faces()) {
        const int n = a_Mesh.valence(f);

        // centroid point & mapping
        MeshType::Point Fp = std::accumulate(
            a_Mesh.fv_begin(f), a_Mesh.fv_end(f), MeshType::Point(0,0,0),
            [&a_Mesh](const MeshType::Point& s, const MeshType::VertexHandle& v){ return s + a_Mesh.point(v); }
        ) / static_cast<double>(n);

        VertexMapping Fm = std::accumulate(
            a_Mesh.fv_begin(f), a_Mesh.fv_end(f), VertexMapping(),
            [&vertexMapping](const VertexMapping& s, const MeshType::VertexHandle& v){ return s + vertexMapping[v]; }
        ) / static_cast<double>(n);


        std::vector<MeshType::VertexHandle> facePoly;
        for (auto fh_it = a_Mesh.fh_begin(f); fh_it != a_Mesh.fh_end(f); ++fh_it) {
            auto h = *fh_it;
            auto v = a_Mesh.from_vertex_handle(h);
            auto v_next = a_Mesh.to_vertex_handle(h);
            auto h_prev = a_Mesh.prev_halfedge_handle(h);
            auto v_prev = a_Mesh.from_vertex_handle(h_prev);

            // edge midpoints adjacent to v inside this face
            MeshType::Point E_prev_p = (a_Mesh.point(v_prev) + a_Mesh.point(v)) * 0.5;
            MeshType::Point E_next_p = (a_Mesh.point(v) + a_Mesh.point(v_next)) * 0.5;

            // corner position: (V + E_prev + E_next + F)/4
            MeshType::Point cornerP = (a_Mesh.point(v) + E_prev_p + E_next_p + Fp) / 4.0;

            // mapping mirrors the same linear combo
            VertexMapping vm =
                ( vertexMapping[v]
                + (vertexMapping[v_prev] + vertexMapping[v]) * 0.5
                + (vertexMapping[v]     + vertexMapping[v_next]) * 0.5
                + Fm ) / 4.0;

            auto vh = t_SubdividedMesh.add_vertex(cornerP);
            subdividedVertexMapping[vh] = vm;

            cornerVertices[h] = vh;
            facePoly.push_back(vh);
        }

        // face-face polygon
        t_SubdividedMesh.add_face(facePoly);
    }

    // Edge-faces: quads around interior edges
    for (auto e : a_Mesh.edges()) {
        if (a_Mesh.is_boundary(e)) continue;

        auto h0 = a_Mesh.halfedge_handle(e, 0);
        auto h1 = a_Mesh.halfedge_handle(e, 1);

        std::vector<MeshType::VertexHandle> quad;
        quad.push_back(cornerVertices[h0]);
        quad.push_back(cornerVertices[a_Mesh.next_halfedge_handle(h1)]);
        quad.push_back(cornerVertices[h1]);
        quad.push_back(cornerVertices[a_Mesh.next_halfedge_handle(h0)]);
        

        t_SubdividedMesh.add_face(quad);
    }

    // Vertex-faces: one polygon per original vertex (corners around that vertex)
    for (auto v : a_Mesh.vertices()) {
        std::vector<MeshType::VertexHandle> ring;
        for (auto voh_it = a_Mesh.voh_ccwbegin(v); voh_it != a_Mesh.voh_ccwend(v); ++voh_it) {
            auto h = *voh_it;
            auto f = a_Mesh.face_handle(h);
            if (!f.is_valid()) continue; // skip gaps at boundary
            ring.push_back(cornerVertices[h]);
            if(markedStatus[v]) {
                subdividedMarkedStatus[cornerVertices[h]] = true;
            }
        }
        if (ring.size() >= 3)
            t_SubdividedMesh.add_face(ring);
    }

    return t_SubdividedMesh;
}