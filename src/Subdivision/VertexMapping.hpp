#pragma once
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

/**
 * \ingroup subdivision
 * @brief Represents a mapping from vertices in subdivided mesh to vertices in the original mesh.
 * Each mapping consists of a list of vertex indices in the original and corresponding weights. The vertex in the subdivided mesh is represented as a weighted combination of the vertices in the original mesh.
 * This class contains arithmetic operators to facilitate combining mappings during subdivision.
 * 
 */
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

        /**
         * @brief The VertexHandles of the vertices in the original mesh that contribute to this vertex in the subdivided mesh.
         * 
         */
        std::vector<MeshType::VertexHandle> indices;

        /**
         * @brief The weights associated with each vertex in \ref indices.
         * The weights should sum to 1.0.
         * 
         */
        std::vector<double> mapping;
    };