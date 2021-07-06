/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#ifndef HELPER_HPP
#define HELPER_HPP

#include <Eigen/Dense>
#include "../Patch/Patch.hpp"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;
typedef MeshType::EdgeHandle EdgeHandle;
typedef MeshType::FaceHandle FaceHandle;
typedef MeshType::HalfedgeHandle HalfedgeHandle;
typedef MeshType::Point Point;
typedef Eigen::Matrix<double, 1, 3> EGPoint;
typedef Eigen::Matrix<double, Eigen::Dynamic, 3> EGPointNd;



namespace Helper
{

// Vert functions
int get_vert_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle);
bool is_vert_3_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle);
bool is_vert_4_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle);
bool is_vert_5_valence(const MeshType& a_Mesh, const VertexHandle& a_VertexHandle);
bool are_verts_of_face_all_4_valence(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
void set_vert_vector_to_default(const int a_Size, std::vector<VertexHandle>& a_VertexHandles);
std::vector<FaceHandle> get_faces_around_vert_counterclock(const MeshType& a_Mesh, const VertexHandle& a_VertHandle);
std::vector<FaceHandle> get_two_layers_faces_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle);
std::vector<VertexHandle> get_two_layers_verts_around_vert(const MeshType& a_Mesh, const VertexHandle& a_VertHandle);

// Face functions
bool is_triangle(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
bool is_quad(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
bool is_pentagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
bool is_hexagon(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
std::vector<FaceHandle> init_neighbor_faces(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
bool has_7_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles);
bool has_9_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles);
bool are_faces_all_quads(const MeshType& a_Mesh, const std::vector<FaceHandle>& a_FaceHandles);
int get_num_of_verts_for_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
int get_num_of_neighbor_faces(const std::vector<FaceHandle>& a_NBFaceHandles);
std::vector<VertexHandle> get_verts_of_face(const MeshType& a_Mesh, const FaceHandle& a_FaceHandle);
int num_of_quads(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles);
int num_of_triangles(const MeshType& a_Mesh, std::vector<FaceHandle> a_FaceHandles);

// Type conversion
EGPoint verthandle_to_EGPoint(const MeshType& a_Mesh, const VertexHandle& a_VertHandle);
EGPointNd verthandles_to_EGPoints(const MeshType& a_Mesh, const std::vector<VertexHandle>& a_VertHandle);

// Patch
Patch EGPoints_to_patch(const EGPointNd& a_EGPoints);
Patch EGPoints_to_patch(const int a_PatchDegU, const int a_PatchDegV, const EGPointNd& a_EGPoints);
Patch EGPoints_to_patch(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const EGPointNd& a_EGPoints);
std::vector<Patch> EGPoints_to_patches(const int a_NumOfPatch, const std::string a_Group, const EGPointNd& a_EGPoints);
std::vector<Patch> EGPoints_to_patches(const int a_PatchDegU, const int a_PatchDegV, const std::string a_Group, const EGPointNd& a_EGPoints);

// Others
template <typename T> std::vector<T> duplicate_vector(int a_Times, const std::vector<T>& a_Vector);

} // end of Helper namespace

#endif //HELPER_HPP
