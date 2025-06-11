#include "ProcessMesh.hpp"
#include "Helper/Helper.hpp"
#include "Patch/PatchBuilder.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "Patch/Patch.hpp"
#include "PatchConsumer/BVWriter.hpp"
#include "PatchConsumer/IGSWriter.hpp"
#include "PatchConsumer/STEPWriter.hpp"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <vector>

extern "C"
{
//-----------------------------------------------------------------------------
// PNSControlMesh Functions                                                   |
//-----------------------------------------------------------------------------
    MeshType* MeshCreate_Interop()
    {
        return new MeshType();
    }
    
    void MeshDestroy_Interop(MeshType* mesh)
    {
        delete mesh;
    }
    
    MeshType* MeshFromFile_Interop(const char* filename)
    {
        MeshType* mesh = new MeshType();
        bool success = OpenMesh::IO::read_mesh(*mesh, filename);
        if (!success) {
            delete mesh;
            return nullptr;
        }
        return mesh;
    }
    
    MeshType* MeshFromData_Interop(const float* vertices, int vertexCount, const int* faceIndices, const int* faceSizes, int faceCount)
    {
        MeshType* mesh = new MeshType();
        
        std::vector<MeshType::VertexHandle> vertexHandles;
        for (int i = 0; i < vertexCount; i++) {
            float x = vertices[i*3];
            float y = vertices[i*3+1];
            float z = vertices[i*3+2];
            vertexHandles.push_back(mesh->add_vertex(MeshType::Point(x, y, z)));
        }
        
        int indexOffset = 0;
        for (int i = 0; i < faceCount; i++) {
            int size = faceSizes[i];
            std::vector<MeshType::VertexHandle> faceVerts;
            for (int j = 0; j < size; j++) {
                int idx = faceIndices[indexOffset + j];
                if (idx < 0 || idx >= vertexCount) continue;
                faceVerts.push_back(vertexHandles[idx]);
            }
            mesh->add_face(faceVerts);
            indexOffset += size;
        }
        
        return mesh;
    }
    
    void MeshSetVertex_Interop(MeshType* mesh, int index, float x, float y, float z)
    {
        if (!mesh) return;
        auto vh = mesh->vertex_handle(index);
        mesh->set_point(vh, MeshType::Point(x, y, z));
    }
    
    void MeshGetVertex_Interop(const MeshType* mesh, int index, float* coords)
    {
        if (!mesh) return;
        auto vh = mesh->vertex_handle(index);
        auto point = mesh->point(vh);
        coords[0] = point[0];
        coords[1] = point[1];
        coords[2] = point[2];
    }
    
    int MeshGetVertexCount_Interop(const MeshType* mesh)
    {
        if (!mesh) return 0;
        return mesh->n_vertices();
    }
    
    void MeshGetVertices_Interop(const MeshType* mesh, float* coords, int count)
    {
        if (!mesh) return;
        int i = 0;
        for (auto vh : mesh->vertices()) {
            if (i >= count/3) break;
            auto point = mesh->point(vh);
            coords[i*3] = point[0];
            coords[i*3+1] = point[1];
            coords[i*3+2] = point[2];
            i++;
        }
    }
    
    int MeshGetFaceCount_Interop(const MeshType* mesh)
    {
        if (!mesh) return 0;
        return mesh->n_faces();
    }
    
    void MeshGetFaceSizes_Interop(const MeshType* mesh, int* sizes, int count)
    {
        if (!mesh) return;
        int i = 0;
        for (auto fh : mesh->faces()) {
            if (i >= count) break;
            sizes[i] = mesh->valence(fh);
            i++;
        }
    }
    
    void MeshGetFaceIndices_Interop(const MeshType* mesh, int* indices, int totalCount) {
        if (!mesh) return;
        int index = 0;
        for (auto fh : mesh->faces()) {
            for (auto vh : mesh->fv_range(fh)) {
                if (index >= totalCount) return;
                indices[index++] = vh.idx();
            }
        }
    }
    
//-----------------------------------------------------------------------------
// PatchBuilder functions                                                     |
//-----------------------------------------------------------------------------
    void PatchBuilderDestroy_Interop(PatchBuilder* builder) {
        delete builder;
    }
    
    PatchBuilder* PatchBuilder_Clone_Interop(PatchBuilder* builder) {
        if (!builder) return nullptr;
        return new PatchBuilder(*builder);
    }
    
    int PatchBuilderGetNeighborVertCount_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->getNeighborVerts().size();
    }
    
    void PatchBuilderGetNeighborVerts_Interop(const PatchBuilder* builder, int* indices, int count) {
        if (!builder) return;
        auto verts = builder->getNeighborVerts();
        for (int i = 0; i < count && i < verts.size(); i++) {
            indices[i] = verts[i].idx();
        }
    }
    
    int PatchBuilderGetMaskRows_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->getMask().getRows();
    }
    
    int PatchBuilderGetMaskCols_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->getMask().getCols();
    }
    
    void PatchBuilderGetMask_Interop(const PatchBuilder* builder, float* mask, int size) {
        if (!builder) return;
        const Matrix& m = builder->getMask();
        int rows = m.getRows();
        int cols = m.getCols();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (i * cols + j >= size) return;
                mask[i * cols + j] = m(i, j);
            }
        }
    }
    
    int PatchBuilderGetNumPatches_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->m_NumOfPatches;
    }
    
    int PatchBuilderGetDegU_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->m_DegU;
    }
    
    int PatchBuilderGetDegV_Interop(const PatchBuilder* builder) {
        if (!builder) return 0;
        return builder->m_DegV;
    }
    
    const char* PatchBuilderGetPatchType_Interop(const PatchBuilder* builder) {
        if (!builder || !builder->getPatchConstructor()) return "";
        return builder->getPatchConstructor()->getGroupName().c_str();
    }
    
    int PatchBuilderGetPatchCount_Interop(const PatchBuilder* builder, const MeshType* mesh) {
        if (!builder || !mesh) return 0;
        std::vector<Patch> patches = builder->buildPatches(*mesh);
        return patches.size();
    }
    
    void PatchBuilderBuildPatches_Interop(const PatchBuilder* builder, const MeshType* mesh, Patch** patchesOut) {
        if (!builder || !mesh || !patchesOut) return;
        std::vector<Patch> patches = builder->buildPatches(*mesh);
        for (size_t i = 0; i < patches.size(); i++) {
            patchesOut[i] = new Patch(patches[i]);
        }
    }
    
    
