/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include "PatchBuilder.hpp"
#include "PatchConstructor.hpp"

PatchBuilder::PatchBuilder(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_NumOfPatches){
    m_NumOfPatches = a_NumOfPatches;
    int t_NumOfCCPtsPerPatch = a_Mask.getRows() / m_NumOfPatches;
    const int t_Deg = sqrt(t_NumOfCCPtsPerPatch) - 1;
    m_DegU = t_Deg;
    m_DegV = t_Deg;
    m_PatchConstructor = a_PatchConstructor;
    initializeMaskAndNeighborVertices(a_Mesh, a_NBVertexHandles, a_Mask);
};
PatchBuilder::PatchBuilder(const MeshType& a_Mesh,std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask, PatchConstructor* a_PatchConstructor, int a_DegU, int a_DegV){
    m_DegU = a_DegU;
    m_DegV = a_DegV;
    int t_NumOfCCPtsPerPatch = (m_DegU + 1) * (m_DegV + 1);
    m_NumOfPatches = a_Mask.getRows() / t_NumOfCCPtsPerPatch;
    m_PatchConstructor = a_PatchConstructor;
    initializeMaskAndNeighborVertices(a_Mesh, a_NBVertexHandles, a_Mask);
};

PatchBuilder& PatchBuilder::operator=(const PatchBuilder& a_PatchBuilder){
    if (this != &a_PatchBuilder) {
        m_NBVertexHandles = a_PatchBuilder.m_NBVertexHandles;
        m_Mask = a_PatchBuilder.m_Mask;
        m_PatchConstructor = a_PatchBuilder.m_PatchConstructor;
        m_NumOfPatches = a_PatchBuilder.m_NumOfPatches;
        m_DegU = a_PatchBuilder.m_DegU;
        m_DegV = a_PatchBuilder.m_DegV;
    }
    return *this;
}

std::vector<VertexHandle> PatchBuilder::getNeighborVerts() const { return m_NBVertexHandles; }

Matrix PatchBuilder::getMask() const { return m_Mask; }

const PatchConstructor* PatchBuilder::getPatchConstructor() const { return m_PatchConstructor; }

std::vector<Patch> PatchBuilder::buildPatches(const MeshType& a_Mesh) const{
    auto t_NBVertsMat = Helper::verthandles_to_points_mat(a_Mesh, m_NBVertexHandles);
    auto t_BBcoefs = m_Mask * t_NBVertsMat;
    return Helper::points_mat_to_patches(m_DegU, m_DegV, m_PatchConstructor->getGroupName(), t_BBcoefs);
}

