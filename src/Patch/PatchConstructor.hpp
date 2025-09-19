/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchBuilder.hpp"

/**
 * \defgroup patch_build Patch Building
 * \brief From a control mesh to a PnS surface represented as Bézier patches.
 *
 * \details
 * \ref process_mesh is the end-to-end entry point: given a mesh, it produces a PnS surface
 * as a set of Bézier \ref Patch "patches" written to a file. It follows the following pipeline:
 *
 * \section patch_build_pipeline Pipeline
 * 1. Traverse all vertices and faces of the input mesh.
 * 2. For each vertex/face we find a matching \ref PatchConstructor from \ref PatchConstructorPool (if any) using the \ref PatchConstructorPool::getPatchConstructor fucntion.
 *    Each PnS patch type is represnted as a subclass of \ref PatchConstructor.
 * 3. We identify if a certain vertex/face matches a PnS patch time using the PatchConstructor::isSamePatchType function.
 * 4. Once we identify a PnS patch type, instead of building a \ref Patch immedietly, we get \ref PatchBuilder using the PatchConstructor::getPatchBuilder function.
 * 5. \ref PatchBuilder::buildPatches function creates the Bézier patches ( \ref Patch) for the PnS patch type.
 * 6. For each \ref PatchBuilder, the \ref Patch "Patches" are built and written to file using a \ref PatchConsumer.
 * 7. If there are regions of the mesh that do not match any PnS patch type, iterative \ref subdivision is used to refine the mesh and the process is repeated the process of identifying PnS patches. See \ref subdivision.
 *    Vertices/faces are marked to prevent overlapping patches. See \ref Helper::is_marked.
 *
 * \section patch_build_why_builder Why PatchBuilder?
 * Returning a builder (rather than a \ref Patch) decouples *discovery* from *building*.
 * If the underlying mesh changes, patches can be rebuilt efficiently from the saved builders
 * without re-traversing the mesh or re-identifying patch types.
 *
 * \section patch_build_entries Functions
 * - \ref getPatchBuilders — discovery only. Traverses the mesh, identifies PnS patches via
 *   the pool, and returns the set of \ref PatchBuilder "builders" for later use (e.g., incremental updates).
 * - \ref process_mesh — full pipeline. Calls \ref getPatchBuilders, then invokes
 *   \ref PatchBuilder::buildPatches on each builder and forwards the results to a \ref PatchConsumer.
 *
 * \section patch_build_types Classes
 * - \ref PatchConstructor — abstract base class; each subclass corresponds to a specific PnS
 *   patch type. Responsibilities:
 *     - decide whether a site matches the patch type; and
 *     - gather the local neighborhood needed to build the patch.
 * - \ref PatchBuilder — stores the data required to construct the resulting \ref Patch "Patches"
 *   via \ref PatchBuilder::buildPatches.
 * - \ref PatchConstructorPool — registry that holds all available constructors and
 *   finds a match for a vertex/face.
 * - \ref PatchConsumer — A consumer receives built patches and writes them to a file or other output.
 *
 * \note A vertex/face will be used to create at most one \ref PatchBuilder. However, not all vertices 
 *
 */


typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;
typedef MeshType::FaceHandle FaceHandle;

/**
 * @brief Abstract base class for patch constructors.
 * 
 * Patch constructors are responsible for identifying PnS patch types and creating \ref PatchBuilder "PatchBuilders".
 * PatchBuilder is returned instead of a Patch directly so that updated patches can be easily if the underlying mesh changes without needing re-traverse the entire mesh to dentify the patch type and gather the vertices again.
 * Essentially, this class indentifies PnS patch types and collects information required to build associated Bézier patches.
 */
class PatchConstructor
{
public:
    /**
     * @brief Given a vertex, checks wither the vertex and the neighborhood arund the vertex of this patch type.
     * 
     * @param a_Mesh The mesh to which the vertex belongs.
     * @param check_marked If true, the function will return false if the vertex is marked. See \ref Helper::is_marked.
     * @return true if the vertex and its neighborhood match this patch type.
     */
    virtual bool isSamePatchType(const VertexHandle&, MeshType& a_Mesh, bool check_marked = false) { return false; };

    /**
     * @brief Given a face, checks wither the face and the neighborhood arund the face of this patch type.
     * 
     * @param a_Mesh The mesh to which the face belongs.
     * @param check_marked If true, the function will return false if any vertex of the face is marked. See \ref Helper::is_marked.
     * @return true if the face and its neighborhood match this patch type.
     */
    virtual bool isSamePatchType(const FaceHandle&, MeshType& a_Mesh, bool check_marked = false) { return false; };

    /**
     * @brief Constructs a PatchBuilder for the patch type at the given vertex.
     * 
     * @param a_Mesh The mesh to which the vertex belongs.
     * @param mark_gathered If true, all vertices gathered for the patch will be marked. See \ref Helper::mark_vert.
     * @return A PatchBuilder configured for the patch type at the given vertex.
     * @throws std::runtime_error if not implemented in derived class.
     */
    virtual PatchBuilder getPatchBuilder(const VertexHandle&, MeshType& a_Mesh, bool mark_gathered = false){throw std::runtime_error("getPatchBuilder for VertexHandle not implemented");};

    /**
     * @brief Constructs a PatchBuilder for the patch type at the given face.
     * 
     * @param a_Mesh The mesh to which the face belongs.
     * @param mark_gathered If true, all vertices gathered for the patch will be marked. See \ref Helper::mark_face_verts.
     * @return A PatchBuilder configured for the patch type at the given face.
     * @throws std::runtime_error if not implemented in derived class.
     */
    virtual PatchBuilder getPatchBuilder(const FaceHandle&, MeshType& a_Mesh, bool mark_gathered = false){throw std::runtime_error("getPatchBuilder for FaceHandle not implemented");};

    /**
     * @brief Returns the name of the patch group this constructor handles.
     * 
     * This method must be implemented in derived classes to return the specific patch group name.
     * 
     * @return The name of the patch group.
     */
    virtual std::string getGroupName() const = 0;
private:
    
};
