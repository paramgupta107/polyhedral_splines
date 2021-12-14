/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "ExtraordinaryPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/ReadCSV2Matrix.hpp"

Mat48x7d ExtraordinaryPatchConstructor::getMaskSct3()
{
    std::string t_MaskCSVFilePathSct3 = "./Table/eopSct3.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct3, 48, 7);
}

Mat80x11d ExtraordinaryPatchConstructor::getMaskSct5()
{
    std::string t_MaskCSVFilePathSct5 = "./Table/eopSct5.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct5, 80, 11);
}

Mat384x13d ExtraordinaryPatchConstructor::getMaskSct6()
{
    std::string t_MaskCSVFilePathSct6 = "./Table/eopSct6.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct6, 384, 13);
}

Mat448x15d ExtraordinaryPatchConstructor::getMaskSct7()
{
    std::string t_MaskCSVFilePathSct7 = "./Table/eopSct7.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct7, 448, 15);
}

Mat512x17d ExtraordinaryPatchConstructor::getMaskSct8()
{
    std::string t_MaskCSVFilePathSct8 = "./Table/eopSct8.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct8, 512, 17);
}

bool ExtraordinaryPatchConstructor::isSamePatchType(const VertexHandle& a_VertexHandle)
{
    // Extraordinary point should be 3, 5, 6, 7, or 8 valence
    int t_Valence = Helper::get_vert_valence(m_Mesh, a_VertexHandle);
    if(t_Valence<3 || t_Valence==4 || t_Valence>8)
    {
        return false;
    }

    // Can not be on the edge
    if(m_Mesh.is_boundary(a_VertexHandle))
    {
        return false;
    }

    // The first layer of faces should all be quad
    auto t_FirstLayerFaces = Helper::get_faces_around_vert_counterclock(m_Mesh, a_VertexHandle);
    for(auto t_Vert : t_FirstLayerFaces)
    {
        if(!Helper::are_faces_all_quads(m_Mesh, t_FirstLayerFaces))
        {
            return false;
        }
    }

    std::cout << t_Valence << "-valent EOP" << std::endl;

    return true;
}

/*
 * The index of neighbor verts (following example is for 3 valent point)
 *        0
 *      / | \
 *     /  |  \
 *    1   6   5
 *    \  / \ /
 *     2    4
 *      \  /
 *       3
 */
std::vector<VertexHandle> ExtraordinaryPatchConstructor::initNeighborVerts(const VertexHandle& a_VertexHandle)
{
    std::vector<VertexHandle> t_VertexHandles;
    for(auto t_VHIt = m_Mesh.cvoh_ccwiter(a_VertexHandle); t_VHIt.is_valid(); ++t_VHIt)
    {
        auto t_NBVerts = HalfedgeOperation::get_verts_fixed_halfedge(m_Mesh, *t_VHIt, {1,4,1,4});
        for(auto t_NBVert : t_NBVerts)
        {
            t_VertexHandles.push_back(t_NBVert);
        }
    }

    // store the central point
    t_VertexHandles.push_back(a_VertexHandle);

    return t_VertexHandles;
}


std::vector<Patch> ExtraordinaryPatchConstructor::getPatch(const VertexHandle& a_VertexHandle)
{
    // Get valence of extraordinary point to determine the # of cpts
    int t_ExtrPointValence = Helper::get_vert_valence(m_Mesh, a_VertexHandle);

    // Get neighbor verts
    auto t_NBVerts = initNeighborVerts(a_VertexHandle);

    // Convert Neighbor Verts to matrix type
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(m_Mesh, t_NBVerts);

    // multiply CC points with mask to generate patches
    Matrix t_BBcoefs;
    switch(t_ExtrPointValence)
    {
        case 3:
            t_BBcoefs = m_MaskSct3 * t_NBVertsMat;
            break;
        case 5:
            t_BBcoefs = m_MaskSct5 * t_NBVertsMat;
            break;
        case 6:
            t_BBcoefs = m_MaskSct6 * t_NBVertsMat;
            break;
        case 7:
            t_BBcoefs = m_MaskSct7 * t_NBVertsMat;
            break;
        case 8:
            t_BBcoefs = m_MaskSct8 * t_NBVertsMat;
            break;
    }

    int t_NumOfPatches = t_ExtrPointValence;
    if(t_ExtrPointValence >= 6)
    {
        t_NumOfPatches *= 4; // have four patches in each sector
    }


    // Save control points into Patch
    return Helper::points_mat_to_patches(t_NumOfPatches, "Group 1 ExtraordinaryPoint", t_BBcoefs);
}
