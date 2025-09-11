/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "PolarPatchConstructor.hpp"
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/ReadCSV2Matrix.hpp"

/*
 * Get the mask for generating Bi3 Patch
 */
Mat36x4d PolarPatchConstructor::getMaskSct3()
{
    std::string t_MaskCSVFilePathSct3 = "polarSct3.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct3, 36, 4); //  NumOfTotalCpts = 60, NumOfVerts = 4
}

Mat48x5d PolarPatchConstructor::getMaskSct4()
{
    std::string t_MaskCSVFilePathSct4 = "polarSct4.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct4, 48, 5);
}

Mat60x6d PolarPatchConstructor::getMaskSct5()
{
    std::string t_MaskCSVFilePathSct5 = "polarSct5.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct5, 60, 6);
}

Mat72x7d PolarPatchConstructor::getMaskSct6()
{
    std::string t_MaskCSVFilePathSct6 = "polarSct6.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct6, 72, 7);
}

Mat84x8d PolarPatchConstructor::getMaskSct7()
{
    std::string t_MaskCSVFilePathSct7 = "polarSct7.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct7, 84, 8);
}

Mat96x9d PolarPatchConstructor::getMaskSct8()
{
    std::string t_MaskCSVFilePathSct8 = "polarSct8.csv";
    return read_csv_as_matrix(t_MaskCSVFilePathSct8, 96, 9);
}


bool PolarPatchConstructor::isSamePatchType(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked)
{
    if(check_marked && Helper::is_marked(a_Mesh, a_VertexHandle))
    {
        return false;
    }
    int t_Valence = Helper::get_vert_valence(a_Mesh, a_VertexHandle);
    if(!Helper::is_polar(a_Mesh, a_VertexHandle)){
        return false;
    }

    std::cout << "Polar" << std::endl;

    m_NumOfSct = t_Valence;

    return true;
}


PatchBuilder PolarPatchConstructor::getPatchBuilder(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool mark_gathered)
{
    auto a_NBVertexHandles = initNeighborVerts(a_VertexHandle, a_Mesh);

    // Get mask
    Matrix t_mask;
    switch (m_NumOfSct)
    {
        case 3:
            t_mask = m_MaskSct3;
            break;
        case 4:
            t_mask = m_MaskSct4;
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

    const int t_DegU = 3;
    const int t_DegV = 2;

    if(mark_gathered)
    {
        Helper::mark_vert(a_Mesh, a_VertexHandle);
    }
    
    return PatchBuilder(a_Mesh, a_NBVertexHandles, t_mask, this, t_DegU, t_DegV);
}


/*
 *     P1 ---- P4
 *      | \  / |
 *      |  P0  |
 *      | /  \ |
 *     P2 ---- P3
 */
std::vector<VertexHandle> PolarPatchConstructor::initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh)
{
    // init vector to stroe all vertices
    std::vector<VertexHandle> t_NBVertexHandles;

    // Get first layer vetices  ex: for 4sct, P1 -> P2 -> P3 -> P4
    for(auto VHIt=a_Mesh.cvoh_ccwiter(a_VertexHandle); VHIt.is_valid(); ++VHIt)
    {
        t_NBVertexHandles.push_back(HalfedgeOperation::get_vert_fixed_halfedge(a_Mesh, *VHIt, {1,4}));
    }

    // Insert the central point P0
    t_NBVertexHandles.insert(t_NBVertexHandles.begin(), a_VertexHandle); ;

    return t_NBVertexHandles;
}

std::string PolarPatchConstructor::getGroupName() const
{
    return "Group 5 Polar";
}
