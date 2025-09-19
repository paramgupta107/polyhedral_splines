/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat256x20d;

/**
 * \ingroup patch_build
 * @brief Patch constructor for T2 patches. See \ref PatchConstructor
 */
class T2PatchConstructor : public PatchConstructor
{
public:
    T2PatchConstructor()
        : m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

private:
    /**
     * @brief The mask for the T2 patch.
     */
    const Mat256x20d m_Mask;

    /**
     * @brief Retrieves the mask for the T2 patch.
     * 
     * @return The mask as a Mat256x20d object.
     */
    Mat256x20d getMask();
    
    /**
     * @brief Gather the list of neighboring vertices around a given face in the expected order to be compatable with mask.
     * ex:
     * \code
     *    0 - 1 -------- 2 - 3
     *    |   |          |   |
     *    4 - 5 -------- 6 - 7
     *    |   |          |   |
     *    8 - 9          |   |
     *    |   |          |   |
     *   10 - 11 - 12 - 13 - 14
     *    |   |    |     |   |
     *   15 - 16 - 17 - 18 - 19
     * \endcode
     * 
     * @param a_FaceHandle The face handle around for which to initialize neighbors.
     * @param a_Mesh The mesh to which the face belongs.
     * @return A vector of neighboring vertex handles.
     */
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, MeshType& a_Mesh);

    std::string getGroupName() const;
};
