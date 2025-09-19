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

/**
 * \ingroup patch_build
 * @brief Pool PnS patch types represented as \ref PatchConstructor subclasses.
 * Each subclass corresponds to a specific PnS patch type.
 * There are two types of patch constructors: vertex-based and face-based.
 * A vertex-based patch constructor checks if a vertex and its neighborhood matches a PnS patch type.
 * A face-based patch constructor checks if a face and its neighborhood matches a PnS patch type.
 * 
 * The \ref getPatchConstructor function traverses the pool and returns the first matching patch constructor for a given vertex or face.
 * 
 */
class PatchConstructorPool
{
public:
    PatchConstructorPool()
    {
        // Functions to generate patches
        m_PatchConstructorPool.push_back(new PolarPatchConstructor()); // Polar must be before Regular & Extraordinary
        m_PatchConstructorPool.push_back(new RegularPatchConstructor());
        m_PatchConstructorPool.push_back(new ExtraordinaryPatchConstructor());
        // m_PatchConstructorPool.push_back(new TwoTrianglesTwoQuadsPatchConstructor()); // deprecated
        m_PatchConstructorPool.push_back(new T0PatchConstructor());
        m_PatchConstructorPool.push_back(new T1PatchConstructor());
        m_PatchConstructorPool.push_back(new T2PatchConstructor());
        m_PatchConstructorPool.push_back(new NGonPatchConstructor());
    }
    template <typename T> PatchConstructor* getPatchConstructor(const T& a_T, MeshType& a_Mesh, bool check_marked = false) const;

private:
    /**
     * @brief The list of that contains an object for each \ref PatchConstructor subclasses that are considered.
     * 
     */
    std::vector<PatchConstructor*> m_PatchConstructorPool;
};

/**
 * @brief Find a patch \ref PatchConstructor that matches the given vertex or face.
 * 
 * @tparam T Type of the input, either VertexHandle or FaceHandle
 * @param a_T Input vertex or face handle
 * @param a_Mesh The mesh to check against
 * @param check_marked If true, only consider unmarked vertices/faces. Marked elements always return nullptr.
 * @return PatchConstructor* Pointer to the matching PatchConstructor, or nullptr if no match is found.
 */
template <typename T> PatchConstructor* PatchConstructorPool::getPatchConstructor(const T& a_T, MeshType& a_Mesh, bool check_marked) const
{
    for(auto t_PatchConstructor : m_PatchConstructorPool)
    {
        if(t_PatchConstructor->isSamePatchType(a_T, a_Mesh, check_marked))
        {
            return t_PatchConstructor;
        }
    }
    return nullptr;
}
template PatchConstructor* PatchConstructorPool::getPatchConstructor(const VertexHandle& a_VertexHandle, MeshType& a_Mesh, bool check_marked) const;
template PatchConstructor* PatchConstructorPool::getPatchConstructor(const FaceHandle& a_FaceHandle, MeshType& a_Mesh, bool check_marked) const;
