/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#include "T0PatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/Helper.hpp"

EGMat64x14d T0PatchConstructor::getMask()
{
    EGMat64x14d t_Mask;
    t_Mask << \
    0,0,0,0,0,0,0,0.25,0.25,0,0,0.25,0.25,0,
    0,0,0,0,0,0,0,0.4166666667,0.08333333333,0,0,0.4166666667,0.08333333333,0,
    0,0,0,0,0,0,0.08333333333,0.4166666667,0,0,0.08333333333,0.4166666667,0,0,
    0,0,0,0,0,0,0.25,0.25,0,0,0.25,0.25,0,0,
    0,0,0,0,0,0,0,0.4166666667,0.4166666667,0,0,0.08333333333,0.08333333333,0,
    0,0,0,0,0,0,0,0.7083333333,0.125,0,0,0.1527777778,0.01388888889,0,
    0,0,0,0,0,0,0.1527777778,0.6805555556,0,0,0.04166666667,0.125,0,0,
    0,0,0,0,0,0,0.4166666667,0.4166666667,0,0,0.08333333333,0.08333333333,0,0,
    0,0,0,-0.01041666667,0.1875,-0.01041666667,0.03125,0.3854166667,0.3854166667,0.03125,0,0,0,0,
    0,0,0,-0.003472222222,0.1875,-0.01736111111,0.07986111111,0.5451388889,0.2256944444,-0.01736111111,0,0,0,0,
    0,0,0,0.02777777778,0.1388888889,0,0.1944444444,0.6388888889,0,0,0,0,0,0,
    0,0,0,0.08333333333,0.08333333333,0,0.4166666667,0.4166666667,0,0,0,0,0,0,
    0,0,0,0.03125,0.4375,0.03125,0.03125,0.21875,0.21875,0.03125,0,0,0,0,
    0,0,0,0.0625,0.4375,0,0.0625,0.3125,0.125,0,0,0,0,0,
    0,0,0,0.125,0.375,0,0.125,0.375,0,0,0,0,0,0,
    0,0,0,0.25,0.25,0,0.25,0.25,0,0,0,0,0,0,
    0,0,0,0.03125,0.4375,0.03125,0.03125,0.21875,0.21875,0.03125,0,0,0,0,
    0,0,0,0.0625,0.4375,0,0.0625,0.3125,0.125,0,0,0,0,0,
    0,0,0,0.125,0.375,0,0.125,0.375,0,0,0,0,0,0,
    0,0,0,0.25,0.25,0,0.25,0.25,0,0,0,0,0,0,
    0,0,0,0.07291666667,0.6875,0.07291666667,0.03125,0.05208333333,0.05208333333,0.03125,0,0,0,0,
    0,0,0,0.1284722222,0.6875,0.01736111111,0.04513888889,0.07986111111,0.02430555556,0.01736111111,0,0,0,0,
    0,0,0,0.2222222222,0.6111111111,0,0.05555555556,0.1111111111,0,0,0,0,0,0,
    0,0,0,0.4166666667,0.4166666667,0,0.08333333333,0.08333333333,0,0,0,0,0,0,
    0.02083333333,0.125,0.02083333333,0.1041666667,0.625,0.1041666667,0,0,0,0,0,0,0,0,
    0.02777777778,0.125,0.01388888889,0.1666666667,0.625,0.04166666667,0,0,0,0,0,0,0,0,
    0.04166666667,0.125,0,0.2638888889,0.5694444444,0,0,0,0,0,0,0,0,0,
    0.08333333333,0.08333333333,0,0.4166666667,0.4166666667,0,0,0,0,0,0,0,0,0,
    0.0625,0.375,0.0625,0.0625,0.375,0.0625,0,0,0,0,0,0,0,0,
    0.1041666667,0.375,0.02083333333,0.1041666667,0.375,0.02083333333,0,0,0,0,0,0,0,0,
    0.1666666667,0.3333333333,0,0.1666666667,0.3333333333,0,0,0,0,0,0,0,0,0,
    0.25,0.25,0,0.25,0.25,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0.25,0.25,0,0,0.25,0.25,
    0,0,0,0,0,0,0,0,0.4166666667,0.08333333333,0,0,0.4166666667,0.08333333333,
    0,0,0,0,0,0,0,0.08333333333,0.4166666667,0,0,0.08333333333,0.4166666667,0,
    0,0,0,0,0,0,0,0.25,0.25,0,0,0.25,0.25,0,
    0,0,0,0,0,0,0,0,0.4166666667,0.4166666667,0,0,0.08333333333,0.08333333333,
    0,0,0,0,0,0,0,0,0.6805555556,0.1527777778,0,0,0.125,0.04166666667,
    0,0,0,0,0,0,0,0.125,0.7083333333,0,0,0.01388888889,0.1527777778,0,
    0,0,0,0,0,0,0,0.4166666667,0.4166666667,0,0,0.08333333333,0.08333333333,0,
    0,0,0,0,0.08333333333,0.08333333333,0,0,0.4166666667,0.4166666667,0,0,0,0,
    0,0,0,0,0.1388888889,0.02777777778,0,0,0.6388888889,0.1944444444,0,0,0,0,
    0,0,0,-0.01736111111,0.1875,-0.003472222222,-0.01736111111,0.2256944444,0.5451388889,0.07986111111,0,0,0,0,
    0,0,0,-0.01041666667,0.1875,-0.01041666667,0.03125,0.3854166667,0.3854166667,0.03125,0,0,0,0,
    0,0,0,0,0.25,0.25,0,0,0.25,0.25,0,0,0,0,
    0,0,0,0,0.375,0.125,0,0,0.375,0.125,0,0,0,0,
    0,0,0,0,0.4375,0.0625,0,0.125,0.3125,0.0625,0,0,0,0,
    0,0,0,0.03125,0.4375,0.03125,0.03125,0.21875,0.21875,0.03125,0,0,0,0,
    0,0,0,0,0.25,0.25,0,0,0.25,0.25,0,0,0,0,
    0,0,0,0,0.375,0.125,0,0,0.375,0.125,0,0,0,0,
    0,0,0,0,0.4375,0.0625,0,0.125,0.3125,0.0625,0,0,0,0,
    0,0,0,0.03125,0.4375,0.03125,0.03125,0.21875,0.21875,0.03125,0,0,0,0,
    0,0,0,0,0.4166666667,0.4166666667,0,0,0.08333333333,0.08333333333,0,0,0,0,
    0,0,0,0,0.6111111111,0.2222222222,0,0,0.1111111111,0.05555555556,0,0,0,0,
    0,0,0,0.01736111111,0.6875,0.1284722222,0.01736111111,0.02430555556,0.07986111111,0.04513888889,0,0,0,0,
    0,0,0,0.07291666667,0.6875,0.07291666667,0.03125,0.05208333333,0.05208333333,0.03125,0,0,0,0,
    0,0.08333333333,0.08333333333,0,0.4166666667,0.4166666667,0,0,0,0,0,0,0,0,
    0,0.125,0.04166666667,0,0.5694444444,0.2638888889,0,0,0,0,0,0,0,0,
    0.01388888889,0.125,0.02777777778,0.04166666667,0.625,0.1666666667,0,0,0,0,0,0,0,0,
    0.02083333333,0.125,0.02083333333,0.1041666667,0.625,0.1041666667,0,0,0,0,0,0,0,0,
    0,0.25,0.25,0,0.25,0.25,0,0,0,0,0,0,0,0,
    0,0.3333333333,0.1666666667,0,0.3333333333,0.1666666667,0,0,0,0,0,0,0,0,
    0.02083333333,0.375,0.1041666667,0.02083333333,0.375,0.1041666667,0,0,0,0,0,0,0,0,
    0.0625,0.375,0.0625,0.0625,0.375,0.0625,0,0,0,0,0,0,0,0;

    return t_Mask;
}

