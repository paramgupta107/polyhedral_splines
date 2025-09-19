/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat48x12d;
typedef Matrix Mat80x20d;
typedef Matrix Mat384x24d;
typedef Matrix Mat448x28d;
typedef Matrix Mat512x32d;

/**
 * \ingroup patch_build
 * @brief Patch constructor for n-gon patches. See \ref PatchConstructor
 */
class NGonPatchConstructor : public PatchConstructor
{
public:
    NGonPatchConstructor()
        : m_MaskSct3(getMaskSct3()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

private:
    /**
     * @brief The mask for the n-gon patch with 3 sides
     */
    const Mat48x12d m_MaskSct3;
    /**
     * @brief The mask for the n-gon patch with 5 sides
     */
    const Mat80x20d m_MaskSct5;
    /**
     * @brief The mask for the n-gon patch with 6 sides
     */
    const Mat384x24d m_MaskSct6;
    /**
     * @brief The mask for the n-gon patch with 7 sides
     */
    const Mat448x28d m_MaskSct7;
    /**
     * @brief The mask for the n-gon patch with 8 sides
     */
    const Mat512x32d m_MaskSct8;
    int m_FaceValence;

    /**
     * @brief Retrieves the mask for the n-gon patch with 3 sides.
     * 
     * @return The mask as a Mat48x12d object.
     */
    Mat48x12d getMaskSct3();
    /**
     * @brief Retrieves the mask for the n-gon patch with 5 sides.
     * 
     * @return The mask as a Mat80x20d object.
     */
    Mat80x20d getMaskSct5();
    /**
     * @brief Retrieves the mask for the n-gon patch with 6 sides.
     * 
     * @return The mask as a Mat384x24d object.
     */
    Mat384x24d getMaskSct6();
    /**
     * @brief Retrieves the mask for the n-gon patch with 7 sides.
     * 
     * @return The mask as a Mat448x28d object.
     */
    Mat448x28d getMaskSct7();
    /**
     * @brief Retrieves the mask for the n-gon patch with 8 sides.
     * 
     * @return The mask as a Mat512x32d object.
     */
    Mat512x32d getMaskSct8();

    /**
     * @brief Gather the list of neighboring vertices around a given face in the expected order to be compatable with mask.
     * ex for 3 sides:
     * \code
     *           8
     *        /    \
     *      9 - 11- 10
     *     /   / \   \
     *    2 - 3 - 7 - 5
     *    |   |   |   |
     *    0 - 1 - 6 - 4
     * \endcode
     * 
     * @param a_FaceHandle The face handle around for which to initialize neighbors.
     * @param a_Mesh The mesh to which the face belongs.
     * @return A vector of neighboring vertex handles.
     */
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle, MeshType& a_Mesh);
    std::string getGroupName() const;
};
