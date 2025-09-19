/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "Helper.hpp"
#include "HalfedgeOperation.hpp"
#include <iostream>
#include <cmath>
#include <unordered_set>

/** 
 * \defgroup helper Helper Functions
 * \ingroup utility
 * @brief General helper functions for mesh processing.
 */
namespace Helper
{

// Vert functions

/**
 * \ingroup helper
 * @brief Get the vert valence object
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle
 * @return the valence of the vertex 
 */
int get_vert_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return a_Mesh.valence(a_VertexHandle);
}

/**
 * \ingroup helper
 * @brief Checks if the valence of the given vertex is 3
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle
 * @return true if the valence is 3
 */
bool is_vert_3_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==3) ? true : false;
}

/**
 * \ingroup helper
 * @brief Checks if the valence of the given vertex is 4
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle
 * @return true if the valence is 4
 */
bool is_vert_4_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==4) ? true : false;
}

/**
 * \ingroup helper
 * @brief Checks if the valence of the given vertex is 5
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle
 * @return true if the valence is 5
 */
bool is_vert_5_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==5) ? true : false;
}

/**
 * \ingroup helper
 * @brief Checks if all the vertices of the given face are 4-valence
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if all the vertices of the face are 4-valence
 */
bool are_verts_of_face_all_4_valence(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    for(auto FVIt = a_Mesh.cfv_iter(a_FaceHandle); FVIt.is_valid(); FVIt++)
    {
        if(!is_vert_4_valence(a_Mesh, *FVIt))
        {
            return false;
        }
    }
    return true;
}

/**
 * \ingroup helper
 * @brief Check if the given vertex is one of the vertices in face
 * @param a_Mesh The mesh containing the face and vertex
 * @param a_FaceHandle The face handle
 * @param a_VertHandle The vertex handle
 * @return true if the vertex is in the face, false otherwise
*/
bool is_vert_in_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle, const VertexHandle& a_VertHandle)
{
    for(auto FVIt=a_Mesh.cfv_iter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        if(*FVIt==a_VertHandle)
        {
            return true;
        }
    }
    return false;
}

/**
 * \ingroup helper
 * @brief Checks if the given vertex could be a polar surrounding vertex. A polar patch must be surrounded by quads. 
 * Check if the surrounding faces are all quads except the two triangles that belong to the polar point.
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle of the potential polar surrounding vertex
 * @param only_regular If true, only 4-valence vertex will be considered
 * @param max_valence The maximum valence of the vertex. The minimum valence is 3.
 * @return true 
 * @return false 
 */
bool is_polar_surrounding_vert(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle, bool only_regular, int max_valence){
    int t_Valence = get_vert_valence(a_Mesh, a_VertexHandle);
    auto t_FHs = get_faces_around_vert_counterclock(a_Mesh, a_VertexHandle);
    if(t_Valence != t_FHs.size()){
        return false;
    }
    if(only_regular && t_Valence != 4){
        return false;
    }
    if(t_Valence < 3 || t_Valence > max_valence){
        return false;
    }
    int t_Tri_Count = 0;
    for(int i=0; i<t_FHs.size(); i++)
    {
        if(!is_triangle(a_Mesh, t_FHs[i]) && !is_quad(a_Mesh, t_FHs[i]))
        {
            return false;
        }

        if(is_triangle(a_Mesh, t_FHs[i]))
        {
            t_Tri_Count++;
            if(!is_triangle(a_Mesh, t_FHs[(i-1)%t_FHs.size()]) &&
                !is_triangle(a_Mesh, t_FHs[(i+1)%t_FHs.size()])){
                return false;
                }
        }
    }
    if (t_Tri_Count != 2)
    {
        return false;
    }
    return true;
}

/**
 * \ingroup helper
 * @brief Checks if the given vertex could be a polar point. 
 * A polar point is defined as a vertex that is surrounded by triangles. The neighborhood around the triangles should be all quads.
 * Also checks if the surrounding vertices are also valid polar surrounding vertices.
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertexHandle The vertex handle of the potential polar point
 * @param only_regular If true, only 4-valence surrounding polar vertex will be considered
 * @param max_valence The maximum valence of the polar vertex. The minimum valence is 3.
 * @param surrounding_max_valence The maximum valence of the surrounding vertex.
 * @return true 
 * @return false 
 */
