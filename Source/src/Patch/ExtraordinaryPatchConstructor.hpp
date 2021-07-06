/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#ifndef EXTRAORDINARYPATCHCONSTRUCTOR_HPP
#define EXTRAORDINARYPATCHCONSTRUCTOR_HPP

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"

typedef Eigen::Matrix<double, 48, 7> EGMat48x7d;
typedef Eigen::Matrix<double, 80, 11> EGMat80x11d;
typedef Eigen::Matrix<double, 384, 13> EGMat384x13d;
typedef Eigen::Matrix<double, 448, 15> EGMat448x15d;
typedef Eigen::Matrix<double, 512, 17> EGMat512x17d;

class ExtraordinaryPatchConstructor : public PatchConstructor
{
public:
    ExtraordinaryPatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh),
          m_MaskSct3(getMaskSct3()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle) override;
    std::vector<Patch> getPatch(const VertexHandle& a_VertexHandle) override;

private:
    const MeshType& m_Mesh;
    const EGMat48x7d m_MaskSct3;
    const EGMat80x11d m_MaskSct5;
    const EGMat384x13d m_MaskSct6;
    const EGMat448x15d m_MaskSct7;
    const EGMat512x17d m_MaskSct8;

    EGMat48x7d getMaskSct3();
    EGMat80x11d getMaskSct5();
    EGMat384x13d getMaskSct6();
    EGMat448x15d getMaskSct7();
    EGMat512x17d getMaskSct8();

    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle);
};

#endif //EXTRAORDINARYPATCHCONSTRUCTOR_HPP
