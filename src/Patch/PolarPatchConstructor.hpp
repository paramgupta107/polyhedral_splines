#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"
#include "../Helper/Helper.hpp"

typedef Matrix Mat36x4d;
typedef Matrix Mat48x5d;
typedef Matrix Mat60x6d;
typedef Matrix Mat72x7d;
typedef Matrix Mat84x8d;
typedef Matrix Mat96x9d;

/**
 * \ingroup patch_build
 * @brief Patch constructor for polar patches. See \ref PatchConstructor
 */
class PolarPatchConstructor : public PatchConstructor
{
public:
    PolarPatchConstructor()
        : m_MaskSct3(getMaskSct3()),
          m_MaskSct4(getMaskSct4()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked = false) override;
    PatchBuilder getPatchBuilder(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool mark_gathered = false) override;

private:
    /**
     * @brief The mask for the polar patch with valence = 3.
     */
    const Mat36x4d m_MaskSct3;

    /**
     * @brief The mask for the polar patch with valence = 4.
     */
    const Mat48x5d m_MaskSct4;

    /**
     * @brief The mask for the polar patch with valence = 5.
     */
    const Mat60x6d m_MaskSct5;

    /**
     * @brief The mask for the polar patch with valence = 6.
     */
    const Mat72x7d m_MaskSct6;

    /**
     * @brief The mask for the polar patch with valence = 7.
     */
    const Mat84x8d m_MaskSct7;

    /**
     * @brief The mask for the polar patch with valence = 8.
     */
    const Mat96x9d m_MaskSct8;

    /**
     * @brief The number of sections in the polar patch.
     */
    int m_NumOfSct = 0;

    /**
     * @brief Retrieves the mask for the polar patch with valence = 3.
     * 
     * @return The mask as a Mat36x4d object.
     */
    Mat36x4d getMaskSct3();

    /**
     * @brief Retrieves the mask for the polar patch with valence = 4.
     * 
     * @return The mask as a Mat48x5d object.
     */
    Mat48x5d getMaskSct4();

    /**
     * @brief Retrieves the mask for the polar patch with valence = 5.
     * 
     * @return The mask as a Mat60x6d object.
     */
    Mat60x6d getMaskSct5();

    /**
     * @brief Retrieves the mask for the polar patch with valence = 6.
     * 
     * @return The mask as a Mat72x7d object.
     */
    Mat72x7d getMaskSct6();

    /**
     * @brief Retrieves the mask for the polar patch with valence = 7.
     * 
     * @return The mask as a Mat84x8d object.
     */
    Mat84x8d getMaskSct7();

    /**
     * @brief Retrieves the mask for the polar patch with valence = 8.
     * 
     * @return The mask as a Mat96x9d object.
     */
    Mat96x9d getMaskSct8();

    /**
     * @brief Gather the list of neighboring vertices around a given vertex in the expected order to be compatable with mask.
     * ex:
     * \code
     *     P1 ---- P4
     *      | \  / |
     *      |  P0  |
     *      | /  \ |
     *     P2 ---- P3
     * \endcode
     * 
     * @param a_VertexHandle The vertex handle around for which to initialize neighbors.
     * @param a_Mesh The mesh to which the vertex belongs.
     * @return A vector of neighboring vertex handles.
     */
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh);
    std::string getGroupName() const;
};
