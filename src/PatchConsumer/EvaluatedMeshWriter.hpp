/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <string>
#include <mutex>
#include <fstream>
#include <queue>

#include "PatchConsumer.hpp"
#include "../Patch/Patch.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

class EvaluatedMeshWriter : public PatchConsumer
{
public:
    EvaluatedMeshWriter(MeshType *mesh);
    void start();
    void stop();
    void consume(const Patch a_Patch);

private:
    MeshType* mesh;
    MeshType::Point de_casteljau_surface(float u, float v, const Patch a_Patch);
    MeshType::Point de_casteljau_normal(float u, float v, const Patch& a_Patch);
    void create_bezier_surface_mesh(const Patch a_Patch, int n, MeshType *mesh);
};
