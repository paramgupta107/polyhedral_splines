/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#ifndef TWOTRIANGLESTWOQUADSPATCHCONSTRUCTOR_HPP
#define TWOTRIANGLESTWOQUADSPATCHCONSTRUCTOR_HPP

#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"

typedef Eigen::Matrix<double, 9, 9> EGMat9x9d;
typedef Eigen::Matrix<double, 9, 3> EGVert9d;

class TwoTrianglesTwoQuadsPatchConstructor : public PatchConstructor
{
public:
    TwoTrianglesTwoQuadsPatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh), m_Mask(getMask()) {};

    bool isSamePatchType(const VertexHandle& a_VertexHandle) override;
    std::vector<Patch> getPatch(const VertexHandle& a_VertexHandle) override;

    Patch getPatch(const std::vector<VertexHandle>& a_NBVertexHandles);
    EGVert9d getEGVertPatch(const std::vector<VertexHandle>& a_NBVertexHandles);

private:
    const MeshType& m_Mesh;
    const EGMat9x9d m_Mask;

    EGMat9x9d getMask();
    std::vector<VertexHandle> initNeighborVerts(const VertexHandle& a_VertexHandle);

};

#endif //TWOTRIANGLESTWOQUADSPATCHCONSTRUCTOR_HPP