bool T0PatchConstructor::isSamePatchType(const FaceHandle& a_FaceHandle)
{
    // The face should be triangle
    if(!Helper::is_triangle(m_Mesh, a_FaceHandle))
    {
        return false;
    }

    // The verts of triangle should be one 5 valences and two 4 valences
    int t_5valenceVertCounter = 0;
    int t_4valenceVertCounter = 0;
    for(auto FVIt = m_Mesh.cfv_ccwiter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        if(Helper::is_vert_5_valence(m_Mesh, *FVIt))
        {
            t_5valenceVertCounter++;
        }
        else if(Helper::is_vert_4_valence(m_Mesh, *FVIt))
        {
            t_4valenceVertCounter++;
        }
    }
    if(t_5valenceVertCounter!=1 || t_4valenceVertCounter!=2)
    {
        return false;
    }

    // There should be 7 faces around the triangle as first layer
    auto t_FirstLayerFaces = Helper::init_neighbor_faces(m_Mesh, a_FaceHandle);
    if(!Helper::has_7_neighbor_faces(t_FirstLayerFaces))
    {
        return false;
    }

    // First layer faces should be all quads
    if(!Helper::are_faces_all_quads(m_Mesh, t_FirstLayerFaces))
    {
        return false;
    }

    std::cout << "T0" << std::endl;

    return true;
}

