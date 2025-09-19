/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <vector>
#include "../Helper/Helper.hpp"
#include "../Subdivision/subdivision.hpp"
#include "../Subdivision/VertexMapping.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;

class PatchConstructor; // forward declaration

/**
 * \ingroup patch_build
 * @brief A builder class for constructing Bézier patches from a mesh.
 * 
 * The PatchBuilder class created by a \ref PatchConstructor contains all the information needed to build a \ref Patch.
 * It holds the neighboring vertex handles, the mask for creating the patch. The mask is always with respect to the control points of the original mesh. 
 * The stored mask is generated in case of patch builders being collected from mesh after \ref subdivision. This way the mask is always with respect to the control points of the original mesh.
 * It can be given a mesh with updated vertices to build updated bezier patches for this particular patch type and neighborhood.
 * By keeping track of the control points that affect this PnS patch, upon updates, only the necessary patch builders need to be re-evaluated.
 * @note The connectivty of the mesh must not change, only the vertex positions.
 */
class PatchBuilder
{
    public:
        /**
         * @brief The neighboring vertex handles used to construct the patch.
         * 
         */
        std::vector<VertexHandle> m_NBVertexHandles;
        /**
         * @brief The mask used to construct the patch.
         * 
         */
        Matrix m_Mask;
        /**
         * @brief The patch constructor that created this patch builder.
         * 
         */
        PatchConstructor *m_PatchConstructor;

        /**
         * @brief The number of bézier patches this PnS patch type outputs.
         * 
         */
        int m_NumOfPatches = -1;
        /**
         * @brief The degree in u direction of each bézier patch this PnS patch type outputs.
         * 
         */
        int m_DegU = -1;
        /**
         * @brief The degree in v direction of each bézier patch this PnS patch type outputs.
         * 
         */
        int m_DegV = -1;

        /**
         * @brief Constructs a PatchBuilder with the given neighboring vertex handles, mask, patch constructor and number of output bezier patches.
         * 
         * @param a_Mesh The mesh to which the vertex handles belong. Used to validate the vertex handles.
         * @param a_NBVertexHandles The neighboring vertex handles used to construct the patch.
         * @param a_Mask The mask used to construct the patch.
         * @param a_PatchConstructor The patch constructor that created this patch builder.
         * @param a_NumOfPatches The number of bézier patches this PnS patch type outputs.
         */
        PatchBuilder(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_NumOfPatches);
        
        /**
         * @brief Constructs a PatchBuilder with the given neighboring vertex handles, mask, patch constructor, and degrees.
         * 
         * @param a_Mesh The mesh to which the vertex handles belong. Used to validate the vertex handles.
         * @param a_NBVertexHandles The neighboring vertex handles used to construct the patch.
         * @param a_Mask The mask used to construct the patch.
         * @param a_PatchConstructor The patch constructor that created this patch builder.
         * @param a_DegU The degree in u direction of each bézier patch this PnS patch type outputs.
         * @param a_DegV The degree in v direction of each bézier patch this PnS patch type outputs.
         */
        PatchBuilder(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_DegU, int a_DegV);
        
        /**
         * @brief Construct a new Patch Builder object
         * 
         * @param a_PatchBuilder 
         */
        PatchBuilder(const PatchBuilder& a_PatchBuilder)
            : m_NBVertexHandles(a_PatchBuilder.m_NBVertexHandles), m_Mask(a_PatchBuilder.m_Mask), m_PatchConstructor(a_PatchBuilder.m_PatchConstructor), m_NumOfPatches(a_PatchBuilder.m_NumOfPatches), m_DegU(a_PatchBuilder.m_DegU), m_DegV(a_PatchBuilder.m_DegV) {};
        /**
         * @brief Assignment operator
         * 
         */
        PatchBuilder& operator=(const PatchBuilder& a_PatchBuilder);
        /**
         * @brief Builds the Bézier patches using the current vertex positions in the given mesh.
         * 
         * @param a_Mesh The mesh containing the current vertex positions.
         * @return A vector of constructed Bézier patches.
         * @note The connectivity of the mesh must not change, only the vertex positions.
         */
        std::vector<Patch> buildPatches(const MeshType& a_Mesh) const;
        
        /**
         * @brief Get the Neighbor Verts object that are used to construct the bezier patches for this PnS patch.
         * 
         * @return std::vector<VertexHandle> 
         */
        std::vector<VertexHandle> getNeighborVerts() const;

        /**
         * @brief Get the Mask object. The linear transformation that maps the neighboring vertices to the coefficients of the bezier patches.
         * 
         * @return Matrix 
         */
        Matrix getMask() const;

        /**
         * @brief Get the PatchConstructor object that created this PatchBuilder.
         * 
         * @return const PatchConstructor* 
         */
        const PatchConstructor* getPatchConstructor() const;

        /**
         * @brief Raises the degree of the bezier patches by upto degree 3 in each direction. Nothing happens if the degree is already 3 or higher in a direction.
         * The mask is modified to produce patches with a higher degree
         */
        void degRaise();

        /**
         * @brief Get the Num Patches object. The number of bezier patches this PnS patch type outputs.
         * 
         * @return int 
         */
        int numPatches() const;
    private:
        void initializeMaskAndNeighborVertices(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask);
};