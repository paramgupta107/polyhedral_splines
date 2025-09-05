/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "T0PatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/ReadCSV2Matrix.hpp"

Mat64x14d T0PatchConstructor::getMask()
{
    std::string t_MaskCSVFilePathT0 = "T0.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathT0, 64, 14); //  NumOfTotalCpts = 64, NumOfVerts = 14
}

bool T0PatchConstructor::isSamePatchType(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    // The face should be triangle
    if(!Helper::is_triangle(a_Mesh, a_FaceHandle))
    {
        return false;
    }

    // The verts of triangle should be one 5 valences and two 4 valences
    int t_5valenceVertCounter = 0;
    int t_4valenceVertCounter = 0;
    for(auto FVIt = a_Mesh.cfv_ccwiter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        if(Helper::is_vert_5_valence(a_Mesh, *FVIt))
        {
            t_5valenceVertCounter++;
        }
        else if(Helper::is_vert_4_valence(a_Mesh, *FVIt))
        {
            t_4valenceVertCounter++;
        }
    }
    if(t_5valenceVertCounter!=1 || t_4valenceVertCounter!=2)
    {
        return false;
    }

    // There should be 7 faces around the triangle as first layer
    auto t_FirstLayerFaces = Helper::init_neighbor_faces(a_Mesh, a_FaceHandle);
    if(!Helper::has_7_neighbor_faces(t_FirstLayerFaces))
    {
        return false;
    }

    // First layer faces should be all quads
    if(!Helper::are_faces_all_quads(a_Mesh, t_FirstLayerFaces))
    {
        return false;
    }

    std::cout << "T0" << std::endl;

    return true;
}

PatchBuilder T0PatchConstructor::getPatchBuilder(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    // Get neighbor verts
    auto t_NBVerts = initNeighborVerts(a_FaceHandle, a_Mesh);


    const int a_NumOfPatch = 4;
    return PatchBuilder(a_Mesh, t_NBVerts, m_Mask, this, a_NumOfPatch);
}


/*
 * Get the neighbor verts
 *      0 - 1 - 2
 *      |   |   |
 *      3 - 4 - 5
 *     /   / \   \
 *    6 - 7 - 8 - 9
 *    |   |   |   |
 *   10 -11 -12 - 13
 */
std::vector<VertexHandle> T0PatchConstructor::initNeighborVerts(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    // Init vector for neighbor points
    const int t_NumOfVerts = 14;
    std::vector<VertexHandle> t_NBVerts;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVerts);

    // Get triangle verts 4 7 8
    std::vector<VertexHandle> t_TriangleVerts;
    for(auto FVIt = a_Mesh.cfv_ccwiter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        t_TriangleVerts.push_back(*FVIt);
    }

    //Rotate vector to make five valence vert at postion 0
    while(true)
    {
        if(Helper::is_vert_5_valence(a_Mesh, t_TriangleVerts[0]))
        {
            break;
        }
        std::rotate(t_TriangleVerts.begin(), t_TriangleVerts.begin()+1, t_TriangleVerts.end());
    }

    // Store the verts into array
    std::vector<int> t_TriVertOrder = {4, 7, 8};
    for(int i=0; i<t_TriVertOrder.size(); i++)
    {
        t_NBVerts[t_TriVertOrder[i]] = t_TriangleVerts[i];
    }

    // Get the halfedge 8->4
    HalfedgeHandle t_HalfedgeHandle;
    for(auto FHIt=a_Mesh.cfh_ccwiter(a_FaceHandle); FHIt.is_valid(); ++FHIt)
    {
        if(t_NBVerts[8] == a_Mesh.from_vertex_handle(*FHIt))
        {
            t_HalfedgeHandle = *FHIt;
            break;
        }
    }

    // Get vert 5
    auto t_VertNo5 = HalfedgeOperation::get_verts(a_Mesh, t_HalfedgeHandle, {3, 2, 4});
    t_NBVerts[5] = t_VertNo5[0];

    // Get verts 2 1 0 3
    auto t_TopCommands = HalfedgeOperation::duplicate_commands(2, {3, 1, 1, 4, 1, 4});
    std::vector<int> t_TopVertOrder = {2, 1, 0, 3};
    HalfedgeOperation::init_verts(a_Mesh, t_HalfedgeHandle, t_NBVerts, t_TopVertOrder, t_TopCommands);

    // Get vert 6 10 11 12 13 9
    auto t_BottomCommands = HalfedgeOperation::duplicate_commands(2, {3, 1, 1, 4, 3, 1, 1, 4, 1, 4});
    std::vector<int> t_BottomVertOrder = {6, 10, 11, 12, 13, 9};
    HalfedgeOperation::init_verts(a_Mesh, t_HalfedgeHandle, t_NBVerts, t_BottomVertOrder, t_BottomCommands);

    return t_NBVerts;
}

std::string T0PatchConstructor::getGroupName() const
{
    return "Group 2 T0";
}