//-----------------------------------------------------------------------------
// Patch Functions                                                            |
//-----------------------------------------------------------------------------
    void PatchDestroy_Interop(Patch* patch) {
        delete patch;
    }
    
    int PatchGetDegU_Interop(const Patch* patch) {
        if (!patch) return 0;
        return patch->m_DegU;
    }
    
    int PatchGetDegV_Interop(const Patch* patch) {
        if (!patch) return 0;
        return patch->m_DegV;
    }
    
    const char* PatchGetGroup_Interop(const Patch* patch) {
        if (!patch) return "";
        return patch->m_Group.c_str();
    }
    
    bool PatchIsValid_Interop(const Patch* patch) {
        if (!patch) return false;
        return patch->isValid();
    }
    
    void PatchGetBBCoefs_Interop(const Patch* patch, float* coefs, int size) {
        if (!patch) return;
        int rows = patch->m_BBcoefs.size();
        if (rows == 0) return;
        
        int cols = patch->m_BBcoefs[0].size();
        int index = 0;
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (index + 2 >= size) return;
                const auto& pt = patch->m_BBcoefs[i][j];
                coefs[index++] = pt[0];
                coefs[index++] = pt[1];
                coefs[index++] = pt[2];
            }
        }
    }
    
    void PatchDegRaise_Interop(Patch* patch) {
        if (!patch) return;
        patch->degRaise();
    }
    
//-----------------------------------------------------------------------------
// Patch Consumer Functions                                                   |
//-----------------------------------------------------------------------------
    void PatchConsumerDestroy_Interop(PatchConsumer* consumer) {
        delete consumer;
    }
    
    void PatchConsumerStart_Interop(PatchConsumer* consumer) {
        if (consumer) consumer->start();
    }
    
    void PatchConsumerStop_Interop(PatchConsumer* consumer) {
        if (consumer) consumer->stop();
    }
    
    void PatchConsumerConsume_Interop(PatchConsumer* consumer, Patch* patch) {
        if (consumer && patch) consumer->consume(*patch);
    }


//-----------------------------------------------------------------------------
// PNS Utility Class Functions                                                |
//-----------------------------------------------------------------------------
    void ProcessMesh_Interop(MeshType* mesh, PatchConsumer* consumer, bool isDegRaise) {
        if (mesh && consumer) {
            process_mesh(*mesh, consumer, isDegRaise);
        }
    }
    
    int GetPatchBuildersCount_Interop(const MeshType* mesh) {
        if (!mesh) return 0;
        auto builders = getPatchBuilders(*mesh);
        return builders.size();
    }
    
    void GetPatchBuilders_Interop(const MeshType* mesh, PatchBuilder** buildersOut, int count) {
        if (!mesh || !buildersOut) return;
        auto builders = getPatchBuilders(*mesh);
        for (size_t i = 0; i < builders.size() && i < count; i++) {
            buildersOut[i] = new PatchBuilder(builders[i]);
        }
    }
    
    BVWriter* BVWriterCreate_Interop(const char* filename) {
        return new BVWriter(filename);
    }
    
    IGSWriter* IGSWriterCreate_Interop(const char* filename) {
        return new IGSWriter(filename);
    }

    STEPWriter* STEPWriterCreate_Interop(const char* filename) {
        return new STEPWriter(filename);
    }

}