/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <cstdint>
#include "PnSPatch_impl.hpp"

struct Patch;

class PnSPatch{
    public:
        PnSPatch();
        ~PnSPatch();
        PnSPatch(const PnSPatch& patch);
        PnSPatch(PnSPatch&& patch) noexcept;


        PnSPatch& operator=(const PnSPatch& a_Other);
        PnSPatch& operator=(PnSPatch&& other) noexcept;

        const double& operator()(uint32_t i, uint32_t j, uint32_t k) const;


        bool isValid() const;
        void degRaise();

        uint32_t getDegreeU() const;
        uint32_t getDegreeV() const;

    private:
        // Only created by PnSpline
        PnSPatch(Patch *impl);
        friend class PnSpline;
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