std::vector<Patch> T0PatchConstructor::getPatch(const FaceHandle& a_FaceHandle)
{
    // Get neighbor verts
    auto t_NBVerts = initNeighborVerts(a_FaceHandle);

    // Convert Neighbor Verts to Eigen matrix type
    auto t_EGNBVerts = Helper::verthandles_to_EGPoints(m_Mesh, t_NBVerts);

    // Generate patch
    auto t_BBcoefs = m_Mask * t_EGNBVerts;

    const int a_NumOfPatch = 4;
    return Helper::EGPoints_to_patches(a_NumOfPatch, "Group 2 T0", t_BBcoefs);
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
std::vector<VertexHandle> T0PatchConstructor::initNeighborVerts(const FaceHandle& a_FaceHandle)
{
    // Init vector for neighbor points
    const int t_NumOfVerts = 14;
    std::vector<VertexHandle> t_NBVerts;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVerts);

    // Get triangle verts 4 7 8
    std::vector<VertexHandle> t_TriangleVerts;
    for(auto FVIt = m_Mesh.cfv_ccwiter(a_FaceHandle); FVIt.is_valid(); ++FVIt)
    {
        t_TriangleVerts.push_back(*FVIt);
    }

    //Rotate vector to make five valence vert at postion 0
    while(true)
    {
        if(Helper::is_vert_5_valence(m_Mesh, t_TriangleVerts[0]))
        {
            break;
        }
        std::rotate(t_TriangleVerts.begin(),t_TriangleVerts.begin()+1,t_TriangleVerts.end());
    }

    // Store the verts into array
    std::vector<int> t_TriVertOrder = {4, 7, 8};
    for(int i=0; i<t_TriVertOrder.size(); i++)
    {
        t_NBVerts[t_TriVertOrder[i]] = t_TriangleVerts[i];
    }

    // Get the halfedge 8->4
    HalfedgeHandle t_HalfedgeHandle;
    for(auto FHIt=m_Mesh.cfh_ccwiter(a_FaceHandle); FHIt.is_valid(); ++FHIt)
    {
        if(t_NBVerts[8] == m_Mesh.from_vertex_handle(*FHIt))
        {
            t_HalfedgeHandle = *FHIt;
            break;
        }
    }

    // Get vert 5
    auto t_VertNo5 = HalfedgeOperation::get_verts(m_Mesh, t_HalfedgeHandle, {3, 2, 4});
    t_NBVerts[5] = t_VertNo5[0];

    // Get verts 2 1 0 3
    auto t_TopCommands = HalfedgeOperation::duplicate_commands(2, {3, 1, 1, 4, 1, 4});
    std::vector<int> t_TopVertOrder = {2, 1, 0, 3};
    HalfedgeOperation::init_verts(m_Mesh, t_HalfedgeHandle, t_NBVerts, t_TopVertOrder, t_TopCommands);

    // Get vert 6 10 11 12 13 9
    auto t_BottomCommands = HalfedgeOperation::duplicate_commands(2, {3, 1, 1, 4, 3, 1, 1, 4, 1, 4});
    std::vector<int> t_BottomVertOrder = {6, 10, 11, 12, 13, 9};
    HalfedgeOperation::init_verts(m_Mesh, t_HalfedgeHandle, t_NBVerts, t_BottomVertOrder, t_BottomCommands);

    return t_NBVerts;
}