bool is_polar(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle, bool only_regular, int max_valence, int surrounding_max_valence){
    int t_Valence = get_vert_valence(a_Mesh, a_VertexHandle);
    const int t_Lowerbound = 3;
    const int t_Upperbound = max_valence;
    if(t_Valence < t_Lowerbound || t_Valence > t_Upperbound)
    {
        return false;
    }

    // Polar point should not be on the boundary
    if(a_Mesh.is_boundary(a_VertexHandle))
    {
        return false;
    }

    // The first layer of surrounded faces should all be triangles
    auto t_NBFaces = get_faces_around_vert_counterclock(a_Mesh, a_VertexHandle);
    if(t_NBFaces.size() != t_Valence)
    {
        return false;
    }
    for(auto t_Face : t_NBFaces)
    {
        if(!is_triangle(a_Mesh, t_Face))
        {
            return false;
        }
    }

    auto surrounding_verts = get_surrounding_verts(a_Mesh, a_VertexHandle);
    for(auto vert : surrounding_verts){
        if(!is_polar_surrounding_vert(a_Mesh, vert, only_regular, max_valence)){
            return false;
        }
    }
    return true;
    
}

/**
 * \ingroup helper
 * @brief Given a vertex that is potintially a vertex surrounding a polar point, find the polar point vertex.
 * 
 * @param mesh The mesh containing the vertex
 * @param outerVH The vertex handle of the potential polar surrounding vertex
 * @return VertexHandle The vertex handle of the polar point. Invalid handle if not found.
 */
VertexHandle find_polar_vertex(const MeshType& mesh, VertexHandle outerVH) {
    for (auto heh : mesh.voh_range(outerVH)) {
        FaceHandle f1 = mesh.face_handle(heh);
        FaceHandle f2 = mesh.face_handle(mesh.opposite_halfedge_handle(heh));

        if (f1.is_valid() && f2.is_valid() &&
            is_triangle(mesh, f1) && is_triangle(mesh, f2)) 
        {
            // the vertex at the far end of this halfedge is the polar
            return mesh.to_vertex_handle(heh);
        }
    }
    return VertexHandle(); // invalid handle if not found
}

/**
 * \ingroup helper
 * @brief Set the vert vector to default object
 * 
 * @param a_Size The size of the vector to be initialized to.
 * @param a_VertexHandles The vertex handle vector to be initialized.
 */
void set_vert_vector_to_default(const int a_Size, std::vector<VertexHandle>& a_VertexHandles)
{
    a_VertexHandles.clear();
    VertexHandle t_DefaultVertexHandle;
    for(int i=0; i<a_Size; i++)
    {
        a_VertexHandles.push_back(t_DefaultVertexHandle);
    }
}

/**
 * \ingroup helper
 * @brief Get the faces around vert counterclock object
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return all the facehandles around the given vertexhandle
 */
std::vector<FaceHandle> get_faces_around_vert_counterclock(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<FaceHandle> t_Faces;
    for(auto t_VFIt = a_Mesh.cvf_ccwiter(a_VertHandle); t_VFIt.is_valid(); ++t_VFIt)
    {
        t_Faces.push_back(*t_VFIt);
    }
    return t_Faces;
}

/**
 * \ingroup helper
 * @brief Get first and second layers of faces aroumnd the vert (unordered)
 *  ex:
 *  \code
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - v - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 * \endcode
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return All the facehandles in the two layers around the given vertexhandle
 * 
 */
std::vector<FaceHandle> get_two_layers_faces_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<FaceHandle> t_TwoLayerFaces;

    // Get first layer of faces around vertex
    // And get the neighbor faces of each first layer face (neighbors might be duplicated)
    for(auto VF_It = a_Mesh.cvf_iter(a_VertHandle); VF_It.is_valid(); ++VF_It)
    {
        auto t_FaceNBFaces = init_neighbor_faces(a_Mesh, *VF_It);
        t_TwoLayerFaces.insert(t_TwoLayerFaces.end(), t_FaceNBFaces.begin(), t_FaceNBFaces.end());
    }

    // Eliminate the duplicated faces
    sort(t_TwoLayerFaces.begin(), t_TwoLayerFaces.end());
    t_TwoLayerFaces.erase(unique(t_TwoLayerFaces.begin(), t_TwoLayerFaces.end()), t_TwoLayerFaces.end());

    return t_TwoLayerFaces;
}

