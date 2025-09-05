#include "VertexMapping.hpp"

VertexMapping::VertexMapping() = default;

// VertexMapping::VertexMapping(const std::vector<int>& a_indices, const std::vector<double>& a_mapping)
// {
//     if (indices.size() != mapping.size())
//     throw std::invalid_argument("Indices and mapping must match in size");

//     // Pair up and sort by index
//     std::vector<std::pair<int, double>> pairs;
//     pairs.reserve(indices.size());
//     for (size_t i = 0; i < indices.size(); ++i) {
//         pairs.emplace_back(indices[i], mapping[i]);
//     }
//     std::sort(pairs.begin(), pairs.end(),
//         [](auto& a, auto& b) { return a.first < b.first; });

//     // Merge duplicates by summing
//     indices.clear();
//     mapping.clear();
//     for (auto& p : pairs) {
//         if (indices.empty() || indices.back() != p.first) {
//             indices.push_back(p.first);
//             mapping.push_back(p.second);
//         } else {
//             mapping.back() += p.second;
//         }
//     }
// }

VertexMapping VertexMapping::operator+(const VertexMapping& other) const {
    VertexMapping result;
    result.indices.reserve(indices.size() + other.indices.size());
    result.mapping.reserve(indices.size() + other.indices.size());

    size_t i = 0, j = 0;
    while (i < indices.size() && j < other.indices.size()) {
        if (indices[i] == other.indices[j]) {
            double sum = mapping[i] + other.mapping[j];
            if (sum != 0.0) {
                result.indices.push_back(indices[i]);
                result.mapping.push_back(sum);
            }
            ++i; ++j;
        } else if (indices[i] < other.indices[j]) {
            result.indices.push_back(indices[i]);
            result.mapping.push_back(mapping[i]);
            ++i;
        } else {
            result.indices.push_back(other.indices[j]);
            result.mapping.push_back(other.mapping[j]);
            ++j;
        }
    }
    while (i < indices.size()) {
        result.indices.push_back(indices[i]);
        result.mapping.push_back(mapping[i]);
        ++i;
    }
    while (j < other.indices.size()) {
        result.indices.push_back(other.indices[j]);
        result.mapping.push_back(other.mapping[j]);
        ++j;
    }
    return result;
}

VertexMapping VertexMapping::operator-() const {
    return (*this) * -1.0;
}

VertexMapping VertexMapping::operator-(const VertexMapping& other) const {
    return (*this) + (-other);
}

VertexMapping VertexMapping::operator*(double scalar) const {
    if (scalar == 0.0)
        return VertexMapping();
    VertexMapping result;
    result.indices = indices;
    result.mapping.reserve(mapping.size());
    for (double v : mapping)
        result.mapping.push_back(v * scalar);
    return result;
}

VertexMapping VertexMapping::operator/(double scalar) const {
    if (scalar == 0.0)
        throw std::invalid_argument("Division by zero in VertexMapping");
    return (*this) * (1.0 / scalar);
}