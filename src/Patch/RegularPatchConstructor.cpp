/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "RegularPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"

/*
 * Check if the current face (facehandle) and its neighbors match the Regular structure
 */
bool RegularPatchConstructor::isSamePatchType(const VertexHandle& a_VertHandle, MeshType& a_Mesh, bool check_marked)
{
    if(check_marked && Helper::is_marked(a_Mesh, a_VertHandle))
    {
        return false;
    }
    // Check if the vertex four valence
    if(!Helper::is_vert_4_valence(a_Mesh, a_VertHandle))
    {
        return false;
    }

    // The surrounding faces should be either quad or triangle and no more than two
    // consecutive triangles.
    auto t_FHs = Helper::get_faces_around_vert_counterclock(a_Mesh, a_VertHandle);
    if(t_FHs.size() != 4)
    {
        return false;
    }

    // If the vertex neighbors a polar patch, only collect if the neighboring vertex is actually polar
    bool t_isPolarSurroundingVert = Helper::is_polar_surrounding_vert(a_Mesh, a_VertHandle);
    if (t_isPolarSurroundingVert && !Helper::is_polar(a_Mesh, Helper::find_polar_vertex(a_Mesh, a_VertHandle))) {
        return false;
    }

    // The first layer of faces should all be quad or 2 consecutive triangles
    if(!Helper::are_faces_all_quads(a_Mesh, t_FHs) && !t_isPolarSurroundingVert)
    {
        return false;
    }

    return true;
}


/*
 * Find the neighbor verts around the given face, then use the verts to generate patch
 */
PatchBuilder RegularPatchConstructor::getPatchBuilder(const VertexHandle& a_VertHandle, MeshType& a_Mesh, bool mark_gathered)
{
    auto t_NBVertexHandles = initNeighborVerts(a_VertHandle, a_Mesh);
    Matrix t_mask = m_Mask;
    const int t_PatchDegU = 2;
    const int t_PatchDegV = 2;
    if(mark_gathered)
    {
        Helper::mark_vert(a_Mesh, a_VertHandle);
    }
    return PatchBuilder(a_Mesh, t_NBVertexHandles, t_mask, this, t_PatchDegU, t_PatchDegV);
}

/*
 * Initialize (obtain) the vertices on original mesh needed for getPatch
 */
std::vector<VertexHandle> RegularPatchConstructor::initNeighborVerts(const VertexHandle& a_VertHandle, MeshType& a_Mesh)
{
    // Init m_NeighborVerts with 9 default vertexhandle
    const int t_NumOfVerts = 9;
    std::vector<VertexHandle> t_NBVertexHandles;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVertexHandles);

    // Get center vertex
    t_NBVertexHandles[4] = a_VertHandle;

    // Get the halfedge belonds to current face
    auto t_CurrentHef = a_Mesh.halfedge_handle(a_VertHandle);

    /* Using halfedge operation to get value for each neighbors
     * Operations: 1=next 2=previous 3=opposite 4=get
     *  Vertex index:
     *    0  1  2
     *    3  4  5
     *    6  7  8
     */
    // Four wings
    std::vector<int> t_GetVertOrder = {1,0,3,6,7,8,5,2};

    // // Operation for one wing
    // std::vector<int> t_WingOperation{1,4,1,4,1,3};

    // HalfedgeOperation::init_verts(a_Mesh, t_CurrentHef, t_NBVertexHandles, t_GetVertOrder, t_WingOperation);
    int i = 0;
    std::vector<VertexHandle> t_NBVerts;
    for(auto t_VHIt = a_Mesh.cvoh_ccwiter(a_VertHandle); t_VHIt.is_valid(); ++t_VHIt)
    {
        auto t_CurrFH = a_Mesh.face_handle(*t_VHIt);
        std::vector<int> t_Commands;
        if(Helper::is_quad(a_Mesh, t_CurrFH))
        {
            t_Commands = {1,4,1,4};
        }
        else if(Helper::is_triangle(a_Mesh, t_CurrFH))
        {
            auto t_OppositeHE = a_Mesh.opposite_halfedge_handle(*t_VHIt);
            auto t_PreFH = a_Mesh.face_handle(t_OppositeHE);
            if(Helper::is_quad(a_Mesh, t_PreFH))
            {
                t_Commands = {1,4,1,4};
            }
            else
            {
                t_Commands = {1,4,4};
            }
        }
        else
        {
            std::cout << "Error: Not a quad or triangle face!" << std::endl;
            assert(false);
        }

        t_NBVerts = HalfedgeOperation::get_verts_fixed_halfedge(a_Mesh, *t_VHIt, t_Commands);

        for(auto t_NBVert : t_NBVerts)
        {
            t_NBVertexHandles[t_GetVertOrder[i]] = t_NBVert;
            i++;
        }
    }

    return t_NBVertexHandles;
}

std::string RegularPatchConstructor::getGroupName() const
{
    return "Group 0 Regular";
}