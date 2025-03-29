/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "PatchBuilder.hpp"
#include "PatchConstructor.hpp"

std::vector<VertexHandle> PatchBuilder::getNeighborVerts() const { return m_NBVertexHandles; }

Matrix PatchBuilder::getMask() const { return m_Mask; }

const PatchConstructor* PatchBuilder::getPatchConstructor() const { return m_PatchConstructor; }

std::vector<Patch> PatchBuilder::buildPatches(const MeshType& a_Mesh) const{
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(a_Mesh, m_NBVertexHandles);
    auto t_BBcoefs = m_Mask * t_NBVertsMat;
    if (m_NumOfPatches > 0) {
        return Helper::points_mat_to_patches(m_NumOfPatches, m_PatchConstructor->getGroupName(), t_BBcoefs);
    }
    else if (m_DegU > 0 && m_DegV > 0) {
        return Helper::points_mat_to_patches(m_DegU, m_DegV, m_PatchConstructor->getGroupName(), t_BBcoefs);
    }
    else {
        throw std::runtime_error("Invalid patch parameters: either NumOfPatches or DegU and DegV must be set.");
    }
}