/**
 * \ingroup helper
 * @brief Get first and second layers of verts around the given vert (unordered). The layout does not need to be regular.
 *
 * ex:
 * \code
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - v - o - o  v: Given point(included)
 *      |   |   |   |   |  o: Verts to get
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 * \endcode
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return All the vertexhandles in the two layers around the given vertexhandle
 */
std::vector<VertexHandle> get_two_layers_verts_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<VertexHandle> t_AllVerts;
    auto t_TwoLayersFaces = get_two_layers_faces_around_vert(a_Mesh, a_VertHandle);

    //Get vertices from each face
    for(auto t_Face : t_TwoLayersFaces)
    {
        auto t_VertsForAFace = get_verts_of_face(a_Mesh, t_Face);
        t_AllVerts.insert(t_AllVerts.end(), t_VertsForAFace.begin(), t_VertsForAFace.end());
    }

    // Eliminate the duplicated verts
    sort(t_AllVerts.begin(), t_AllVerts.end());
    t_AllVerts.erase(unique(t_AllVerts.begin(), t_AllVerts.end()), t_AllVerts.end());

    return t_AllVerts;
}

/**
 * \ingroup helper
 * @brief Get first layer of verts around the given vert. The layout does not need to be regular.
 *
 * ex:
 * \code
 *    1 - 0 - 7   x: Given point(not included)
 *    |   |   |   0-7: Verts to get
 *    2 - x - 6
 *    |   |   |
 *    3 - 4 - 5
 * \endcode
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return All the vertexhandles in the first layer around the given vertexhandle
 */
std::vector<VertexHandle> get_first_layers_verts_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<VertexHandle> t_Verts;
    std::vector<int> t_Commands = {1, 4, 1, 4}; //Command for getting one section (ex: 0, 1)
    for(auto VHIt=a_Mesh.cvoh_ccwiter(a_VertHandle); VHIt.is_valid(); ++VHIt)
    {
        auto t_SectionVerts = HalfedgeOperation::get_verts_fixed_halfedge(a_Mesh, *VHIt, t_Commands);
        t_Verts.insert(t_Verts.begin(), t_SectionVerts.begin(), t_SectionVerts.end());
    }
    return t_Verts;
}

/**
 * \ingroup helper
 * @brief Get surrounding verts around the given vert (unordered)
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return All the vertexhandles surrounding the given vertexhandle
 */
std::vector<VertexHandle> get_surrounding_verts(const MeshType& a_Mesh, const VertexHandle& a_VertHandle){
    std::vector<VertexHandle> t_surrounding_verts;
    for(auto vv_it = a_Mesh.cvv_iter(a_VertHandle); vv_it.is_valid(); ++vv_it)
    {
        t_surrounding_verts.push_back(*vv_it);
    }
    return t_surrounding_verts;
}

/**
 * \ingroup helper
 * @brief Get second layer of faces around the given vert (unordered). The layout does not need to be regular.
 * 
 * ex:
 * \code
 *      o - o - o - o - o
 *      | x | x | x | x |
 *      o - o - o - o - o
 *      | x |   |   | x |
 *      o - o - v - o - o  v: Given point
 *      | x |   |   | x |  x: Faces to get
 *      o - o - o - o - o
 *      | x | x | x | x |
 *      o - o - o - o - o
 * \endcode
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return All the facehandles in the second layer around the given vertexhandle
 */
std::vector<FaceHandle> get_second_layer_faces_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    auto t_TwoLayerFaces = get_two_layers_faces_around_vert(a_Mesh, a_VertHandle);
    auto t_FirstLayerFaces = get_faces_around_vert_counterclock(a_Mesh, a_VertHandle);

    auto pred = [&t_FirstLayerFaces](const FaceHandle& key) ->bool
    {
        return std::find(t_FirstLayerFaces.begin(), t_FirstLayerFaces.end(), key) != t_FirstLayerFaces.end();
    };

    t_TwoLayerFaces.erase(std::remove_if(t_TwoLayerFaces.begin(), t_TwoLayerFaces.end(), pred), t_TwoLayerFaces.end());

    return t_TwoLayerFaces;
}

/**
 * \ingroup helper
 * @brief Check if the given vertex is marked. A marked vertex is determined by the bool named property "marked_status" on a vertex.
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return true if the vertex is marked
 */
