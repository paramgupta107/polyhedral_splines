/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <vector>
#include <array>
#include <string>
#include <cstdint>

#include "PnSPatch.hpp"
#include "PnSpline_impl.hpp"

struct Patch;
extern "C" {
    struct PnSplineImpl; // opaque handle to impl
}

class PnSpline {
public:
    PnSpline();
    PnSpline(std::vector<std::array<double,3>>& controlPoints,
             std::vector<std::vector<uint32_t>>& controlIndices,
             bool degRaise = false);
    PnSpline(const PnSpline& other);
    PnSpline& operator=(const PnSpline& other); // Makes a copy
    PnSpline(PnSpline&& other) noexcept;
    PnSpline& operator=(PnSpline&& other) noexcept;
    ~PnSpline();

    std::vector<uint32_t> updateControlMesh(
        std::vector<std::array<double,3>>& updatedControlPoints,
        std::vector<uint32_t>& updateIndices);

    void degRaise();
    
    uint32_t numPatches() const;
    PnSPatch getPatch(uint32_t index) const;

private:
    PnSplineImpl* impl; // opaque pointer to implementation
};


////////////////////////////////////////////////////////////////
inline PnSpline::PnSpline() : impl(PnSpline_create_empty()) {}

inline PnSpline::PnSpline(std::vector<std::array<double,3>>& controlPoints,
                          std::vector<std::vector<uint32_t>>& controlIndices,
                          bool degRaise) {
    std::vector<double> flatPts;
    flatPts.reserve(controlPoints.size() * 3);
    for (auto& p : controlPoints) {
        flatPts.insert(flatPts.end(), {p[0], p[1], p[2]});
    }

    std::vector<uint32_t> flatIndices;
    std::vector<uint64_t> faceSizes;
    for (auto& face : controlIndices) {
        faceSizes.push_back(face.size());
        flatIndices.insert(flatIndices.end(), face.begin(), face.end());
    }

    impl = PnSpline_create_from_points(flatPts.data(), controlPoints.size(),
                                       flatIndices.data(), faceSizes.data(), controlIndices.size(),
                                       degRaise);
}

inline PnSpline::PnSpline(const PnSpline& other)
    : impl(PnSpline_clone(other.impl)) {}

inline PnSpline& PnSpline::operator=(const PnSpline& other) {
    if (this != &other) {
        if (impl) PnSpline_destroy(impl);
        impl = PnSpline_clone(other.impl);
    }
    return *this;
}

inline PnSpline::PnSpline(PnSpline&& other) noexcept : impl(other.impl) {
    other.impl = nullptr;
}

inline PnSpline& PnSpline::operator=(PnSpline&& other) noexcept {
    if (this != &other) {
        if (impl) PnSpline_destroy(impl);
        impl = other.impl;
        other.impl = nullptr;
    }
    return *this;
}

inline PnSpline::~PnSpline() {
    if (impl) PnSpline_destroy(impl);
}


inline std::vector<uint32_t> PnSpline::updateControlMesh(
    std::vector<std::array<double,3>>& updatedControlPoints,
    std::vector<uint32_t>& updateIndices) {

    std::vector<double> flatPts;
    flatPts.reserve(updatedControlPoints.size() * 3);
    for (auto& p : updatedControlPoints) {
        flatPts.insert(flatPts.end(), {p[0], p[1], p[2]});
    }

    std::vector<uint32_t> outPatchIds(PnSpline_getNumPatches(impl)); // over-allocate
    uint64_t numUpdated = PnSpline_updateControlMesh(
        impl, flatPts.data(), updatedControlPoints.size(),
        updateIndices.data(), updateIndices.size(),
        outPatchIds.data(), outPatchIds.size());

    outPatchIds.resize(numUpdated);
    return outPatchIds;
}

inline void PnSpline::degRaise() { PnSpline_degRaise(impl); }

inline uint32_t PnSpline::numPatches() const {
    return PnSpline_getNumPatches(impl);
}

inline PnSPatch PnSpline::getPatch(uint32_t index) const {
    return PnSPatch(PnSpline_getPatch(impl, index));
}