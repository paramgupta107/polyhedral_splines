#define OM_STATIC_BUILD
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ProcessMesh.hpp"
#include "Helper/Helper.hpp"
#include "Patch/PatchBuilder.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "Patch/Patch.hpp"
#include "PatchConsumer/BVWriter.hpp"
#include "PatchConsumer/IGSWriter.hpp"

#include <OpenMesh/Core/IO/MeshIO.hh>

namespace py = pybind11;

static std::vector<std::vector<double>> matrix_to_list(const Matrix& M) {
    std::vector<std::vector<double>> out(M.getRows(), std::vector<double>(M.getCols()));
    for (int i = 0; i < M.getRows(); ++i)
        for (int j = 0; j < M.getCols(); ++j)
            out[i][j] = M(i,j);
    return out;
}

PYBIND11_MODULE(polyhedral_net_splines, m) {
    m.doc() = "Polyhedral Net Splines Python Bindings";

    py::class_<MeshType>(m, "Pns_control_mesh")
        .def(py::init<>())
        .def_static("from_data",
            [](py::list verts, py::list faces) {
                MeshType mesh;
                std::vector<MeshType::VertexHandle> vertex_handles;
                for (auto v: verts){
                    auto [x, y, z] = v.cast<std::tuple<double, double, double>>();
                    vertex_handles.push_back(mesh.add_vertex({x, y, z}));
                }
                for (auto f: faces){
                    std::vector<int> indices = f.cast<std::vector<int>>();
                    std::vector<MeshType::VertexHandle> face_handles;
                    for (int i: indices){
                        face_handles.push_back(vertex_handles[i]);
                    }
                    mesh.add_face(face_handles);
                }
                return mesh;
            }, "Create a Pns Control net from vertex and face data\n Args: verts, faces \n Returns: Pns_control_mesh")
        .def_static("from_file",
            [](const std::string& filename) {
                MeshType mesh;
                OpenMesh::IO::read_mesh(mesh, filename);
                return mesh;
            }, "Create a Pns Control net from a file\n Args: filename \n Returns: Pns_control_mesh")
        .def("set_vertex",
            [](MeshType& mesh, int index, double x, double y, double z) {
                auto vh = mesh.vertex_handle(index);
                mesh.set_point(vh, {x, y, z});
            }, "Set vertex position\n Args: index, x, y, z")
        .def("get_vertex",
            [](const MeshType& mesh, int index) {
                auto vh = mesh.vertex_handle(index);
                auto point = mesh.point(vh);
                return py::make_tuple(point[0], point[1], point[2]);
            }, "Get vertex position\n Args: index")
        .def("get_vertices",
            [](const MeshType& mesh) {
                std::vector<std::tuple<double, double, double>> verts;
                for (auto vh: mesh.vertices()){
                    auto point = mesh.point(vh);
                    verts.push_back(std::make_tuple(point[0], point[1], point[2]));
                }
                return verts;
            }, "Get all vertex positions\n Returns: list of vertex positions")
        .def("get_faces",
            [](const MeshType& mesh) {
                std::vector<std::vector<int>> faces;
                for (auto fh: mesh.faces()){
                    std::vector<int> face;
                    for (auto vh: mesh.fv_range(fh)){
                        face.push_back(vh.idx());
                    }
                    faces.push_back(face);
                }
                return faces;
            }, "Get all face indices\n Returns: list of face indices");
    
    py::class_<Patch>(m, "Patch")
        .def_property_readonly("deg_u", [](Patch& p){ return p.m_DegU; })
        .def_property_readonly("deg_v", [](Patch& p){ return p.m_DegV; })
        .def_property_readonly("group", [](Patch& p){ return p.m_Group; })
        .def_property_readonly("is_valid", &Patch::isValid)
        .def_property_readonly("bb_coefs",
            [](Patch& p){
            py::list rows;
            for(auto& row: p.m_BBcoefs) {
                py::list cols;
                for(auto& pt: row)
                cols.append(py::make_tuple(pt[0],pt[1],pt[2]));
                rows.append(cols);
            }
            return rows;
            })
        .def("degRaise",
            [](Patch& p) {
                p.degRaise();
            }, "Raise the degree of the patch");

    py::class_<PatchBuilder>(m, "PatchBuilder")
        .def_property_readonly("neighbor_verts", 
            [](PatchBuilder& pb) {
                std::vector<int> indices;
                for (auto vh: pb.getNeighborVerts()){
                    indices.push_back(vh.idx());
                }
                return indices;
            }, "Get the indices of the neighboring vertices\n Returns: list of vertex indices")
        .def_property_readonly("mask", 
            [](PatchBuilder& pb) {
                return matrix_to_list(pb.getMask());
            }, "Get the mask matrix\n Returns: mask matrix")
        .def_property_readonly("num_patches", 
            [](PatchBuilder& pb) {
                return pb.m_NumOfPatches;
            }, "Get the number of patches\n Returns: number of patches")
        .def_property_readonly("deg_u", 
            [](PatchBuilder& pb) {
                return pb.m_DegU;
            }, "Get the degree in u direction\n Returns: degree in u direction")
        .def_property_readonly("deg_v", 
            [](PatchBuilder& pb) {
                return pb.m_DegV;
            }, "Get the degree in v direction\n Returns: degree in v direction")
        .def("patch_type", 
            [](PatchBuilder& pb) {
                return pb.getPatchConstructor()->getGroupName();
            }, "Get the patch type\n Returns: patch type")
        .def("build_patches", 
            &PatchBuilder::buildPatches,
            py::arg("mesh"),
            "Build patches from the mesh\n Args: mesh\n Returns: list of patches");
    m.def("get_patch_builders",
            &getPatchBuilders,
            py::arg("mesh"),
            "Get patch builders from the mesh\n Args: mesh\n Returns: list of patch builders");
    py::class_<PatchConsumer>(m, "PatchConsumer")
        .def("start", &PatchConsumer::start, "Start the consumer")
        .def("stop", &PatchConsumer::stop, "Stop the consumer")
        .def("consume", &PatchConsumer::consume, "Consume a patch\n Args: patch");
    py::class_<BVWriter, PatchConsumer>(m, "BVWriter")
        .def(py::init<const std::string&>(), py::arg("filename"))
        .def("start", &BVWriter::start)
        .def("stop", &BVWriter::stop)
        .def("consume", &BVWriter::consume);
    py::class_<IGSWriter, PatchConsumer>(m, "IGSWriter")
        .def(py::init<const std::string&>(), py::arg("filename"))
        .def("start", &IGSWriter::start)
        .def("stop", &IGSWriter::stop)
        .def("consume", &IGSWriter::consume);
    m.def("process_mesh",
        &process_mesh,
        py::arg("Pns_control_mesh"),
        py::arg("PatchConsumer"),
        py::arg("is_deg_raise") = false,
        "Process the mesh and consume patches\n Args: Pns_control_mesh, PatchConsumer, is_deg_raise\n Returns: None");
}