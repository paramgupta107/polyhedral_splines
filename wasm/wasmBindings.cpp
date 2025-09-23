#  define OM_STATIC_BUILD 1

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <emscripten.h>

#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "PatchConsumer/BVWriter.hpp"
#include "PatchConsumer/IGSWriter.hpp"
#include "PatchConsumer/STEPWriter.hpp"
#include "ProcessMesh.hpp"
#include "PatchConsumer/EvaluatedMeshWriter.hpp"


typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;


void scaleAndTranslateMesh(MeshType &mesh) {
    // Variables to store the min and max coordinates
    MeshType::Point min_point(FLT_MAX, FLT_MAX, FLT_MAX);
    MeshType::Point max_point(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // Find the bounding box of the mesh
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        MeshType::Point p = mesh.point(*v_it);
        for (int i = 0; i < 3; ++i) {
            if (p[i] < min_point[i]) min_point[i] = p[i];
            if (p[i] > max_point[i]) max_point[i] = p[i];
        }
    }

    // Compute the center of the bounding box
    MeshType::Point center = (min_point + max_point) * 0.5;

    // Compute the scaling factor (largest dimension)
    MeshType::Point extents = max_point - min_point;
    float max_extent = std::max(std::max(extents[0], extents[1]), extents[2]);

    // Scale factor to fit in [-1, 1]
    float scale_factor = 2.0f / max_extent;

    // Apply scaling and translation to all vertices
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        MeshType::Point p = mesh.point(*v_it);
        p = (p - center) * scale_factor; // Scale and translate to center at origin
        mesh.set_point(*v_it, p); // Update vertex position
    }
}


std::string saveMeshToString(MeshType& mesh) {

    // mesh.request_vertex_normals();
    // mesh.update_normals();
    std::ostringstream objStream;
    OpenMesh::IO::Options options;

    // You can set options like requesting vertex normals or vertex colors if needed
    // options += OpenMesh::IO::Options::VertexNormal;
    // options += OpenMesh::IO::Options::VertexColor;
    options += OpenMesh::IO::Options::VertexTexCoord;

    // Write mesh to the ostringstream instead of a file
    if (!OpenMesh::IO::write_mesh(mesh, objStream, "obj", options)) {
        std::cerr << "Error: Cannot write mesh to string!" << std::endl;
        return "";
    }

    // Convert stream content to a string
    return objStream.str();
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int evaluatedPnsMesh(const unsigned char* data, int length, float *vertexBuffer, int *indexBuffer, float *uvBuffer, float *normalBuffer) {
        // Load the mesh from the string
        std::string str = std::string(data, data + length);
        std::istringstream objStream(str);
        MeshType mesh;
        OpenMesh::IO::Options options;
        if (!OpenMesh::IO::read_mesh(mesh, objStream, "obj", options)) {
            std::cerr << "Could not read mesh" << std::endl;
            return 0;
        }

        //create bv
        MeshType t_Mesh;
        t_Mesh.request_vertex_texcoords2D();
        PatchConsumer* t_Writer = new EvaluatedMeshWriter(&t_Mesh);

        // Convert mesh into Patches (contain BB-coefficients) and write patches into .bv file
        const bool t_IsDegRaise = false;
        process_mesh(mesh, t_Writer, t_IsDegRaise);

        delete t_Writer;
        std::cout << "Vertices: " << t_Mesh.n_vertices() << " Faces: " << t_Mesh.n_faces() << std::endl;
        // scaleAndTranslateMesh(t_Mesh);
        // std::string temp = saveMeshToString(t_Mesh);
        for (int i = 0; i < t_Mesh.n_vertices(); i++) {
            MeshType::Point p = t_Mesh.point(MeshType::VertexHandle(i));
            vertexBuffer[i * 3] = p[0];
            vertexBuffer[i * 3 + 1] = p[1];
            vertexBuffer[i * 3 + 2] = p[2];

            MeshType::Normal n = t_Mesh.normal(MeshType::VertexHandle(i));
            normalBuffer[i * 3] = n[0];
            normalBuffer[i * 3 + 1] = n[1];
            normalBuffer[i * 3 + 2] = n[2];

            MeshType::TexCoord2D uv = t_Mesh.texcoord2D(MeshType::VertexHandle(i));
            uvBuffer[i * 2] = uv[0];
            uvBuffer[i * 2 + 1] = uv[1];
        }

        for (int i = 0; i < t_Mesh.n_faces(); i++) {
            MeshType::FaceHandle fh = MeshType::FaceHandle(i);
            MeshType::FaceVertexIter fv_it = t_Mesh.fv_iter(fh);
            int j = 0;
            for (; fv_it.is_valid(); ++fv_it) {
                indexBuffer[i * 3 + j] = fv_it->idx();
                j++;
            }
        }
        return t_Mesh.n_faces() * 3;

    }
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int getBv(const unsigned char* data, int length) {


        std::string str = std::string(data, data + length);


        std::istringstream objStream(str);
        MeshType mesh;
        OpenMesh::IO::Options options;
        if (!OpenMesh::IO::read_mesh(mesh, objStream, "obj", options)) {
            std::cerr << "Could not read mesh" << std::endl;
            return 1;
        }


        PatchConsumer* t_Writer = new BVWriter("output.bv");

        // Convert mesh into Patches (contain BB-coefficients) and write patches into .bv file
        const bool t_IsDegRaise = false;
        process_mesh(mesh, t_Writer, t_IsDegRaise);
        delete t_Writer;

        return 0;

    }
}
extern "C" {
    EMSCRIPTEN_KEEPALIVE  
    int getigs(const unsigned char* data, int length) {
        std::string str = std::string(data, data + length);
        std::istringstream objStream(str);
        MeshType mesh;
        OpenMesh::IO::Options options;
        if (!OpenMesh::IO::read_mesh(mesh, objStream, "obj", options)) {
            std::cerr << "Could not read mesh" << std::endl;
            return 1;
        }
        PatchConsumer* t_Writer = new IGSWriter("output.igs");

        // Convert mesh into Patches (contain BB-coefficients) and write patches into .bv file
        const bool t_IsDegRaise = false;
        process_mesh(mesh, t_Writer, t_IsDegRaise);
        delete t_Writer;
        return 0;
    }
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE  
    int getstep(const unsigned char* data, int length) {
        std::string str = std::string(data, data + length);
        std::istringstream objStream(str);
        MeshType mesh;
        OpenMesh::IO::Options options;
        if (!OpenMesh::IO::read_mesh(mesh, objStream, "obj", options)) {
            std::cerr << "Could not read mesh" << std::endl;
            return 1;
        }
        PatchConsumer* t_Writer = new STEPWriter("output.step");

        // Convert mesh into Patches (contain BB-coefficients) and write patches into .bv file
        const bool t_IsDegRaise = false;
        process_mesh(mesh, t_Writer, t_IsDegRaise);
        delete t_Writer;
        return 0;
    }
}