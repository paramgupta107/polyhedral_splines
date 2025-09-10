/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <iostream>

typedef OpenMesh::PolyMesh_ArrayKernelT<>::Point Point;

extern "C" {
struct Patch
{
    Patch();
    Patch(int a_BiDeg);
    Patch(int a_BiDeg, std::string a_Group);

    Patch(int a_DegU, int a_DegV);

    Patch(int a_DegU, int a_DegV, std::string a_Group);

    Patch(const Patch& other) = default;
    Patch(Patch&& other) noexcept = default;
    Patch& operator=(const Patch& other);
    Patch& operator=(Patch&& other) noexcept;

    bool isValid() const;

    void initBBcoefs();

    void degRaise();

    // Please find patch type in
    // https://www.cise.ufl.edu/research/SurfLab/bview/#file-format
    const std::string m_PatchType = "5";

    int m_DegU;
    int m_DegV;
    std::string m_Group = "Group 0 default";

    // m_BBcoefs should be a m_DegU+1 x m_DegV+1 2D array
    std::vector<std::vector<Point>> m_BBcoefs;
};
}