/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

//  C++ std library includes
#include <iostream>
#include <string>
#include <filesystem>

//  Open Mesh includes
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

//  Our own headers
#include "Pool/Pool.hpp"
#include "PatchConsumer/PatchConsumer.hpp"
#include "PatchConsumer/BVWriter/Serial/BVWriterSerial.hpp"
#include "serial.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MeshType;

int main(int argc, char **argv)
{
    // Check correct number of input arguments
    if(argc < 2)
    {
        std::cout << "\nArgument error!\n";
        std::cout << "Correct usage:`./SemiStructuredSpline [OPTIONS] ${INPUT_FILENAME}`\n";
        std::cout << "Options: \n";
        std::cout << "-d --DEGREE_RAISE : raise deg 2 patches to deg 3. \n";
        return 1;
    }


    bool t_IsDegRaise = false;

    for (int i = 1; i < argc-1; i++)
    {
        if(std::string(argv[i]) == "--DEGREE_RAISE" || std::string(argv[i]) == "-d")
        {
            t_IsDegRaise = true;
        }
        else
        {
            std::cout << "Invalid arguments, please try again.\n";
        }
    }


    // Load mesh from .obj file
    MeshType t_Mesh;
    const std::string t_InputFile = argv[argc-1];
    OpenMesh::IO::read_mesh(t_Mesh, t_InputFile);

    // Init output .bv file
    const std::string t_FileName = std::filesystem::path(t_InputFile).stem().string() + ".bv";
    PatchConsumer* t_BVWriterSerial= new BVWriterSerial(t_FileName);

    // Generate BB patches and write BB coef. to .bv file
    serial::process_mesh(t_Mesh, t_BVWriterSerial, t_IsDegRaise);

    return 0;
}
