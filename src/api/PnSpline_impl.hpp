/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once
#include <vector>
#include <array>
#include <string>
#include <cstdint>

/* WARNING:
    C-style bridge functions for ABI stability.
    Should only be used with caution. */

    
struct Patch;

extern "C" {
struct PnSplineImpl;
PnSplineImpl* PnSpline_create_empty();
PnSplineImpl* PnSpline_create_from_points(const double* points, uint64_t numPoints,
                                        const uint32_t* faceIndices, const uint64_t* faceSizes, uint64_t numFaces,
                                        bool degRaise);
PnSplineImpl* PnSpline_clone(const PnSplineImpl* other);
void PnSpline_destroy(PnSplineImpl* impl);

// Operations
void PnSpline_degRaise(PnSplineImpl* impl);

uint64_t PnSpline_updateControlMesh(PnSplineImpl* impl,
                                const double* updatedPoints, uint64_t numPoints,
                                const uint32_t* updateIndices, uint64_t numIndices,
                                uint32_t* outUpdatedPatchIndices, uint64_t maxOut);

uint64_t PnSpline_getNumPatches(const PnSplineImpl* impl);

Patch* PnSpline_getPatch(const PnSplineImpl* impl, uint32_t index);
}