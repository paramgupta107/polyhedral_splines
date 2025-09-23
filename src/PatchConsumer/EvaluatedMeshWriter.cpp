/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#include <assert.h>
#include <iomanip>
#include <thread>
#include <cmath>

#include "EvaluatedMeshWriter.hpp"


EvaluatedMeshWriter::EvaluatedMeshWriter(MeshType *a_Mesh){
    mesh = a_Mesh;
}
/*
 *  Calls WriteFromQueue in a new thread
 *  and then detaches that thread.
 */
void EvaluatedMeshWriter::start()
{
    /*
     *  Do nothing
     */
    return;
}

/*
 *  Signals to WriteFromQueue to stop once
 *  it has processed all the patches in
 *  the queue.
 */
void EvaluatedMeshWriter::stop()
{
    /*
     *  Do nothing
     */
    return;
}

/*
 *  Adds a patch to the queue in a
 *  thread-safe way.
 */
void EvaluatedMeshWriter::consume(const Patch a_Patch)
{
    create_bezier_surface_mesh(a_Patch, 16, mesh);
    return;
}

// Compute normal at (u,v) using De Casteljau's algorithm
MeshType::Point EvaluatedMeshWriter::de_casteljau_normal(float u, float v, const Patch& a_Patch) {
    int degu = a_Patch.m_DegU;
    int degv = a_Patch.m_DegV;

    // ---- Tangent in v-direction ----
    std::vector<MeshType::Point> temp_u(degu + 1);
    for (int i = 0; i <= degu; ++i) {
        std::vector<MeshType::Point> temp_v(degv + 1);
        for (int j = 0; j <= degv; ++j)
            temp_v[j] = a_Patch.m_BBcoefs[i][j];

        // De Casteljau up to penultimate layer
        for (int k = 1; k < degv; ++k)
            for (int j = 0; j <= degv - k; ++j)
                temp_v[j] = (1.0f - v) * temp_v[j] + v * temp_v[j + 1];

        // Derivative control points (scaled by degree)
        temp_u[i] = (float)degv * (temp_v[1] - temp_v[0]);
    }
    for (int k = 1; k <= degu; ++k)
        for (int i = 0; i <= degu - k; ++i)
            temp_u[i] = (1.0f - u) * temp_u[i] + u * temp_u[i + 1];
    MeshType::Point tangent_v = temp_u[0];

    // ---- Tangent in u-direction ----
    std::vector<MeshType::Point> temp_v2(degv + 1);
    for (int j = 0; j <= degv; ++j) {
        std::vector<MeshType::Point> temp_u2(degu + 1);
        for (int i = 0; i <= degu; ++i)
            temp_u2[i] = a_Patch.m_BBcoefs[i][j];

        for (int k = 1; k < degu; ++k)
            for (int i = 0; i <= degu - k; ++i)
                temp_u2[i] = (1.0f - u) * temp_u2[i] + u * temp_u2[i + 1];

        temp_v2[j] = (float)degu * (temp_u2[1] - temp_u2[0]);
    }
    for (int k = 1; k <= degv; ++k)
        for (int j = 0; j <= degv - k; ++j)
            temp_v2[j] = (1.0f - v) * temp_v2[j] + v * temp_v2[j + 1];
    MeshType::Point tangent_u = temp_v2[0];

    // ---- Normal = cross product of tangents ----
    MeshType::Point n = OpenMesh::cross(tangent_u, tangent_v);
    n.normalize();
    return n;
}


