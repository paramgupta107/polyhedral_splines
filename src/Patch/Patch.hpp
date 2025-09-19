/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <iostream>

typedef OpenMesh::PolyMesh_ArrayKernelT<>::Point Point;

extern "C" {

/**
 * \ingroup patch_build
 * @brief A class representing a Bézier patch.
 * 
 * The Patch class encapsulates the properties and control points of a Bézier patch.
 * It includes methods for initialization, validation, and degree elevation.
 */
struct Patch
{
    /**
     * @brief Construct a new Patch object
     * 
     */
    Patch();

    /**
     * @brief Construct a new Patch object
     * 
     * @param a_BiDeg The bi-degree of the patch (same in both u and v directions).
     */
    Patch(int a_BiDeg);

    /**
     * @brief Construct a new Patch object
     * 
     * @param a_BiDeg The bi-degree of the patch (same in both u and v directions).
     * @param a_Group The group name associated with the patch.
     */
    Patch(int a_BiDeg, std::string a_Group);

    /**
     * @brief Construct a new Patch object
     * 
     * @param a_DegU The degree of the patch in the u direction.
     * @param a_DegV The degree of the patch in the v direction.
     */
    Patch(int a_DegU, int a_DegV);

    /**
     * @brief Construct a new Patch object
     * 
     * @param a_DegU The degree of the patch in the u direction.
     * @param a_DegV The degree of the patch in the v direction.
     * @param a_Group The group name associated with the patch.
     */
    Patch(int a_DegU, int a_DegV, std::string a_Group);

    /**
     * @brief Copy constructor
     * 
     * @param other The Patch object to copy from.
     */
    Patch(const Patch& other) = default;

    /**
     * @brief Move constructor
     * 
     * @param other The Patch object to move from.
     */
    Patch(Patch&& other) noexcept = default;

    /**
     * @brief Copy assignment operator
     * 
     * @param other 
     * @return Patch& 
     */
    Patch& operator=(const Patch& other);

    /**
     * @brief Move assignment operator
     * 
     * @param other 
     * @return Patch& 
     */
    Patch& operator=(Patch&& other) noexcept;

    /**
     * @brief Checks if a patch is valid.
     * @return true if the patch is valid, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief intializes the control points array based on the degrees.
     */
    void initBBcoefs();

    /**
     * @brief Raises the degree of the patch by one in both u and v directions.
     * 
     * This method modifies the control points to reflect the degree elevation.
     */
    void degRaise();

    /** @brief Patch type
     * Please find patch type at
     * https://www.cise.ufl.edu/research/SurfLab/bview/#file-format
     */ 
    const std::string m_PatchType = "5";

    /**
     * @brief Degree in u direction.
     * 
     */
    int m_DegU;
    /**
     * @brief Degree in v direction.
     * 
     */
    int m_DegV;

    /**
     * @brief Group name associated with the patch.
     * 
     */
    std::string m_Group = "Group 0 default";

    /**
     * @brief Control points of the Bézier patch.
     * 
     * This is a 2D vector where each element represents a control point in 3D space.
     * The size of the outer vector is (m_DegU + 1) and the size of the inner vectors is (m_DegV + 1).
     */
    std::vector<std::vector<Point>> m_BBcoefs;
};
}