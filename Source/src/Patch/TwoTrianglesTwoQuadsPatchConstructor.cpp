/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "TwoTrianglesTwoQuadsPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"

/*
 * Check if the current face (facehandle) and its neighbors match the Regular structure
 */
bool TwoTrianglesTwoQuadsPatchConstructor::isSamePatchType(const VertexHandle& a_VertHandle)
{
    // Check if the vertex four valence
    if(!Helper::is_vert_4_valence(m_Mesh, a_VertHandle))
    {
        return false;
    }

    // Get faces around the vert
    auto t_NBFaceHandles = Helper::get_faces_around_vert_counterclock(m_Mesh, a_VertHandle);

    // Check if there are four faces around the vertex
    const int t_NumOfFacesForRegCase = 4;
    if(Helper::get_num_of_neighbor_faces(t_NBFaceHandles) != t_NumOfFacesForRegCase)
    {
        return false;
    }

    // Check whether there are two quads and two triangles
    if(Helper::num_of_quads(m_Mesh, t_NBFaceHandles)!=2)
    {
        return false;
    }

    if(Helper::num_of_triangles(m_Mesh, t_NBFaceHandles)!=2)
    {
        return false;
    }

    //make sure two trangles and two quads are connected.
    bool t_IsPreFaceTriagnle = Helper::is_triangle(m_Mesh, t_NBFaceHandles[0]);
    bool t_IsNextFaceTriagnle = Helper::is_triangle(m_Mesh, t_NBFaceHandles[2]);
    if(t_IsPreFaceTriagnle == t_IsNextFaceTriagnle)
    {
        return false;
    }

    std::cout<< "2 Tri 2 Quads" << std::endl;

    return true;
}


/*
 * Find the neighbor verts around the given face, then use the verts to generate patch
 */
PatchBuilder TwoTrianglesTwoQuadsPatchConstructor::getPatchBuilder(const VertexHandle& a_VertHandle)
{
    auto t_NBVertexHandles = initNeighborVerts(a_VertHandle);
    const int t_PatchDegU = 2;
    const int t_PatchDegV = 2;
    return PatchBuilder(t_NBVertexHandles, m_Mask, this, m_Mesh, t_PatchDegU, t_PatchDegV);
}


/*
 * Initialize (obtain) the vertices on original mesh needed for getPatch
 *
 *                 1 --- 2     0 3 6 use same vertex
 *               / |     |
 *              /  |     |
 *      (0,3,6) -- 4 --- 5
 *              \  |     |
 *               \ |     |
 *                 7 --- 8
 */
std::vector<VertexHandle> TwoTrianglesTwoQuadsPatchConstructor::initNeighborVerts(const VertexHandle& a_VertHandle)
{
    // Init m_NeighborVerts with 9 default vertexhandle
    const int t_NumOfVerts = 9;
    std::vector<VertexHandle> t_NBVertexHandles;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVertexHandles);

    // Get halfedge 4->5
    HalfedgeHandle t_HE;
    for(auto VHIt=m_Mesh.cvoh_ccwiter(a_VertHandle); VHIt.is_valid(); ++VHIt)
    {
        // 4->5 halfedge has two adj faces
        auto t_PreFace = m_Mesh.face_handle(*VHIt);
        auto t_CurrentFace = m_Mesh.face_handle(m_Mesh.opposite_halfedge_handle(*VHIt));
        if(Helper::is_quad(m_Mesh, t_PreFace) && Helper::is_quad(m_Mesh, t_CurrentFace))
        {
            t_HE = *VHIt;
        }
    }

    /* Using halfedge operation to get value for each neighbors
     * Operations: 1=next 2=previous 3=opposite 4=get
     *  Vertex index:
     *    0  1  2
     *    3  4  5
     *    6  7  8
     */
    std::vector<int> t_GetVertOrder{4,5,2,1,0,3,6,7,8};
    std::vector<int> t_Command = {4,1,4,1,4,1,4,3,1,1,4,4,3,1,4,1,4,3,1,1,4};

    HalfedgeOperation::init_verts(m_Mesh, t_HE, t_NBVertexHandles, t_GetVertOrder, t_Command);

    return t_NBVertexHandles;
}

std::string TwoTrianglesTwoQuadsPatchConstructor::getGroupName() const
{
    return "Group 0 2tri2quad";
}