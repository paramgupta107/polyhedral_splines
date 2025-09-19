/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <cstdint>
#include "PnSPatch_impl.hpp"

struct Patch;

/**
 * @class PnSPatch
 * @ingroup api_group
 * @brief Represents a polynomial patch in Bernstein–Bézier form.
 *
 * A PnSPatch stores the coefficients and degree information of a single spline patch.
 * 
 * This class uses the PIMPL idiom to ensure binary compatibility across different versions of the library. The member functions are inline functions that call binary safe C functions declared in @ref PnSPatch_impl.hpp.
 * 
 */
class PnSPatch {
public:
    /**
     * @brief Default constructor.
     *
     * Constructs an empty patch.
     */
    PnSPatch();

    /**
     * @brief Destructor.
     */
    ~PnSPatch();

    /**
     * @brief Copy constructor.
     * @param patch Another patch to copy from.
     */
    PnSPatch(const PnSPatch& patch);

    /**
     * @brief Move constructor.
     * @param patch Another patch to move from.
     */
    PnSPatch(PnSPatch&& patch) noexcept;

    /**
     * @brief Copy assignment operator.
     * @param a_Other Another patch to copy from.
     * @return Reference to this patch.
     */
    PnSPatch& operator=(const PnSPatch& a_Other);

    /**
     * @brief Move assignment operator.
     * @param other Another patch to move from.
     * @return Reference to this patch.
     */
    PnSPatch& operator=(PnSPatch&& other) noexcept;

    /**
     * @brief Access Bézier coefficient by index.
     *
     * @param i Index in the u-direction between 0 and Degree in u-direction.
     * @param j Index in the v-direction between 0 and Degree in v-direction.
     * @param k Dimension of control point between 0 and 2 (x, y, or z).
     * @return Const reference to the coefficient value.
     */
    const double& operator()(uint32_t i, uint32_t j, uint32_t k) const;

    /**
     * @brief Check whether this patch is valid.
     * @return True if the patch is valid.
     */
    bool isValid() const;

    /**
     * @brief Perform degree elevation on the patch.
     *
     * Degree raise the patch upto degree 3 for each paramter. Degree greater than 3 will remain unchanged. This is not relevant for PnS3.
     */
    void degRaise();

    /**
     * @brief Get the polynomial degree in the u-direction.
     * @return Degree in u.
     */
    uint32_t getDegreeU() const;

    /**
     * @brief Get the polynomial degree in the v-direction.
     * @return Degree in v.
     */
    uint32_t getDegreeV() const;

private:
    /**
     * @brief Construct a patch from an internal implementation.
     * @param impl Pointer to underlying implementation.
     *
     * Only the PnSpline class can call this constructor.
     */
    PnSPatch(Patch *impl);

    friend class PnSpline;

    /// Opaque pointer to implementation (PIMPL idiom). This is to ensure binary compatibility.
    Patch *impl;
};



////////////////////////////////////////////////////////////////

inline PnSPatch::PnSPatch(Patch *impl) : impl(PnSPatch_clone(impl)) {}

inline PnSPatch::~PnSPatch(){
    PnSPatch_destroy(impl);
}

inline PnSPatch::PnSPatch(const PnSPatch& patch){
    impl = PnSPatch_clone(patch.impl);
}

inline PnSPatch::PnSPatch(PnSPatch&& patch) noexcept : impl(patch.impl) {
    patch.impl = nullptr;
}

inline PnSPatch& PnSPatch::operator=(const PnSPatch& a_Other){
    if (this != &a_Other) {
        PnSPatch_destroy(impl);
        impl = PnSPatch_clone(a_Other.impl);
    }
    return *this;
}

inline PnSPatch& PnSPatch::operator=(PnSPatch&& other) noexcept {
    if (this != &other) {
        PnSPatch_destroy(impl);
        impl = other.impl;
        other.impl = nullptr;
    }
    return *this;
}

inline const double& PnSPatch::operator()(uint32_t i, uint32_t j, uint32_t k) const{
    return PnSPatch_getValue(impl, i, j, k);
}

inline bool PnSPatch::isValid() const{
    return PnSPatch_isValid(impl);
}

inline void PnSPatch::degRaise(){
    PnSPatch_degRaise(impl);
}

inline uint32_t PnSPatch::getDegreeU() const{
    return PnSPatch_getDegreeU(impl);
}

inline uint32_t PnSPatch::getDegreeV() const{
    return PnSPatch_getDegreeV(impl);
}