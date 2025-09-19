#pragma once
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <map>
#include <numeric>
#include "VertexMapping.hpp"

/**
 * \defgroup subdivision Subdivision
 * @brief Used for iterative refinement of regions that do not match any PnS patch type. 
 * It keeps track of the mapping between the original mesh vertices and the subdivided mesh vertices. 
 * A \ref VertexMapping object is stored for each vertex in the mesh. This is useful so that a mask relative to the original mesh can be generated.
 * 
 */

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

/**
 * \ingroup subdivision
 * @brief Subdivide the input mesh using Catmull-Clark subdivision scheme.
 * Used for iterative refinement of regions that do not match any PnS patch type.
 * Catmull-Clark is for PnS3.
 * The marking system is based on face. This keeps track of faces that are marked. See \ref Helper::is_face_marked.
 * If a face is marked, the subdivided faces that originate from it will also be marked.
 * 
 * @param a_Mesh The input mesh to be subdivided.
 * @return MeshType The subdivided mesh.
 */
MeshType subdividePnsControlMeshCatmullClark(MeshType& a_Mesh);

/**
 * \ingroup subdivision
 * @brief Subdivide the input mesh using Doo-Sabin subdivision scheme.
 * Used for iterative refinement of regions that do not match any PnS patch type.
 * Doo-Sabin is for PnS2.
 * The marking system is based on vertices. This keeps track of vertices that are marked. See \ref Helper::is_vert_marked.
 * If a vertex is marked, the subdivided vertices that form a ring around it will also be marked.
 * 
 * @param a_Mesh The input mesh to be subdivided.
 * @return MeshType The subdivided mesh.
 */
MeshType subdividePnsControlMeshDooSabin(MeshType& a_Mesh);