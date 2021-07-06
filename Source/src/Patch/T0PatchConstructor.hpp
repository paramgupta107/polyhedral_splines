/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

#ifndef T0PATCHCONSTRUCTOR_HPP
#define T0PATCHCONSTRUCTOR_HPP

#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "PatchConstructor.hpp"

typedef Eigen::Matrix<double, 64, 14> EGMat64x14d;

class T0PatchConstructor : public PatchConstructor
{
public:
    T0PatchConstructor(const MeshType& a_Mesh)
        : m_Mesh(a_Mesh), m_Mask(getMask()) {};

    bool isSamePatchType(const FaceHandle& a_FaceHandle) override;
    std::vector<Patch> getPatch(const FaceHandle& a_FaceHandle) override;

private:
    const MeshType& m_Mesh;
    const EGMat64x14d m_Mask;

    EGMat64x14d getMask();
    std::vector<VertexHandle> initNeighborVerts(const FaceHandle& a_FaceHandle);

};

#endif //T0PATCHCONSTRUCTOR_HPP