bool is_marked(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    OpenMesh::VPropHandleT<bool> t_marked;
    a_Mesh.get_property_handle(t_marked, "marked_status");
    return a_Mesh.property(t_marked, a_VertHandle);
}

/**
 * \ingroup helper
 * @brief Mark the given vertex. A marked vertex is determined by the bool named property "marked_status" on a vertex.
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 */
void mark_vert(MeshType& a_Mesh, const VertexHandle& a_VertHandle){
    OpenMesh::VPropHandleT<bool> t_marked;
    a_Mesh.get_property_handle(t_marked, "marked_status");
    a_Mesh.property(t_marked, a_VertHandle) = true;
}


// Face functions

/**
 * \ingroup helper
 * @brief Check if the given face is a triangle
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if the face is a triangle
 */
bool is_triangle(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForQuad = 3;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForQuad) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if the given face is a quad
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if the face is a quad
 */
bool is_quad(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForQuad = 4;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForQuad) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if the given face is a pentagon
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if the face is a pentagon
 */
bool is_pentagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForPentagon = 5;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForPentagon) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if the given face is a hexagon
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if the face is a hexagon
 */
bool is_hexagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForHexagon = 6;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForHexagon) ? true : false;
}

/**
 * \ingroup helper
 * @brief Initialize the neighbor faces of the given face.
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return The vector of neighbor face handles
 */
std::vector<FaceHandle> init_neighbor_faces(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    std::vector<FaceHandle> t_NBFaceHandles;

    for (auto t_FVIt = a_Mesh.cfv_iter(a_FaceHandle); t_FVIt.is_valid(); ++t_FVIt)
    {
        for (auto t_VFIt = a_Mesh.cvf_iter(*t_FVIt); t_VFIt.is_valid(); ++t_VFIt)
        {
            t_NBFaceHandles.push_back(*t_VFIt);
        }
    }
    // Remove center face (Original)
    t_NBFaceHandles.erase(std::remove(t_NBFaceHandles.begin(),
                          t_NBFaceHandles.end(), a_FaceHandle), t_NBFaceHandles.end());

    // Remove duplicated neighbor faces
    sort(t_NBFaceHandles.begin(), t_NBFaceHandles.end());
         t_NBFaceHandles.erase(unique(t_NBFaceHandles.begin(),
                               t_NBFaceHandles.end()), t_NBFaceHandles.end());

    return t_NBFaceHandles;
}

/**
 * \ingroup helper
 * @brief Check if the number of neighbor faces is 7
 * 
 * @param a_NBFaceHandles The vector of neighbor face handles
 * @return true if the number of neighbor faces is 7
 */
bool has_7_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    const int t_7Neighbor = 7;
    return (get_num_of_neighbor_faces(a_NBFaceHandles)==t_7Neighbor) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if the number of neighbor faces is 8
 * 
 * @param a_NBFaceHandles The vector of neighbor face handles
 * @return true if the number of neighbor faces is 8
 */
bool has_8_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    const int t_8Neighbor = 8;
    return (get_num_of_neighbor_faces(a_NBFaceHandles)==t_8Neighbor) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if the number of neighbor faces is 9
 * 
 * @param a_NBFaceHandles The vector of neighbor face handles
 * @return true if the number of neighbor faces is 9
 */
bool has_9_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    const int t_9Neighbor = 9;
    return (get_num_of_neighbor_faces(a_NBFaceHandles)==t_9Neighbor) ? true : false;
}

/**
 * \ingroup helper
 * @brief Check if all the given faces are quads
 * 
 * @param a_Mesh The mesh containing the faces
 * @param a_FaceHandles The vector of face handles
 * @return true if all the faces are quads
 */
bool are_faces_all_quads(const MeshType& a_Mesh, const std::vector<FaceHandle>& a_FaceHandles)
{
    for(auto t_FH : a_FaceHandles)
    {
        if(!is_quad(a_Mesh, t_FH))
        {
            return false;
        }
    }
    return true;
}

/**
 * \ingroup helper
 * @brief Check if the given vertex is only surrounded by quads
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return true if the vertex is only surrounded by quads
 */
bool is_only_surrounded_by_quad(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    for(auto VFIt = a_Mesh.cvf_iter(a_VertHandle); VFIt.is_valid(); ++VFIt)
    {
        if(!is_quad(a_Mesh, *VFIt))
        {
            return false;
        }
    }
    return true;
}

