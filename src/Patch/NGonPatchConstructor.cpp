/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "NGonPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/ReadCSV2Matrix.hpp"

Mat48x12d NGonPatchConstructor::getMaskSct3()
{
    std::string t_MaskCSVFilePathSct3 = "ngonSct3.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct3, 48, 12);
}

Mat80x20d NGonPatchConstructor::getMaskSct5()
{
    std::string t_MaskCSVFilePathSct5 = "ngonSct5.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct5, 80, 20);
}

Mat384x24d NGonPatchConstructor::getMaskSct6()
{
    std::string t_MaskCSVFilePathSct6 = "ngonSct6.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct6, 384, 24);
}

Mat448x28d NGonPatchConstructor::getMaskSct7()
{
    std::string t_MaskCSVFilePathSct7 = "ngonSct7.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct7, 448, 28);
}

Mat512x32d NGonPatchConstructor::getMaskSct8()
{
    std::string t_MaskCSVFilePathSct8 = "ngonSct8.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct8, 512, 32);
}

bool NGonPatchConstructor::isSamePatchType(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    // The face should have 3 5 6 7 8 vertices
    auto t_FaceValence = Helper::get_num_of_verts_for_face(a_Mesh, a_FaceHandle);
    if(t_FaceValence < 3 || t_FaceValence > 8 || t_FaceValence == 4)
    {
        return false;
    }

    // All the vertices of central face should be 4-valent
    if(!Helper::are_verts_of_face_all_4_valence(a_Mesh, a_FaceHandle))
    {
        return false;
    }

    // All surrounded faces should be quads
    auto t_FirstLayerFaces = Helper::init_neighbor_faces(a_Mesh, a_FaceHandle);
    if(!Helper::are_faces_all_quads(a_Mesh, t_FirstLayerFaces))
    {
        return false;
    }

    std::cout << t_FaceValence << "-gon" << std::endl;

    return true;
}

PatchBuilder NGonPatchConstructor::getPatchBuilder(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    m_FaceValence = Helper::get_num_of_verts_for_face(a_Mesh, a_FaceHandle);

    // Get neighbor verts
    auto t_NBVerts = initNeighborVerts(a_FaceHandle, a_Mesh);

    // Convert Neighbor Verts to matrix type
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(a_Mesh, t_NBVerts);

    // Generate patch
    Matrix t_mask;
    switch(m_FaceValence)
    {
        case 3:
            t_mask = m_MaskSct3;
            break;
        case 5:
            t_mask = m_MaskSct5;
            break;
        case 6:
            t_mask = m_MaskSct6;
            break;
        case 7:
            t_mask = m_MaskSct7;
            break;
        case 8:
            t_mask = m_MaskSct8;
            break;
    }

    // Only one bi3 patch for each sector when n = 3,5 other wise four per sector
    int a_NumOfPatch = m_FaceValence;
    if(m_FaceValence!=3 && m_FaceValence!=5)
    {
        a_NumOfPatch = a_NumOfPatch * 4;
    }

    return PatchBuilder(a_Mesh, t_NBVerts, t_mask, this, a_NumOfPatch);
}


/*
 * Get the neighbor verts
 *           8
 *        /    \
 *      9 - 11- 10
 *     /   / \   \
 *    2 - 3 - 7 - 5
 *    |   |   |   |
 *    0 - 1 - 6 - 4
 */
std::vector<VertexHandle> NGonPatchConstructor::initNeighborVerts(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh)
{
    // Init vector for neighbor points
    const int t_NumOfVerts = 4 * m_FaceValence;
    std::vector<VertexHandle> t_NBVerts;
    Helper::set_vert_vector_to_default(t_NumOfVerts, t_NBVerts);

    // get vertices corner by corner (0 1 2 3 is a corner)
    // Get the halfedge 3->7
    int t_WingID = 0;
    for(auto t_FHIt = a_Mesh.cfh_ccwiter(a_FaceHandle); t_FHIt.is_valid(); t_FHIt++)
    {
        // Move halfedge to 0->1 and get vert 0 1 3 2
        int k = t_WingID * 4;
        std::vector<int> t_IDs = {3+k, 1+k, 0+k, 2+k};
        HalfedgeOperation::init_verts_fixed_halfedge(a_Mesh, *t_FHIt, t_NBVerts, t_IDs, {3,1,4,3,4,2,4,2,4});

        t_WingID++;
    }

    return t_NBVerts;
}

std::string NGonPatchConstructor::getGroupName() const
{
    return "Group 8 nGon";
}