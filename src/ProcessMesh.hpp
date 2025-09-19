/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "Helper/Helper.hpp"
#include "Subdivision/subdivision.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

std::vector<PatchBuilder> getPatchBuilders(MeshType& a_Mesh);

/**
 * \ingroup patch_build
 * @brief Given an OpenMesh PolyMesh, process the mesh to generate the PnS surface in the form of Bézier patches.
 * 
 * The bezier patches are written to file using the provided \ref PatchConsumer.
 * 
 * The function traverses the mesh to identify PnS patches, build the Bézier patches, and write them to file.
 * 
 * @param a_Mesh The mesh to be processed.
 * @param a_Consumer The PatchConsumer that will receive the extracted patches.
 * @param a_IsDegRaise If true, raises the degree in direction upto 3 for each patch. If the degree is already 3 or higher in a direction, nothing happens in that direction.
 */
void process_mesh(MeshType& a_Mesh, PatchConsumer* a_Consumer, const bool a_IsDegRaise);

/**
 * \ingroup patch_build
 * @brief Given an OpenMesh PolyMesh, retrieve all PatchBuilders for the PnS patches in the mesh.
 * 
 * The function traverses the mesh to identify PnS patches and build their corresponding \ref PatchBuilder "PatchBuilders".
 * 
 * @param a_Mesh The mesh to be processed.
 * @return A vector of PatchBuilders for all identified PnS patches in the mesh.
 */
std::vector<PatchBuilder> getPatchBuilders(MeshType& a_Mesh);