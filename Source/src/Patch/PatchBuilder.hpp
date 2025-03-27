/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <vector>
#include "../Helper/Helper.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;

class PatchConstructor; // forward declaration

class PatchBuilder
{
    private:
        const std::vector<VertexHandle> m_NBVertexHandles;
        const Matrix m_Mask;
        PatchConstructor *m_PatchConstructor;
        const MeshType& m_Mesh;

        // need either m_NumOfPatches or m_DegU and m_DegV to be set
        const int m_NumOfPatches = -1;
        const int m_DegU = -1;
        const int m_DegV = -1;


    public:
        PatchBuilder(std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, const MeshType& a_Mesh, int a_NumOfPatches)
            : m_NBVertexHandles(a_NBVertexHandles), m_Mask(a_Mask), m_PatchConstructor(a_PatchConstructor), m_Mesh(a_Mesh), m_NumOfPatches(a_NumOfPatches) {};
        PatchBuilder(std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, const MeshType& a_Mesh, int a_DegU, int a_DegV)
            : m_NBVertexHandles(a_NBVertexHandles), m_Mask(a_Mask), m_PatchConstructor(a_PatchConstructor), m_Mesh(a_Mesh), m_DegU(a_DegU), m_DegV(a_DegV) {};
        std::vector<Patch> buildPatches();
        std::vector<VertexHandle> getNeighborVerts() const { return m_NBVertexHandles; }
        Matrix getMask() const { return m_Mask; }
        const PatchConstructor* getPatchConstructor() const { return m_PatchConstructor; }
};