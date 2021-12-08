/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "RegularPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"

/*
 * Check if the current face (facehandle) and its neighbors match the Regular structure
 */
bool RegularPatchConstructor::isSamePatchType(const VertexHandle& a_VertHandle)
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

    // Check whether 4 neighbor faces are all quad
    if(!Helper::are_faces_all_quads(m_Mesh, t_NBFaceHandles))
    {
        return false;
    }

    return true;
}


/*
 * Find the neighbor verts around the given face, then use the verts to generate patch
 */
std::vector<Patch> RegularPatchConstructor::getPatch(const VertexHandle& a_VertHandle)
{
    auto t_NBVertexHandles = initNeighborVerts(a_VertHandle);
    std::vector<Patch> t_Patches;
    t_Patches.push_back(getPatch(t_NBVertexHandles));
    return t_Patches;
}

/*
 * Given neighbor verts, multiply it with mask to generate patch.
 */
Patch RegularPatchConstructor::getPatch(const std::vector<VertexHandle>& a_NBVertexHandles)
{
    Matrix t_BBcoefs = getPatchMat(a_NBVertexHandles);

    const int t_PatchDegU = 2;
    const int t_PatchDegV = 2;
    return Helper::points_mat_to_patch(t_PatchDegU, t_PatchDegV, "Group 0 Regular", t_BBcoefs);
}

/*
 * Given neighbor verts, multiply it with mask to generate patch in matrix form.
 */
Matrix RegularPatchConstructor::getPatchMat(const std::vector<VertexHandle>& a_NBVertexHandles)
{
    //convert NeighborVerts to matrix type
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(m_Mesh, a_NBVertexHandles);

    // getPatch
    auto t_BBcoefs = m_Mask * t_NBVertsMat;

    return t_BBcoefs;
}


/*
 * Initialize (obtain) the vertices on original mesh needed for getPatch
 */
std::vector<VertexHandle> RegularPatchConstructor::initNeighborVerts(const VertexHandle& a_VertHandle)
{
    // Init m_NeighborVerts with 9 default vertexhandle
    const int t_NumOfVerts = 9;
    std::vector<VertexHandle> t_NBVertexHandles;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVertexHandles);

    // Get center vertex
    t_NBVertexHandles[4] = a_VertHandle;

    // Get the halfedge belonds to current face
    auto t_CurrentHef = m_Mesh.halfedge_handle(a_VertHandle);

    /* Using halfedge operation to get value for each neighbors
     * Operations: 1=next 2=previous 3=opposite 4=get
     *  Vertex index:
     *    0  1  2
     *    3  4  5
     *    6  7  8
     */
    // Four wings
    std::vector<std::vector<int>> t_GetVertOrder{{1,0}, {3,6}, {7,8}, {5,2}};

    // Operation for one wing
    std::vector<int> t_WingOperation{1,4,1,4,1,3};

    HalfedgeOperation::init_verts(m_Mesh, t_CurrentHef, t_NBVertexHandles, t_GetVertOrder, t_WingOperation);

    return t_NBVertexHandles;
}