/**
 * \ingroup helper
 * @brief Get the number of vertices for the given face
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return the number of vertices for the face
 */
int get_num_of_verts_for_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    return a_Mesh.valence(a_FaceHandle);
}

/**
 * \ingroup helper
 * @brief Get the number of neighbor faces for the given neighborhood
 * 
 * @param a_NBFaceHandles The vector of neighbor face handles
 * @return the number of neighbor faces
 */
int get_num_of_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    return a_NBFaceHandles.size();
}

/**
 * \ingroup helper
 * @brief Get the vertices of the given face
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return The vector of vertex handles of the face
 */
std::vector<VertexHandle> get_verts_of_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    std::vector<VertexHandle> t_VertsOfFace;
    for (auto t_FVIt = a_Mesh.cfv_ccwiter(a_FaceHandle); t_FVIt.is_valid(); ++t_FVIt)
    {
        t_VertsOfFace.push_back(*t_FVIt);
    }
    return t_VertsOfFace;
}

/**
 * \ingroup helper
 * @brief Get the vertices of the given faces
 * 
 * @param a_Mesh The mesh containing the faces
 * @param a_FaceHandles The vector of face handles
 * @return The vector of vertex handles of the faces (no duplicate)
 */
std::vector<VertexHandle> get_verts_of_faces(const MeshType& a_Mesh, const std::vector<FaceHandle>& a_FaceHandles)
{
    std::vector<VertexHandle> t_VertsInFaces;
    for(auto t_Face : a_FaceHandles)
    {
        auto t_Verts = get_verts_of_face(a_Mesh, t_Face);
        t_VertsInFaces.insert(t_VertsInFaces.end(), t_Verts.begin(), t_Verts.end());
    }

    // Eliminate the duplicated verts
    sort(t_VertsInFaces.begin(), t_VertsInFaces.end());
    t_VertsInFaces.erase(unique(t_VertsInFaces.begin(), t_VertsInFaces.end()), t_VertsInFaces.end());

    return t_VertsInFaces;
}

/**
 * \ingroup helper
 * @brief Given a neighborhood of faces, count how many quads are there.
 * 
 * @param a_Mesh The mesh containing the faces
 * @param a_FaceHandles The vector of face handles
 * @return the number of quads in the neighborhood 
 */
int num_of_quads(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles)
{
    int t_NumOfQuads = 0;
    for(auto t_Face : a_FaceHandles)
    {
        if(is_quad(a_Mesh, t_Face))
        {
            t_NumOfQuads++;
        }
    }
    return t_NumOfQuads;
}

/**
 * \ingroup helper
 * @brief Given a neighborhood of faces, count how many triangles are there.
 * 
 * @param a_Mesh The mesh containing the faces
 * @param a_FaceHandles The vector of face handles
 * @return the number of triangles in the neighborhood 
 */
int num_of_triangles(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles)
{
    int t_NumOfTriangles = 0;
    for(auto t_Face : a_FaceHandles)
    {
        if(is_triangle(a_Mesh, t_Face))
        {
            t_NumOfTriangles++;
        }
    }
    return t_NumOfTriangles;
}

/**
 * \ingroup helper
 * @brief Get the second layer of faces around the given face (unordered). Output faces do not include the first layer faces.
 * 
 * ex:
 * \code
 *      o - o - o - o - o - o
 *      | x | x | x | x | x |
 *      o - o - o - o - o - o
 *      | x |   |   |   | x |
 *      o - o - o - o - o - o  f: Given face
 *      | x |   | f |   | x |  x: Faces to get
 *      o - o - o - o - o - o
 *      | x |   |   |   | x |
 *      o - o - o - o - o - o
 *      | x | x | x | x | x |
 *      o - o - o - o - o - o
 * \endcode
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return The vector of face handles in the second layer around the given facehandle
 */