static int getCPIndex(int a_DegU, int a_DegV, int patchIdx, int i, int j){
    int t_NumOfCCPtsPerPatch = (a_DegU + 1) * (a_DegV + 1);
    return patchIdx * t_NumOfCCPtsPerPatch + i * (a_DegV+1) + j;
}
void PatchBuilder::degRaise(){
    int t_NumOfPatches = m_NumOfPatches;
    int t_NumOfCCPtsPerPatch  = m_Mask.getRows() / m_NumOfPatches;
    bool t_IsRaiseU = m_DegU < 3;
    bool t_IsRaiseV = m_DegV < 3;
    if(!t_IsRaiseU && !t_IsRaiseV)
    {
        return;
    }
    int t_UR = t_IsRaiseU ? m_DegU+1 : m_DegU;
    int t_VR = t_IsRaiseV ? m_DegV+1 : m_DegV;
    
    Matrix t_BBUR((t_UR+1) * (m_DegV+1) * t_NumOfPatches, m_Mask.getCols());
    if(t_IsRaiseU)
    {
        for(int patchIdx = 0; patchIdx < t_NumOfPatches; patchIdx++){
            for(int i=0; i<=t_UR; i++)
            {
                int k = t_UR - i;
                for(int j=0; j<=m_DegV; j++)
                {
                    int a = (i-1 < 0) ? 0 : i-1;
                    int b = (i > m_DegU) ? i-1 : i;

                    int ajIdx = getCPIndex(m_DegU, m_DegV, patchIdx, a, j);
                    int bjIdx = getCPIndex(m_DegU, m_DegV, patchIdx, b, j);
                    int ijIdx = getCPIndex(t_UR, m_DegV, patchIdx, i, j);
                    for(int colIdx = 0; colIdx < m_Mask.getCols(); colIdx++)
                    {
                        t_BBUR(ijIdx, colIdx) = (i * m_Mask(ajIdx, colIdx) + k * m_Mask(bjIdx, colIdx)) / t_UR;
                    }

                }
            }
        }
        if(!t_IsRaiseV)
        {
            m_Mask = t_BBUR;
        }
    }
    if(t_IsRaiseV && !t_IsRaiseU)
    {
        Matrix t_BBVR((m_DegU+1) * (t_VR+1) * t_NumOfPatches, m_Mask.getCols());
        for(int patchIdx = 0; patchIdx < t_NumOfPatches; patchIdx++){
            for(int j=0; j<=t_VR; j++)
            {
                int k = t_VR - j;
                for(int i=0; i<=m_DegU; i++)
                {
                    int a = (j-1 < 0) ? 0 : j-1;
                    int b = (j > m_DegV) ? j-1 : j;
                    int iaIdx = getCPIndex(m_DegU, m_DegV, patchIdx, i, a);
                    int ibIdx = getCPIndex(m_DegU, m_DegV, patchIdx, i, b);
                    int ijIdx = getCPIndex(m_DegU, t_VR, patchIdx, i, j);
                    for(int colIdx = 0; colIdx < m_Mask.getCols(); colIdx++)
                    {
                        t_BBVR(ijIdx, colIdx) = (j * m_Mask(iaIdx, colIdx) + k * m_Mask(ibIdx, colIdx)) / t_VR;
                    }
                }
            }
        }
        m_Mask = t_BBVR;
    }
    else  // deg raise u & v direction
    {
        Matrix t_BBR((t_UR+1) * (t_VR+1) * t_NumOfPatches, m_Mask.getCols());
        for(int patchIdx = 0; patchIdx < t_NumOfPatches; patchIdx++){
            for(int j=0; j<=t_VR; j++)
            {
                int k = t_VR - j;
                for(int i=0; i<=t_UR; i++)
                {
                    int a = (j-1 < 0) ? 0 : j-1;
                    int b = (j > m_DegV) ? j-1 : j;

                    int iaIdx = getCPIndex(t_UR, m_DegV, patchIdx, i, a);
                    int ibIdx = getCPIndex(t_UR, m_DegV, patchIdx, i, b);
                    int ijIdx = getCPIndex(t_UR, t_VR, patchIdx, i, j);
                    for(int colIdx = 0; colIdx < m_Mask.getCols(); colIdx++)
                    {
                        t_BBR(ijIdx, colIdx) = (j * t_BBUR(iaIdx, colIdx) + k * t_BBUR(ibIdx, colIdx)) / t_VR;
                    }
                }
            }
        }
        m_Mask = t_BBR;
    }
    // update member deg
    m_DegU = t_UR;
    m_DegV = t_VR;
}

int PatchBuilder::numPatches() const{
    return m_NumOfPatches;
}

void PatchBuilder::initializeMaskAndNeighborVertices(const MeshType& a_Mesh, std::vector<VertexHandle> a_NBVertexHandles, const Matrix& a_Mask) {
    if (!OpenMesh::hasProperty<OpenMesh::VertexHandle, VertexMapping>(a_Mesh, "vertex_mapping")) { 
        m_NBVertexHandles = a_NBVertexHandles;
        m_Mask = a_Mask;
        return;
    }
    OpenMesh::VPropHandleT<VertexMapping> vertexMapping;
    a_Mesh.get_property_handle(vertexMapping, "vertex_mapping");
    // auto vertexMapping = OpenMesh::VProp<VertexMapping>(a_Mesh, "vertex_mapping");
    std::set<VertexHandle> uniqueHandles;
    for (auto vh : a_NBVertexHandles) {
        for (auto originalVh : a_Mesh.property(vertexMapping, vh).indices) {
            uniqueHandles.insert(originalVh);
        }
    }
    m_NBVertexHandles.assign(uniqueHandles.begin(), uniqueHandles.end());
    m_Mask = Matrix(a_Mask.getRows(), m_NBVertexHandles.size());
    for (int row = 0; row < a_Mask.getRows(); ++row) {
        for (int i = 0; i < a_NBVertexHandles.size(); ++i) {
            auto vh = a_NBVertexHandles[i];
            for (int j = 0; j < a_Mesh.property(vertexMapping, vh).indices.size(); ++j) {
                auto originalVh = a_Mesh.property(vertexMapping, vh).indices[j];
                int colIdx = std::distance(uniqueHandles.begin(), uniqueHandles.find(originalVh));
                m_Mask(row, colIdx) += a_Mask(row, i) * a_Mesh.property(vertexMapping, vh).mapping[j];
            }
        }
    }
    
}