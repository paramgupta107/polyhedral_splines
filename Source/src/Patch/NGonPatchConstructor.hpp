#ifndef NGONPATCHCONSTRUCTOR_HPP
#define NGONPATCHCONSTRUCTOR_HPP

#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"

typedef Eigen::Matrix<double, 48, 12> EGMat48x12d;
typedef Eigen::Matrix<double, 80, 20> EGMat80x20d;
typedef Eigen::Matrix<double, 384, 24> EGMat384x24d;
typedef Eigen::Matrix<double, 448, 28> EGMat448x28d;
typedef Eigen::Matrix<double, 512, 32> EGMat512x32d;

class NGonPatchConstructor : public PatchConstructor
{
public:
    NGonPatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh),
          m_MaskSct3(getMaskSct3()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle) override;
    std::vector<Patch> getPatch(const FaceHandle& a_FaceHandle) override;

private:
    const MeshType& m_Mesh;
    const EGMat48x12d m_MaskSct3;
    const EGMat80x20d m_MaskSct5;
    const EGMat384x24d m_MaskSct6;
    const EGMat448x28d m_MaskSct7;
    const EGMat512x32d m_MaskSct8;
    int m_FaceValence;

    EGMat48x12d getMaskSct3();
    EGMat80x20d getMaskSct5();
    EGMat384x24d getMaskSct6();
    EGMat448x28d getMaskSct7();
    EGMat512x32d getMaskSct8();

    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle);
};

#endif //NGONPATCHCONSTRUCTOR_HPP
