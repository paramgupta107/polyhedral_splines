/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "PnSPatch_impl.hpp"
#include "Patch/Patch.hpp"

extern "C" {
bool PnSPatch_isValid(const Patch* patch){
    return patch->isValid();
}
void PnSPatch_degRaise(Patch* patch){
    patch->degRaise();
}

double PnSPatch_getValue(const Patch* patch, uint32_t i, uint32_t j, uint32_t k){
    return patch->m_BBcoefs[i][j][k];
}

uint32_t PnSPatch_getDegreeU(const Patch* patch){
    return patch->m_DegU;
}

uint32_t PnSPatch_getDegreeV(const Patch* patch){
    return patch->m_DegV;
}

Patch* PnSPatch_clone(const Patch* patch){
    if (!patch) return nullptr;
    return new Patch(*patch);
}

void PnSPatch_destroy(Patch* patch){
    if(patch) delete patch;
}

}