/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

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
    const Mat36x4d m_MaskSct3;
    const Mat48x5d m_MaskSct4;
    const Mat60x6d m_MaskSct5;
    const Mat72x7d m_MaskSct6;
    const Mat84x8d m_MaskSct7;
    const Mat96x9d m_MaskSct8;
    int m_NumOfSct = 0;

    Mat36x4d getMaskSct3();
    Mat48x5d getMaskSct4();
    Mat60x6d getMaskSct5();
    Mat72x7d getMaskSct6();
    Mat84x8d getMaskSct7();
    Mat96x9d getMaskSct8();
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle, MeshType& a_Mesh);
    std::string getGroupName() const;
};
