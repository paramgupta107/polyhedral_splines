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
#include "PatchConsumer/STEPWriter.hpp"

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
    m.doc() = R"pbdoc(
        Polyhedral-Net-Splines
        ---------------------
        Python bindings for constructing
        *Polyhedral Net Splines* (PNS) surfaces.
        )pbdoc";

    py::class_<MeshType>(m, "Pns_control_mesh", R"pbdoc(
        Editable polygonal control net.

        Internally wraps an *OpenMesh* ``PolyMesh_ArrayKernelT``.  
        Indices are **zero-based** everywhere.
        )pbdoc")
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
            }, R"pbdoc(
                Build a control mesh from raw data.

                Args:
                    verts (List[Tuple[float, float, float]]):
                        Cartesian vertex coordinates.
                    faces (List[List[int]]):
                        Each inner list stores vertex indices forming
                        a *single* face (arbitrary polygon).

                Returns:
                    Pns_control_mesh: Newly-constructed control net.
                )pbdoc")
        .def_static("from_file",
            [](const std::string& filename) {
                MeshType mesh;
                OpenMesh::IO::read_mesh(mesh, filename);
                return mesh;
            }, R"pbdoc(
                Load a control mesh from an **OBJ**, **OFF**, or any format
                supported by *OpenMesh::IO*.

                Args:
                    filename (str): Path to mesh file on disk.

                Returns:
                    Pns_control_mesh
                )pbdoc")
        .def("set_vertex",
            [](MeshType& mesh, int index, double x, double y, double z) {
                auto vh = mesh.vertex_handle(index);
                mesh.set_point(vh, {x, y, z});
            }, R"pbdoc(
                Move a single control point.

                Args:
                    index (int): Zero-based vertex index.
                    x (float): New *x* coordinate.
                    y (float): New *y* coordinate.
                    z (float): New *z* coordinate.

                Returns:
                    None
                )pbdoc")
        .def("get_vertex",
            [](const MeshType& mesh, int index) {
                auto vh = mesh.vertex_handle(index);
                auto point = mesh.point(vh);
                return py::make_tuple(point[0], point[1], point[2]);
            }, R"pbdoc(
                Fetch coordinates of a single control point.

                Args:
                    index (int): Zero-based vertex index.

                Returns:
                    Tuple[float, float, float]: ``(x, y, z)``
                )pbdoc")
        .def("get_vertices",
            [](const MeshType& mesh) {
                std::vector<std::tuple<double, double, double>> verts;
                for (auto vh: mesh.vertices()){
                    auto point = mesh.point(vh);
                    verts.push_back(std::make_tuple(point[0], point[1], point[2]));
                }
                return verts;
            }, R"pbdoc(
                Get *all* control-net vertices.

                Returns:
                    List[Tuple[float, float, float]]
                )pbdoc")
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
            }, R"pbdoc(
                Get polygonal face connectivity.

                Returns:
                    List[List[int]]: Each inner list stores vertex indices
                    of one face.
                )pbdoc");
    
    py::class_<Patch>(m, "Patch", R"pbdoc(
        Single Bézier patch produced by a ``PatchBuilder``.
    )pbdoc")
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
            }, R"pbdoc(
                Bernstein-Bézier control points.

                Returns:
                    List[List[Tuple[float, float, float]]]:
                        2-D array indexed as ``[i][j]``.
                )pbdoc")
        .def("degRaise",
            [](Patch& p) {
                p.degRaise();
            }, R"pbdoc(
                Elevate degree upto 3.

                Returns:
                    None
            )pbdoc");

    py::class_<PatchBuilder>(m, "PatchBuilder", R"pbdoc(
        Represents a Pns Patch. Based on the current control net vertex positions, generates one or more ``Patch``.
    )pbdoc")
        .def_property_readonly("neighbor_verts", 
            [](PatchBuilder& pb) {
                std::vector<int> indices;
                for (auto vh: pb.getNeighborVerts()){
                    indices.push_back(vh.idx());
                }
                return indices;
            }, R"pbdoc(
                Indices of Pns patch neighborhood that are use to generate patches.

                Returns:
                    List[int]
            )pbdoc")
        .def_property_readonly("mask", 
            [](PatchBuilder& pb) {
                return matrix_to_list(pb.getMask());
            }, R"pbdoc(
                Linear transform to generate Patches from neighborhood.

                Returns:
                    List[List[float]]
            )pbdoc")
        .def_property_readonly("num_patches", 
            [](PatchBuilder& pb) {
                return pb.m_NumOfPatches;
            }, "Number of patches generated by this builder.")
        .def_property_readonly("deg_u", 
            [](PatchBuilder& pb) {
                return pb.m_DegU;
            })
        .def_property_readonly("deg_v", 
            [](PatchBuilder& pb) {
                return pb.m_DegV;
            })
        .def("patch_type", 
            [](PatchBuilder& pb) {
                return pb.getPatchConstructor()->getGroupName();
            }, "Name of the patch type")
        .def("build_patches", 
            &PatchBuilder::buildPatches,
            py::arg("mesh"),
            R"pbdoc(
                Construct Bézier patches for the supplied control mesh.

                Args:
                    mesh (Pns_control_mesh)

                Returns:
                    List[Patch]
                )pbdoc")
        .def("degRaise",
            [](PatchBuilder& pb) {
                pb.degRaise();
            }, R"pbdoc(
                Elevate degree upto 3. Modifies the mask so that it generates patches of degree bi-3.

                Returns:
                    None
            )pbdoc");
    m.def("get_patch_builders",
            &getPatchBuilders,
            py::arg("mesh"),
            R"pbdoc(
            Creates a ``PatchBuilder`` for each Pns patch type that are found in the control mesh.

            Args:
                mesh (Pns_control_mesh)

            Returns:
                List[PatchBuilder]
        )pbdoc");
    py::class_<PatchConsumer>(m, "PatchConsumer", R"pbdoc(
        Abstract class for writing ``Patch`` objects.
    )pbdoc")
        .def("start", &PatchConsumer::start, "Start the consumer")
        .def("stop", &PatchConsumer::stop, "Stop the consumer")
        .def("consume", &PatchConsumer::consume, R"pbdoc(
                Writes a Patch.

                Args:
                    patch (Patch)

                Returns:
                    None
             )pbdoc");
    py::class_<BVWriter, PatchConsumer>(m, "BVWriter", R"pbdoc(
        *.bv* writer https://www.cise.ufl.edu/research/SurfLab/bview/#file-format.
    )pbdoc")
        .def(py::init<const std::string&>(), py::arg("filename"), "Args:\n    filename (str): Destination file.")
        .def("start", &BVWriter::start)
        .def("stop", &BVWriter::stop)
        .def("consume", &BVWriter::consume);
    py::class_<IGSWriter, PatchConsumer>(m, "IGSWriter", R"pbdoc(
        *.igs* (IGES) surface writer.
    )pbdoc")
        .def(py::init<const std::string&>(), py::arg("filename"), "Args:\n    filename (str): Destination file.")
        .def("start", &IGSWriter::start)
        .def("stop", &IGSWriter::stop)
        .def("consume", &IGSWriter::consume);
    py::class_<STEPWriter, PatchConsumer>(m, "STEPWriter", R"pbdoc(
        *.step* surface writer.
    )pbdoc")
        .def(py::init<const std::string&>(), py::arg("filename"), "Args:\n    filename (str): Destination file.")
        .def("start", &STEPWriter::start)
        .def("stop", &STEPWriter::stop)
        .def("consume", &STEPWriter::consume);
    m.def("process_mesh",
        &process_mesh,
        py::arg("Pns_control_mesh"),
        py::arg("PatchConsumer"),
        py::arg("is_deg_raise") = false,
        R"pbdoc(
            Constructs a Pns surface using a control net and writes it using a give consumer.

            Args:
                control_mesh (Pns_control_mesh):
                    Input control net.
                consumer (PatchConsumer):
                    Active consumer instance (e.g. ``BVWriter``).
                is_deg_raise (bool, optional):
                    Raise degree to 3. Default is ``False``.

            Returns:
                None
        )pbdoc");
}