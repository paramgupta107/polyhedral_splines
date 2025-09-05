/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <vector>
#include "../Helper/Helper.hpp"
#include "../Subdivision/subdivision.hpp"
#include "../Subdivision/VertexMapping.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;
typedef MeshType::VertexHandle VertexHandle;

class PatchConstructor; // forward declaration

class PatchBuilder
{
    public:
        std::vector<VertexHandle> m_NBVertexHandles;
        Matrix m_Mask;
        PatchConstructor *m_PatchConstructor;

        // need either m_NumOfPatches or m_DegU and m_DegV to be set
        int m_NumOfPatches = -1;
        int m_DegU = -1;
        int m_DegV = -1;


        PatchBuilder(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_NumOfPatches);
        PatchBuilder(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_DegU, int a_DegV);
        
        PatchBuilder(const PatchBuilder& a_PatchBuilder)
            : m_NBVertexHandles(a_PatchBuilder.m_NBVertexHandles), m_Mask(a_PatchBuilder.m_Mask), m_PatchConstructor(a_PatchBuilder.m_PatchConstructor), m_NumOfPatches(a_PatchBuilder.m_NumOfPatches), m_DegU(a_PatchBuilder.m_DegU), m_DegV(a_PatchBuilder.m_DegV) {};
        PatchBuilder& operator=(const PatchBuilder& a_PatchBuilder);
        std::vector<Patch> buildPatches(const MeshType& a_Mesh) const;
        std::vector<VertexHandle> getNeighborVerts() const;
        Matrix getMask() const;
        const PatchConstructor* getPatchConstructor() const;
        void degRaise();
        int numPatches() const;
    private:
        void initializeMaskAndNeighborVertices(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask);
};