// De Casteljau's algorithm for Bézier surface evaluation
MeshType::Point EvaluatedMeshWriter::de_casteljau_surface(float u, float v, const Patch a_Patch) {
    int degu = a_Patch.m_DegU;
    int degv = a_Patch.m_DegV;
    // Temporary array to store intermediate results along the u direction
    std::vector<MeshType::Point> temp_u(degu + 1);

    // Apply De Casteljau's algorithm along the u direction first
    for (int i = 0; i <= degu; ++i) {
        // Apply De Casteljau's algorithm along the v direction for each fixed u
        std::vector<MeshType::Point> temp_v(degv + 1);
        for (int j = 0; j <= degv; ++j) {
            temp_v[j] = a_Patch.m_BBcoefs[i][j];
        }

        // Now reduce the control points in the v direction
        for (int k = 1; k <= degv; ++k) {
            for (int j = 0; j <= degv - k; ++j) {
                temp_v[j] = (1.0f - v) * temp_v[j] + v * temp_v[j + 1];
            }
        }

        temp_u[i] = temp_v[0];
    }

    // Now reduce the control points in the u direction
    for (int k = 1; k <= degu; ++k) {
        for (int i = 0; i <= degu - k; ++i) {
            temp_u[i] = (1.0f - u) * temp_u[i] + u * temp_u[i + 1];
        }
    }

    return temp_u[0];  // Final evaluated point on the surface
}

// Function to create a mesh from a Bézier surface
void EvaluatedMeshWriter::create_bezier_surface_mesh(const Patch a_Patch, int n, MeshType *mesh) {
    mesh->request_vertex_normals();
    std::vector<MeshType::VertexHandle> vhandles;
    std::vector<std::vector<MeshType::VertexHandle>> grid(n, std::vector<MeshType::VertexHandle>(n));
    std::vector<std::vector<MeshType::Point>> points(n, std::vector<MeshType::Point>(n));

    // Step 1: Evaluate the Bézier surface at each point on the n x n grid
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float u = static_cast<float>(i) / (n - 1);
            float v = static_cast<float>(j) / (n - 1);

            // Evaluate the point on the Bézier surface
            MeshType::Point p = de_casteljau_surface(u, v, a_Patch);
            points[i][j] = p;
            grid[i][j] = mesh->add_vertex(p);
            MeshType::TexCoord2D texcoord(u, v);
            mesh->set_texcoord2D(grid[i][j], texcoord);
            mesh->set_normal(grid[i][j], de_casteljau_normal(u, v, a_Patch));
        }
    }

    // Step 2: Compute normals using forward differences
    // for (int i = 0; i < n; ++i) {
    //     for (int j = 0; j < n; ++j) {
    //         MeshType::Point tangent_u(0.0, 0.0, 0.0);
    //         MeshType::Point tangent_v(0.0, 0.0, 0.0);

    //         // Forward difference for tangent in u-direction
    //         if (i < n - 1) {
    //             tangent_u = points[i + 1][j] - points[i][j];
    //         } else if (i > 0) {  // Backward difference for last row
    //             tangent_u = points[i][j] - points[i - 1][j];
    //         }

    //         // Forward difference for tangent in v-direction
    //         if (j < n - 1) {
    //             tangent_v = points[i][j + 1] - points[i][j];
    //         } else if (j > 0) {  // Backward difference for last column
    //             tangent_v = points[i][j] - points[i][j - 1];
    //         }

    //         // Compute the normal as the cross product of the tangents
    //         MeshType::Normal normal = tangent_u % tangent_v;
    //         normal.normalize(); // Normalize the normal

    //         // Assign the normal to the vertex
    //         mesh->set_normal(grid[i][j], normal);
    //     }
    // }

    // Step 3: Create faces from the grid points
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            std::vector<MeshType::VertexHandle> face_vhandles;

            // First triangle
            face_vhandles.push_back(grid[i][j]);
            face_vhandles.push_back(grid[i + 1][j]);
            face_vhandles.push_back(grid[i][j + 1]);
            mesh->add_face(face_vhandles);

            // Second triangle
            face_vhandles.clear();
            face_vhandles.push_back(grid[i + 1][j]);
            face_vhandles.push_back(grid[i + 1][j + 1]);
            face_vhandles.push_back(grid[i][j + 1]);
            mesh->add_face(face_vhandles);
        }
    }
}
