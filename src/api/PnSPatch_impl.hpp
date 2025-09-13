/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include <cstdint>

/* WARNING:
    C-style bridge functions for ABI stability.
    Should only be used with caution. */

struct Patch;

extern "C" {
bool PnSPatch_isValid(const Patch* patch);
void PnSPatch_degRaise(Patch* patch);

double PnSPatch_getValue(const Patch* patch, uint32_t i, uint32_t j, uint32_t k);

uint32_t PnSPatch_getDegreeU(const Patch* patch);
uint32_t PnSPatch_getDegreeV(const Patch* patch);

Patch* PnSPatch_clone(const Patch* patch);

void PnSPatch_destroy(Patch* patch);

}