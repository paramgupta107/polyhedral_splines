/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "ExtraordinaryPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/ReadCSV2Matrix.hpp"

Mat48x7d ExtraordinaryPatchConstructor::getMaskSct3()
{
    std::string t_MaskCSVFilePathSct3 = "eopSct3.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct3, 48, 7);
}

Mat80x11d ExtraordinaryPatchConstructor::getMaskSct5()
{
    std::string t_MaskCSVFilePathSct5 = "eopSct5.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct5, 80, 11);
}

Mat384x13d ExtraordinaryPatchConstructor::getMaskSct6()
{
    std::string t_MaskCSVFilePathSct6 = "eopSct6.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct6, 384, 13);
}

Mat448x15d ExtraordinaryPatchConstructor::getMaskSct7()
{
    std::string t_MaskCSVFilePathSct7 = "eopSct7.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct7, 448, 15);
}

Mat512x17d ExtraordinaryPatchConstructor::getMaskSct8()
{
    std::string t_MaskCSVFilePathSct8 = "eopSct8.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct8, 512, 17);
}

bool ExtraordinaryPatchConstructor::isSamePatchType(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh)
{
    // Extraordinary point should be 3, 5, 6, 7, or 8 valence
    int t_Valence = Helper::get_vert_valence(a_Mesh, a_VertexHandle);
    if(t_Valence<3 || t_Valence==4 || t_Valence>8)
    {
        return false;
    }

    // Can not be on the edge
    if(a_Mesh.is_boundary(a_VertexHandle))
    {
        return false;
    }

    // The first layer of faces should all be quad or 2 consecutive triangles
    auto t_FirstLayerFaces = Helper::get_faces_around_vert_counterclock(a_Mesh, a_VertexHandle);
    if(!Helper::are_faces_all_quads(a_Mesh, t_FirstLayerFaces) && !Helper::is_polar_surrounding_vert(a_Mesh, a_VertexHandle))
        {
            return false;
        }

    // // The surrounding faces should be either quad or triangle and no more than two
    // // consecutive triangles.
    // auto t_FHs = Helper::get_faces_around_vert_counterclock(a_Mesh, a_VertexHandle);
    // bool t_IsPrevTri = false;
    // for(int i=0; i<t_FHs.size(); i++)
    // {
    //     if(!Helper::is_triangle(a_Mesh, t_FHs[i]) && !Helper::is_quad(a_Mesh, t_FHs[i]))
    //     {
    //         return false;
    //     }
    //
    //     if(Helper::is_triangle(a_Mesh, t_FHs[i]) &&
    //        Helper::is_triangle(a_Mesh, t_FHs[(i+1)%t_FHs.size()]) &&
    //        Helper::is_triangle(a_Mesh, t_FHs[(i+2)%t_FHs.size()]))
    //     {
    //         return false;
    //     }
    // }

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
std::vector<VertexHandle> ExtraordinaryPatchConstructor::initNeighborVerts(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh)
{
    std::vector<VertexHandle> t_VertexHandles;
    std::vector<VertexHandle> t_NBVerts;
    for(auto t_VHIt = a_Mesh.cvoh_ccwiter(a_VertexHandle); t_VHIt.is_valid(); ++t_VHIt)
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
            assert(false);
        }

        t_NBVerts = HalfedgeOperation::get_verts_fixed_halfedge(a_Mesh, *t_VHIt, t_Commands);

        for(auto t_NBVert : t_NBVerts)
        {
            t_VertexHandles.push_back(t_NBVert);
        }
    }

    // store the central point
    t_VertexHandles.push_back(a_VertexHandle);

    return t_VertexHandles;
}


PatchBuilder ExtraordinaryPatchConstructor::getPatchBuilder(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh)
{
    // Get valence of extraordinary point to determine the # of cpts
    int t_ExtrPointValence = Helper::get_vert_valence(a_Mesh, a_VertexHandle);

    // Get neighbor verts
    auto t_NBVerts = initNeighborVerts(a_VertexHandle, a_Mesh);

    // Convert neighbor verts to matrix type
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(a_Mesh, t_NBVerts);

    Matrix t_mask;
    switch(t_ExtrPointValence)
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

    int t_NumOfPatches = t_ExtrPointValence;
    if(t_ExtrPointValence >= 6)
    {
        t_NumOfPatches *= 4; // have four patches in each sector
    }


    return PatchBuilder(t_NBVerts, t_mask, this, t_NumOfPatches);
}

std::string ExtraordinaryPatchConstructor::getGroupName() const
{
    return "Group 1 ExtraordinaryPoint";
}