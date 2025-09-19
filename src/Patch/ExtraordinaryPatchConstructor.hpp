/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat48x7d;
typedef Matrix Mat80x11d;
typedef Matrix Mat384x13d;
typedef Matrix Mat448x15d;
typedef Matrix Mat512x17d;

/**
 * \ingroup patch_build
 * @brief Patch constructor for extraordinary patches. See \ref PatchConstructor
 */
class ExtraordinaryPatchConstructor : public PatchConstructor
{
public:
    ExtraordinaryPatchConstructor()
        : m_MaskSct3(getMaskSct3()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool mark_gathered = false) override;
    std::string getGroupName() const;

private:
    /**
     * @brief The mask for the extraordinary patch with valence = 3.
     */
    const Mat48x7d m_MaskSct3;
    /**
     * @brief The mask for the extraordinary patch with valence = 5.
     */
    const Mat80x11d m_MaskSct5;
    /**
     * @brief The mask for the extraordinary patch with valence = 6.
     */
    const Mat384x13d m_MaskSct6;
    /**
     * @brief The mask for the extraordinary patch with valence = 7.
     */
    const Mat448x15d m_MaskSct7;
    /**
     * @brief The mask for the extraordinary patch with valence = 8.
     */
    const Mat512x17d m_MaskSct8;

    /**
     * @brief Retrieves the mask for the extraordinary patch with valence = 3.
     * 
     * @return The mask as a Mat48x7d object.
     */
    Mat48x7d getMaskSct3();
    /**
     * @brief Retrieves the mask for the extraordinary patch with valence = 5.
     * 
     * @return The mask as a Mat80x11d object.
     */
    Mat80x11d getMaskSct5();
    /**
     * @brief Retrieves the mask for the extraordinary patch with valence = 6.
     * 
     * @return The mask as a Mat384x13d object.
     */
    Mat384x13d getMaskSct6();
    /**
     * @brief Retrieves the mask for the extraordinary patch with valence = 7.
     * 
     * @return The mask as a Mat448x15d object.
     */
    Mat448x15d getMaskSct7();
    /**
     * @brief Retrieves the mask for the extraordinary patch with valence = 8.
     * 
     * @return The mask as a Mat512x17d object.
     */
    Mat512x17d getMaskSct8();

    /**
     * @brief Gather the list of neighboring vertices around a given vertex in the expected order to be compatable with mask.
     * ex for valence = 3:
     * \code
     *        0
     *      / | \
     *     /  |  \
     *    1   6   5
     *    \  / \ /
     *     2    4
     *      \  /
     *       3
     * \endcode
     * 
     * @param a_VertexHandle The vertex handle around for which to initialize neighbors.
     * @param a_Mesh The mesh to which the vertex belongs.
     * @return A vector of neighboring vertex handles.
     */
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh);
};
