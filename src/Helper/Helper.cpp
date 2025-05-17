/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "Helper.hpp"
#include "HalfedgeOperation.hpp"
#include <iostream>
#include <cmath>
#include <unordered_set>

namespace Helper
{

// Vert functions

int get_vert_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return a_Mesh.valence(a_VertexHandle);
}

bool is_vert_3_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==3) ? true : false;
}

bool is_vert_4_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==4) ? true : false;
}

bool is_vert_5_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle)
{
    return (get_vert_valence(a_Mesh, a_VertexHandle)==5) ? true : false;
}

bool are_verts_of_face_all_4_valence(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    for(auto FVIt = a_Mesh.cfv_iter(a_FaceHandle); FVIt.is_valid(); FVIt++)
    {
        if(!is_vert_4_valence(a_Mesh, *FVIt))
        {
            return false;
        }
    }
    return true;
}

void set_vert_vector_to_default(const int a_Size, std::vector<VertexHandle>& a_VertexHandles)
{
    a_VertexHandles.clear();
    VertexHandle t_DefaultVertexHandle;
    for(int i=0; i<a_Size; i++)
    {
        a_VertexHandles.push_back(t_DefaultVertexHandle);
    }
}

// Return all the facehandles around the given vertexhandle
std::vector<FaceHandle> get_faces_around_vert_counterclock(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<FaceHandle> t_Faces;
    for(auto t_VFIt = a_Mesh.cvf_ccwiter(a_VertHandle); t_VFIt.is_valid(); ++t_VFIt)
    {
        t_Faces.push_back(*t_VFIt);
    }
    return t_Faces;
}

/*
 *  Get first and second layers of faces aroumnd the vert (unorder)
 *  ex:
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - v - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *      |   |   |   |   |
 *      o - o - o - o - o
 *
 */
std::vector<FaceHandle> get_two_layers_faces_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<FaceHandle> t_TwoLayerFaces;

    // Get first layer of faces around vertex
    // And get the neighbor faces of each first layer face (neighbors might be duplicated)
    for(auto VF_It = a_Mesh.cvf_iter(a_VertHandle); VF_It.is_valid(); ++VF_It)
    {
        auto t_FaceNBFaces = init_neighbor_faces(a_Mesh, *VF_It);
        t_TwoLayerFaces.insert(t_TwoLayerFaces.end(), t_FaceNBFaces.begin(), t_FaceNBFaces.end());
    }

    // Eliminate the duplicated faces
    sort(t_TwoLayerFaces.begin(), t_TwoLayerFaces.end());
    t_TwoLayerFaces.erase(unique(t_TwoLayerFaces.begin(), t_TwoLayerFaces.end()), t_TwoLayerFaces.end());

    return t_TwoLayerFaces;
}

/*
 * Return two layer of vertexhandle around a given vert
 * w/o ordering
 */
std::vector<VertexHandle> get_two_layers_verts_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    std::vector<VertexHandle> t_AllVerts;
    auto t_TwoLayersFaces = get_two_layers_faces_around_vert(a_Mesh, a_VertHandle);

    //Get vertices from each face
    for(auto t_Face : t_TwoLayersFaces)
    {
        auto t_VertsForAFace = get_verts_of_face(a_Mesh, t_Face);
        t_AllVerts.insert(t_AllVerts.end(), t_VertsForAFace.begin(), t_VertsForAFace.end());
    }

    // Eliminate the duplicated verts
    sort(t_AllVerts.begin(), t_AllVerts.end());
    t_AllVerts.erase(unique(t_AllVerts.begin(), t_AllVerts.end()), t_AllVerts.end());

    return t_AllVerts;
}




// Face functions

bool is_triangle(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForQuad = 3;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForQuad) ? true : false;
}

bool is_quad(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForQuad = 4;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForQuad) ? true : false;
}

bool is_pentagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForPentagon = 5;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForPentagon) ? true : false;
}

bool is_hexagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    const int t_NumOfVertsForHexagon = 6;
    return (get_num_of_verts_for_face(a_Mesh, a_FaceHandle)==t_NumOfVertsForHexagon) ? true : false;
}

std::vector<FaceHandle> init_neighbor_faces(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    std::vector<FaceHandle> t_NBFaceHandles;

    for (auto t_FVIt = a_Mesh.cfv_iter(a_FaceHandle); t_FVIt.is_valid(); ++t_FVIt)
    {
        for (auto t_VFIt = a_Mesh.cvf_iter(*t_FVIt); t_VFIt.is_valid(); ++t_VFIt)
        {
            t_NBFaceHandles.push_back(*t_VFIt);
        }
    }
    // Remove center face (Original)
    t_NBFaceHandles.erase(std::remove(t_NBFaceHandles.begin(),
                          t_NBFaceHandles.end(), a_FaceHandle), t_NBFaceHandles.end());

    // Remove duplicated neighbor faces
    sort(t_NBFaceHandles.begin(), t_NBFaceHandles.end());
         t_NBFaceHandles.erase(unique(t_NBFaceHandles.begin(),
                               t_NBFaceHandles.end()), t_NBFaceHandles.end());

    return t_NBFaceHandles;
}

bool has_7_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    const int t_7Neighbor = 7;
    return (get_num_of_neighbor_faces(a_NBFaceHandles)==t_7Neighbor) ? true : false;
}

bool has_9_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    const int t_9Neighbor = 9;
    return (get_num_of_neighbor_faces(a_NBFaceHandles)==t_9Neighbor) ? true : false;
}

