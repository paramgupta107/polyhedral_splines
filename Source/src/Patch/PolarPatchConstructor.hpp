/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#ifndef POLARPATCHCONSTRUCTOR_HPP
#define POLARPATCHCONSTRUCTOR_HPP

#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"

typedef Eigen::Matrix<double, 36, 4> EGMat36x4d;
typedef Eigen::Matrix<double, 48, 5> EGMat48x5d;
typedef Eigen::Matrix<double, 60, 6> EGMat60x6d;
typedef Eigen::Matrix<double, 72, 7> EGMat72x7d;
typedef Eigen::Matrix<double, 84, 8> EGMat84x8d;
typedef Eigen::Matrix<double, 96, 9> EGMat96x9d;

class PolarPatchConstructor : public PatchConstructor
{
public:
    PolarPatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh),
          m_MaskSct3(getMaskSct3()),
          m_MaskSct4(getMaskSct4()),
          m_MaskSct5(getMaskSct5()),
          m_MaskSct6(getMaskSct6()),
          m_MaskSct7(getMaskSct7()),
          m_MaskSct8(getMaskSct8()) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle) override;
    std::vector<Patch> getPatch(const VertexHandle& a_VertexHandle) override;

private:
    const MeshType& m_Mesh;
    const EGMat36x4d m_MaskSct3;
    const EGMat48x5d m_MaskSct4;
    const EGMat60x6d m_MaskSct5;
    const EGMat72x7d m_MaskSct6;
    const EGMat84x8d m_MaskSct7;
    const EGMat96x9d m_MaskSct8;
    int m_NumOfSct = 0;

    EGMat36x4d getMaskSct3();
    EGMat48x5d getMaskSct4();
    EGMat60x6d getMaskSct5();
    EGMat72x7d getMaskSct6();
    EGMat84x8d getMaskSct7();
    EGMat96x9d getMaskSct8();
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle);
};

#endif //POLARPATCHCONSTRUCTOR_HPP
