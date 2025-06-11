/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

//  C++ std library includes
#include <iostream>
#include <string>

//  Open Mesh includes
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

//  Our own headers
#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "PatchConsumer/BVWriter.hpp"
#include "PatchConsumer/IGSWriter.hpp"
#include "PatchConsumer/STEPWriter.hpp"
#include "ProcessMesh.hpp"
#include "Helper/simple_arg.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

int main(int argc, char **argv)
{
    // Check correct number of input arguments
    SimpleArg p{argv[0]};
    p.add<bool>('d', "DEGREE_RAISE", "raise degree 2 patches to degree 3");
    p.add<std::string>('f', "FORMAT", "output format", false, "bv",
                       {"bv", "igs", "step"});
    p.addPositional<std::string>("input",   "input file");

    if (!p.parse(argc, argv) || p.help()) {
        if (p.errors()) std::cerr << p.errorMsg() << std::endl;
        p.printHelp();
        return p.errors() ? 1 : 0;
    }


    bool t_IsDegRaise = p.get<bool>("DEGREE_RAISE");
    const std::string t_InputFile = p.getPositional<std::string>(0);
    const std::string t_Format = p.get<std::string>("FORMAT");

    // Load mesh from .obj file
    MeshType t_Mesh;
    
    OpenMesh::IO::read_mesh(t_Mesh, t_InputFile);

    // Init .bv file writer
    const std::string t_FileName = "output." + t_Format;
    PatchConsumer* t_Writer;
    if (t_Format == "bv") {
        t_Writer = new BVWriter(t_FileName);
    } else if(t_Format == "igs") {
        t_Writer = new IGSWriter(t_FileName);
    } else{ // t_Format == "step"
        t_Writer = new STEPWriter(t_FileName);
    }

    // Convert mesh into Patches (contain BB-coefficients) and write patches into .bv file
    process_mesh(t_Mesh, t_Writer, t_IsDegRaise);

    delete t_Writer;

    return 0;
}
