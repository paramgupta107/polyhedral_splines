/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat9x9d;
typedef Matrix Mat9x3d;

/**
 * \ingroup patch_build
 * @brief Patch constructor for patches consisting of two triangles and two quads. Designed for patch next to a polar patch. See \ref PatchConstructor
 * @warning Not in use anymore
 */
class TwoTrianglesTwoQuadsPatchConstructor : public PatchConstructor
{
public:
    TwoTrianglesTwoQuadsPatchConstructor() {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

    Patch getPatch(const std::vector<VertexHandle>& a_NBVertexHandles, MeshType& a_Mesh);
    Mat9x3d getPatchMat(const std::vector<VertexHandle>& a_NBVertexHandles, MeshType& a_Mesh);

private:
    const Mat9x9d m_Mask = Matrix({
        {0.25, 0.25, 0, 0.25, 0.25, 0, 0, 0, 0},
        {0, 0.5, 0, 0, 0.5, 0, 0, 0, 0},
        {0, 0.25, 0.25, 0, 0.25, 0.25, 0, 0, 0},
        {0, 0, 0, 0.5, 0.5, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0.5, 0.5, 0, 0, 0},
        {0, 0, 0, 0.25, 0.25, 0, 0.25, 0.25, 0},
        {0, 0, 0, 0, 0.5, 0, 0, 0.5, 0},
        {0, 0, 0, 0, 0.25, 0.25, 0, 0.25, 0.25}
    });

    /**
     * @brief Gather the list of neighboring vertices around a given vertex in the expected order to be compatable with mask.
     * ex:
     * \code
     *                 1 --- 2     0 3 6 use same vertex
     *               / |     |
     *              /  |     |
     *      (0,3,6) -- 4 --- 5
     *              \  |     |
     *               \ |     |
     *                 7 --- 8
     * \endcode
     * 
     * @param a_VertexHandle The vertex handle around for which to initialize neighbors.
     * @param a_Mesh The mesh to which the vertex belongs.
     * @return A vector of neighboring vertex handles.
     */
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh);

    std::string getGroupName() const;
};