std::vector<FaceHandle> get_second_layer_faces_around_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    std::vector<FaceHandle> t_SecLayerFaces;

    // Get First layer faces
    auto t_FirstLayerFaces = init_neighbor_faces(a_Mesh, a_FaceHandle);

    // Get Verts of first layer face
    auto t_FirstLayerVertsOri = get_verts_of_faces(a_Mesh, t_FirstLayerFaces);
    auto t_FirstLayerVerts = t_FirstLayerVertsOri;

    // Remove the verts in central face
    for(auto t_Vert : t_FirstLayerVertsOri)
    {
        if(is_vert_in_face(a_Mesh, a_FaceHandle, t_Vert))
        {
            t_FirstLayerVerts.erase(
                std::remove(t_FirstLayerVerts.begin(), t_FirstLayerVerts.end(), t_Vert), t_FirstLayerVerts.end());
        }
    }


    // Get all faces around the first layer verts
    for(auto t_Vert : t_FirstLayerVerts)
    {
        auto t_Faces = get_faces_around_vert_counterclock(a_Mesh, t_Vert);
        t_SecLayerFaces.insert(t_SecLayerFaces.end(), t_Faces.begin(), t_Faces.end());
    }

    // Remove duplicate faces
    sort(t_SecLayerFaces.begin(), t_SecLayerFaces.end());
    t_SecLayerFaces.erase(
        unique(t_SecLayerFaces.begin(), t_SecLayerFaces.end()), t_SecLayerFaces.end());

    // Remove first layer faces
    for(auto t_FLFace : t_FirstLayerFaces)
    {
        t_SecLayerFaces.erase(std::remove(t_SecLayerFaces.begin(), t_SecLayerFaces.end(), t_FLFace), t_SecLayerFaces.end());
    }

    return t_SecLayerFaces;
}

/**
 * \ingroup helper
 * @brief Check if the given face is marked. A marked face is determined by the bool named property "marked_status" on a face.
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 * @return true if the face is marked
 */
bool is_marked(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    OpenMesh::FPropHandleT<bool> t_marked;
    a_Mesh.get_property_handle(t_marked, "marked_status");
    return a_Mesh.property(t_marked, a_FaceHandle);
}

/**
 * \ingroup helper
 * @brief Mark all the vertices of the given face. A marked vertex is determined by the bool named property "marked_status" on a vertex.
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 */
void mark_face_verts(MeshType& a_Mesh, const FaceHandle& a_FaceHandle){
    for(auto FVIt = a_Mesh.cfv_iter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        mark_vert(a_Mesh, *FVIt);
    }
}

/**
 * \ingroup helper
 * @brief Mark the given face. A marked face is determined by the bool named property "marked_status" on a face.
 * 
 * @param a_Mesh The mesh containing the face
 * @param a_FaceHandle The face handle
 */
void mark_face(MeshType& a_Mesh, const FaceHandle& a_FaceHandle){
    OpenMesh::FPropHandleT<bool> t_marked;
    a_Mesh.get_property_handle(t_marked, "marked_status");
    a_Mesh.property(t_marked, a_FaceHandle) = true;
}

// Type conversion

/**
 * \ingroup helper
 * @brief Convert a vertex handle to a 3D point vector
 * 
 * @param a_Mesh The mesh containing the vertex
 * @param a_VertHandle The vertex handle
 * @return Vec3d The 3D point vector that is a type alias for std::vector<double> 
 */
Vec3d verthandles_to_point_vec(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    Vec3d t_PointVec = {0, 0, 0};
    if(a_VertHandle.is_valid())
    {
        for(int i=0; i<3; i++)
        {
            t_PointVec[i] = a_Mesh.point(a_VertHandle)[i];
        }
    }
    return t_PointVec;
}

/**
 * \ingroup helper
 * @brief Convert a vector of vertex handles to a \ref Matrix of 3D points
 * 
 * @param a_Mesh The mesh containing the vertices
 * @param a_VertHandle The vector of vertex handles
 * @return Matrix The matrix of 3D points.
 */
Matrix verthandles_to_points_mat(const MeshType& a_Mesh, const std::vector<VertexHandle>& a_VertHandle)
{
    int t_NumOfVerts = a_VertHandle.size();
    auto t_PointMat = Matrix(t_NumOfVerts, 3);

    for(int i=0; i<t_NumOfVerts; i++)
    {
        t_PointMat(i) = verthandles_to_point_vec(a_Mesh, a_VertHandle[i]);
    }
    return t_PointMat;
}


// Patch

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to a \ref Patch
 * 
 * @param a_PointMat 
 * @return Patch 
 */
