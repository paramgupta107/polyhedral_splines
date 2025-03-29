/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "../Patch/Patch.hpp"
#include "../Patch/T0PatchConstructor.hpp"
#include "../Patch/T1PatchConstructor.hpp"
#include "../Patch/T2PatchConstructor.hpp"
#include "../Patch/ExtraordinaryPatchConstructor.hpp"
// #include "../Patch/TwoTrianglesTwoQuadsPatchConstructor.hpp"
#include "../Patch/NGonPatchConstructor.hpp"
#include "../Patch/PolarPatchConstructor.hpp"
#include "../Patch/RegularPatchConstructor.hpp"

/*
 * Store the pointers of patch constructor
 */
class PatchConstructorPool
{
public:
    PatchConstructorPool()
    {
        // Functions to generate patches
        m_PatchConstructorPool.push_back(new RegularPatchConstructor());
        m_PatchConstructorPool.push_back(new ExtraordinaryPatchConstructor());
        // m_PatchConstructorPool.push_back(new TwoTrianglesTwoQuadsPatchConstructor()); // deprecated
        m_PatchConstructorPool.push_back(new PolarPatchConstructor());
        m_PatchConstructorPool.push_back(new T0PatchConstructor());
        m_PatchConstructorPool.push_back(new T1PatchConstructor());
        m_PatchConstructorPool.push_back(new T2PatchConstructor());
        m_PatchConstructorPool.push_back(new NGonPatchConstructor());
    }
    template <typename T> PatchConstructor* getPatchConstructor(const T& a_T, const MeshType& a_Mesh) const;

private:
    std::vector<PatchConstructor*> m_PatchConstructorPool;
};

template <typename T> PatchConstructor* PatchConstructorPool::getPatchConstructor(const T& a_T, const MeshType& a_Mesh) const
{
    for(auto t_PatchConstructor : m_PatchConstructorPool)
    {
        if(t_PatchConstructor->isSamePatchType(a_T, a_Mesh))
        {
            return t_PatchConstructor;
        }
    }
    return nullptr;
}
template PatchConstructor* PatchConstructorPool::getPatchConstructor(const VertexHandle& a_VertexHandle, const MeshType& a_Mesh) const;
template PatchConstructor* PatchConstructorPool::getPatchConstructor(const FaceHandle& a_FaceHandle, const MeshType& a_Mesh) const;
