#pragma once
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

class VertexMapping {
    public:    
        // Constructors & assignment
        VertexMapping();
        // VertexMapping(const std::vector<int>& indices, const std::vector<double>& values);
        VertexMapping(const VertexMapping& other) = default;
        VertexMapping& operator=(const VertexMapping& other) = default;
    
        // Arithmetic operators
        VertexMapping operator+(const VertexMapping& other) const;
        VertexMapping operator-(const VertexMapping& other) const;
        VertexMapping operator*(double scalar) const;
        VertexMapping operator/(double scalar) const;
        VertexMapping operator-() const;

        std::vector<MeshType::VertexHandle> indices;
        std::vector<double> mapping;
    };