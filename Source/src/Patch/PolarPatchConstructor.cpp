#include "PolarPatchConstructor.hpp"
#include <Eigen/Dense>
#include "../Helper/HalfedgeOperation.hpp"
#include "../Helper/Helper.hpp"
#include "../Helper/ReadCSV2EigenMatrix.hpp"

typedef Eigen::Matrix<double, Eigen::Dynamic, 3> EGPointNd;

/*
 * Get the mask for generating Bi3 Patch
 */
EGMat36x4d PolarPatchConstructor::getMaskSct3()
{
    std::string t_MaskCSVFilePathSct3 = "../Source/src/Patch/Table/polarSct3.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct3, 36, 4); //  NumOfTotalCpts = 60, NumOfVerts = 4
}

EGMat48x5d PolarPatchConstructor::getMaskSct4()
{
    std::string t_MaskCSVFilePathSct4 = "../Source/src/Patch/Table/polarSct4.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct4, 48, 5);
}

EGMat60x6d PolarPatchConstructor::getMaskSct5()
{
    std::string t_MaskCSVFilePathSct5 = "../Source/src/Patch/Table/polarSct5.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct5, 60, 6);
}

EGMat72x7d PolarPatchConstructor::getMaskSct6()
{
    std::string t_MaskCSVFilePathSct6 = "../Source/src/Patch/Table/polarSct6.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct6, 72, 7);
}

EGMat84x8d PolarPatchConstructor::getMaskSct7()
{
    std::string t_MaskCSVFilePathSct7 = "../Source/src/Patch/Table/polarSct7.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct7, 84, 8);
}

EGMat96x9d PolarPatchConstructor::getMaskSct8()
{
    std::string t_MaskCSVFilePathSct8 = "../Source/src/Patch/Table/polarSct8.csv";
    return read_csv_as_eigen_matrix(t_MaskCSVFilePathSct8, 96, 9);
}


bool PolarPatchConstructor::isSamePatchType(const VertexHandle& a_VertexHandle)
{
    // Polar point should be 3 to 8 valence
    int t_Valence = Helper::get_vert_valence(m_Mesh, a_VertexHandle);
    const int t_Lowerbound = 3;
    const int t_Upperbound = 8;
    if(t_Valence < t_Lowerbound || t_Valence > t_Upperbound)
    {
        return false;
    }

    // Polar point should not be on the boundary
    if(m_Mesh.is_boundary(a_VertexHandle))
    {
        return false;
    }

    // The first layer of surrounded faces should all be triangles
    auto t_NBFaces = Helper::get_faces_around_vert_counterclock(m_Mesh, a_VertexHandle);
    for(auto t_Face : t_NBFaces)
    {
        if(!Helper::is_triangle(m_Mesh, t_Face))
        {
            return false;
        }
    }

    std::cout << "Polar" << std::endl;

    m_NumOfSct = t_Valence;

    return true;
}


std::vector<Patch> PolarPatchConstructor::getPatch(const VertexHandle& a_VertexHandle)
{
    auto a_NBVertexHandles = initNeighborVerts(a_VertexHandle);

    // Convert NeighborVerts to Eigen matrix type
    auto t_EGNBVerts = Helper::verthandles_to_EGPoints(m_Mesh, a_NBVertexHandles);

    // Get mask
    EGPointNd t_BBcoefs;
    switch (m_NumOfSct)
    {
        case 3:
            t_BBcoefs = m_MaskSct3 * t_EGNBVerts;
            break;
        case 4:
            t_BBcoefs = m_MaskSct4 * t_EGNBVerts;
            break;
        case 5:
            t_BBcoefs = m_MaskSct5 * t_EGNBVerts;
            break;
        case 6:
            t_BBcoefs = m_MaskSct6 * t_EGNBVerts;
            break;
        case 7:
            t_BBcoefs = m_MaskSct7 * t_EGNBVerts;
            break;
        case 8:
            t_BBcoefs = m_MaskSct8 * t_EGNBVerts;
            break;
    }


    const int t_DegU = 3;
    const int t_DegV = 2;
    auto t_Patches = Helper::EGPoints_to_patches(t_DegU, t_DegV, "Group 5 Polar", t_BBcoefs);

    return t_Patches;
}


/*
 *     P1 ---- P4
 *      | \  / |
 *      |  P0  |
 *      | /  \ |
 *     P2 ---- P3
 */
std::vector<VertexHandle> PolarPatchConstructor::initNeighborVerts(const VertexHandle& a_VertexHandle)
{
    // init vector to stroe all vertices
    std::vector<VertexHandle> t_NBVertexHandles;

    // Get first layer vetices  ex: for 4sct, P1 -> P2 -> P3 -> P4
    for(auto VHIt=m_Mesh.cvoh_ccwiter(a_VertexHandle); VHIt.is_valid(); ++VHIt)
    {
        t_NBVertexHandles.push_back(HalfedgeOperation::get_vert_fixed_halfedge(m_Mesh, *VHIt, {1,4}));
    }

    // Insert the central point P0
    t_NBVertexHandles.insert(t_NBVertexHandles.begin(), a_VertexHandle); ;

    return t_NBVertexHandles;
}