bool are_faces_all_quads(const MeshType& a_Mesh, const std::vector<FaceHandle>& a_FaceHandles)
{
    for(auto t_FH : a_FaceHandles)
    {
        if(!is_quad(a_Mesh, t_FH))
        {
            return false;
        }
    }
    return true;
}

int get_num_of_verts_for_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    return a_Mesh.valence(a_FaceHandle);
}

int get_num_of_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles)
{
    return a_NBFaceHandles.size();
}

std::vector<VertexHandle> get_verts_of_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle)
{
    std::vector<VertexHandle> t_VertsOfFace;
    for (auto t_FVIt = a_Mesh.cfv_ccwiter(a_FaceHandle); t_FVIt.is_valid(); ++t_FVIt)
    {
        t_VertsOfFace.push_back(*t_FVIt);
    }
    return t_VertsOfFace;
}

int num_of_quads(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles)
{
    int t_NumOfQuads = 0;
    for(auto t_Face : a_FaceHandles)
    {
        if(is_quad(a_Mesh, t_Face))
        {
            t_NumOfQuads++;
        }
    }
    return t_NumOfQuads;
}

int num_of_triangles(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles)
{
    int t_NumOfTriangles = 0;
    for(auto t_Face : a_FaceHandles)
    {
        if(is_triangle(a_Mesh, t_Face))
        {
            t_NumOfTriangles++;
        }
    }
    return t_NumOfTriangles;
}




// Type conversion

Vec3d verthandles_to_point_vec(const MeshType& a_Mesh, const VertexHandle& a_VertHandle)
{
    Vec3d t_PointVec = {0, 0, 0};
    if(a_VertHandle.is_valid())
    {
        for(int i=0; i<3; i++)
        {
            t_PointVec[i] = a_Mesh.point(a_VertHandle)[i];
        }
    }
    return t_PointVec;
}

Matrix verthandles_to_points_mat(const MeshType& a_Mesh, const std::vector<VertexHandle>& a_VertHandle)
{
    int t_NumOfVerts = a_VertHandle.size();
    auto t_PointMat = Matrix(t_NumOfVerts, 3);

    for(int i=0; i<t_NumOfVerts; i++)
    {
        t_PointMat(i) = verthandles_to_point_vec(a_Mesh, a_VertHandle[i]);
    }
    return t_PointMat;
}


// Patch

Patch points_mat_to_patch(const Matrix& a_PointMat)
{
    int t_PointsPerCol = pow(a_PointMat.getRows(), 0.5);
    Patch t_Patch(t_PointsPerCol-1);
    for(int i=0; i<a_PointMat.getRows(); i++)
    {
        t_Patch.m_BBcoefs[i/t_PointsPerCol][i%t_PointsPerCol] = {a_PointMat(i,0),a_PointMat(i,1),a_PointMat(i,2)};
    }
    return t_Patch;
}

Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const Matrix& a_PointMat)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV, a_Group);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

Patch points_mat_to_patch(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat, const int a_StartIndex)
{
    Patch t_Patch(a_PatchDegU, a_PatchDegV, a_Group);
    for(int i=0; i<a_PatchDegU+1; i++)
    {
        for(int j=0; j<a_PatchDegV+1; j++)
        {
            int t_Index = a_StartIndex + i * (a_PatchDegV+1) + j;
            t_Patch.m_BBcoefs[i][j] = {a_PointMat(t_Index,0),a_PointMat(t_Index,1),a_PointMat(t_Index,2)};
        }
    }
    return t_Patch;
}

std::vector<Patch> points_mat_to_patches(const int a_NumOfPatch, const std::string a_Group, const Matrix& a_PointMat)
{
    std::vector<Patch> t_Patches;
    const int t_NumOfCCPtsPerPatch = a_PointMat.getRows() / a_NumOfPatch;
    const int t_Deg = sqrt(t_NumOfCCPtsPerPatch) - 1;
    for(int i=0; i<a_NumOfPatch; i++ )
    {
        t_Patches.push_back(points_mat_to_patch(t_Deg, t_Deg, a_Group, a_PointMat, i*t_NumOfCCPtsPerPatch));
    }

    return t_Patches;
}

/*
 * For the patch whose height and width are not the same
 */
std::vector<Patch> points_mat_to_patches(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const Matrix& a_PointMat)
{
    std::vector<Patch> t_Patches;
    const int t_NumOfCCPtsPerPatch = (a_PatchDegU + 1) * (a_PatchDegV + 1);
    const int t_NumOfPatch = a_PointMat.getRows() / t_NumOfCCPtsPerPatch;

    for(int i=0; i<t_NumOfPatch; i++ )
    {
        t_Patches.push_back(points_mat_to_patch(a_PatchDegU, a_PatchDegV, a_Group, a_PointMat, i*t_NumOfCCPtsPerPatch));
    }

    return t_Patches;
}



// Others

template <typename T> std::vector<T> duplicate_vector(int a_Times, const std::vector<T>& a_Vector)
{
    std::vector<T> t_DuplicatedVector;
    for(int i=0; i<a_Times; i++)
    {
        t_DuplicatedVector.insert(t_DuplicatedVector.end(), a_Vector.begin(), a_Vector.end());
    }
    return t_DuplicatedVector;
}
template std::vector<int> duplicate_vector(int a_Times, const std::vector<int>& a_Vector);
template std::vector<double> duplicate_vector(int a_Times, const std::vector<double>& a_Vector);


} // end of namespace Helper