Patch points_mat_to_patch(const Matrix& a_PointMat)
{
    int t_PointsPerCol = pow(a_PointMat.getRows(), 0.5);
    Patch t_Patch(t_PointsPerCol-1);
    for(int i=0; i<a_PointMat.getRows(); i++)
    {
        t_Patch.m_BBcoefs[i/t_PointsPerCol][i%t_PointsPerCol] = {a_PointMat(i,0),a_PointMat(i,1),a_PointMat(i,2)};
    }
    return t_Patch;
}

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to a \ref Patch. The U and V degree of the patch are the same.
 * 
 * @param a_PatchDegU The degree of the patch in U direction
 * @param a_PatchDegV The degree of the patch in V direction
 * @param a_PointMat The matrix of 3D points. Each row is a 3D point.
 * @return Patch 
 */
Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const Matrix& a_PointMat)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to a \ref Patch
 * 
 * @param a_PatchDegU The degree of the patch in U direction
 * @param a_PatchDegV The degree of the patch in V direction
 * @param a_Group The group name of the patch
 * @param a_PointMat The matrix of 3D points. Each row is a 3D point.
 * @return Patch 
 */
Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV, a_Group);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to a \ref Patch
 * 
 * @param a_PatchDegU The degree of the patch in U direction
 * @param a_PatchDegV The degree of the patch in V direction
 * @param a_Group The group name of the patch
 * @param a_PointMat The matrix of 3D points.
 * @param a_StartIndex The start index of the points in the matrix.
 * @return Patch 
 */
Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat, const int a_StartIndex)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV, a_Group);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = a_StartIndex + i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to multiple \ref Patch es
 * 
 * @param a_NumOfPatch The number of patches
 * @param a_Group The group name of the patches
 * @param a_PointMat The matrix of 3D points. Each row is a 3D point.
 * @return The vector of patches
 */
std::vector<Patch> points_mat_to_patches(const int a_NumOfPatch, const std::string a_Group, const Matrix& a_PointMat)
{
    std::vector<Patch> t_Patches;
    const int t_NumOfCCPtsPerPatch = a_PointMat.getRows() / a_NumOfPatch;
    const int t_Deg = sqrt(t_NumOfCCPtsPerPatch) - 1;
    for(int i=0; i<a_NumOfPatch; i++ )
    {
        t_Patches.push_back(points_mat_to_patch(t_Deg, t_Deg, a_Group, a_PointMat, i*t_NumOfCCPtsPerPatch));
    }

    return t_Patches;
}

/**
 * \ingroup helper
 * @brief Convert a \ref Matrix of 3D points to multiple \ref Patch es
 * 
 * @param a_PatchDegU The degree of the patch in U direction
 * @param a_PatchDegV The degree of the patch in V direction
 * @param a_Group The group name of the patches
 * @param a_PointMat The matrix of 3D points. Each row is a 3D point.
 * @return The vector of patches
 */
std::vector<Patch> points_mat_to_patches(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat)
{
    std::vector<Patch> t_Patches;
    const int t_NumOfCCPtsPerPatch = (a_PatchDegU + 1) * (a_PatchDegV + 1);
    const int t_NumOfPatch = a_PointMat.getRows() / t_NumOfCCPtsPerPatch;

    for(int i=0; i<t_NumOfPatch; i++ )
    {
        t_Patches.push_back(points_mat_to_patch(a_PatchDegU, a_PatchDegV, a_Group, a_PointMat, i*t_NumOfCCPtsPerPatch));
    }

    return t_Patches;
}



// Others
/**
 * \ingroup helper
 * @brief Duplicate a vector multiple times
 * 
 * @tparam T The type of the vector elements
 * @param a_Times The number of times to duplicate the vector
 * @param a_Vector The vector to be duplicated
 * @return The duplicated vector
 */
template <typename T> std::vector<T> duplicate_vector(int a_Times, const std::vector<T>& a_Vector)
{
    std::vector<T> t_DuplicatedVector;
    for(int i=0; i<a_Times; i++)
    {
        t_DuplicatedVector.insert(t_DuplicatedVector.end(), a_Vector.begin(), a_Vector.end());
    }
    return t_DuplicatedVector;
}
template std::vector<int> duplicate_vector(int a_Times, const std::vector<int>& a_Vector);
template std::vector<double> duplicate_vector(int a_Times, const std::vector<double>& a_Vector);


} // end of namespace Helper
