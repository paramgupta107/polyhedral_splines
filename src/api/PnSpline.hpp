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

/** \defgroup api_group Public C++ API
 *  @brief C++ classes forming the ABI safe public interface of the Polyhedral Net Splines library. @ref cpp_lib "Usage example"
 */

/**
 * @class PnSpline
 * @ingroup api_group
 * @brief Represents a PnSpline(Polyhedral net Spline) composed of piecwise polynomial patches @ref PnSPatch
 *
 * A PnSpline manages a collection of patches defined over a control mesh.
 * It provides methods to update the underlying control structure efficiently without needing to reconstruct the entire surface, retrieve
 * individual patches, and perform operations such as degree elevation.
 * 
 * This class uses the PIMPL idiom to ensure binary compatibility across different versions of the library.
 * The member functions are inline functions that call binary safe C functions declared in @ref PnSpline_impl.hpp.
 * 
 */
class PnSpline {
public:
    /**
     * @brief Construct an empty PnSpline.
     *
     * Only useful as a placeholder before assignment.
     */
    PnSpline();

    /**
     * @brief Construct a PnSpline from control net data.
     *
     * @param controlPoints 3D positions of control points.
     * @param controlIndices Connectivity information for the control mesh. Indices are 0-based. Orientation of the faces must be consistent.
     * @param degRaise If true, degree raise all patches upto degree 3 for each paramter. Degree greater than 3 will remain unchanged. This is not relevant for PnS3.
     *
     * 
     * @note The PnSpline does not maintain any references to the input data but stores a copy. The input control points and indices can be safely deleted after construction.
     * 
     */
    PnSpline(std::vector<std::array<double,3>>& controlPoints,
             std::vector<std::vector<uint32_t>>& controlIndices,
             bool degRaise = false);

    /**
     * @brief Copy constructor.
     * @param other Another PnSpline to copy from.
     */
    PnSpline(const PnSpline& other);

    /**
     * @brief Copy assignment operator.
     * @param other Another PnSpline to copy from.
     * @return Reference to this PnSpline.
     */
    PnSpline& operator=(const PnSpline& other);

    /**
     * @brief Move constructor.
     * @param other Another PnSpline to move from.
     */
    PnSpline(PnSpline&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other Another PnSpline to move from.
     * @return Reference to this PnSpline.
     */
    PnSpline& operator=(PnSpline&& other) noexcept;

    /**
     * @brief Destructor.
     */
    ~PnSpline();

    /**
     * @brief Update part of the control mesh.
     *
     * @param updatedControlPoints New vertex positions.
     * @param updateIndices Indices of the control points to update.
     * @return Indices of patches that were affected by the update.
     *
     * This allows incremental updates without rebuilding the entire surface.
     *
     * The patches affected be retieved using @ref getPatch.
     */
    std::vector<uint32_t> updateControlMesh(
        std::vector<std::array<double,3>>& updatedControlPoints,
        std::vector<uint32_t>& updateIndices);

    /**
     * @brief Degree raise all patches upto degree 3 for each paramter. Degree greater than 3 will remain unchanged. This is not relevant for PnS3.
     */
    void degRaise();
    
    /**
     * @brief Get the number of patches in this PnSpline.
     * @return Number of PnSPatch elements.
     */
    uint32_t numPatches() const;

    /**
     * @brief Access an individual patch by index.
     * @param index Patch index in the range [0, numPatches()).
     * @return A patch object.
     * 
     * @note The patch must be retrieved again after any update.
     * 
     */
    PnSPatch getPatch(uint32_t index) const;

private:
    /// Opaque pointer to implementation (PIMPL idiom). This is to ensure binary compatibility.
    PnSplineImpl* impl;
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