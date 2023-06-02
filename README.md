

### cite as
<i>Bi-cubic splines for polyhedral control nets</i>,
Jörg Peters , Kyle Lo, Kestutis Karciauskas
TOMS , 2023
[paper](https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS.pdf)
[documentation](<https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS_Doc.pdf>)

# - Environment Setting

### Recommended Operating Systems
Linux (Ubuntu 20.04 LTS) & macOS (Catalina 10.15) & Windows 10

### Tools & Dependencies

[CMake](https://cmake.org/) (>=3.9)
[OpenMesh](<https://www.openmesh.org/>) (8.1)

**Note:**

* CMake will automatically download and install OpenMesh into `/Source/External` using the source with commit hash pointing to the tested version.
* The program should be compatible to equivalent or higher versions with little or no modification.

### Tested Environments
macOS: Apple clang++-11

ubuntu 18.04: g++-6 g++-7 g++-8

ubuntu 20.04: g++-9 g++-10 g++-11

Debian 9: g++-6

Debian 10: g++-7 g++-8

Debian 11: g++-9 g++-10 g++11

CentOS 8: g++-8

Windows 10: Visual Studio 2017


# - Building #
For UNIX-based system:
```shell
git clone https://kylelo@bitbucket.org/surflab/polyhedral_splines.git
cd polyhedral_splines
mkdir build
cd build
cmake ../Source
make
```

For Windows:

1. Launch `x86 Native Tools Command Prompt for VS 2017` with admin

2. Run the following commands

```
git clone https://kylelo@bitbucket.org/surflab/polyhedral_splines.git
cd polyhedral_splines
mkdir build
cd build
cmake ../Source
```

3. In the build folder find and launch `PolyhedralSplines.sln` with Visual Studio 2017 (or just double click on .sln file)

4. In Visual Studio 2017, set configuration to `Release` mode and switch platform to `Win32` then click `Build Solution`

5. `cd ~/build/` (WARNING: calling executable from other directory might fail.)


**Note:** macOS users need to make sure $PATH includes path to qt5 bin folder

# - Usage #
* Input:  quad-dominant mesh in .obj file format
* Output: BB-coefficients written in .bv file format.


Please find [BView file introduction](https://www.cise.ufl.edu/research/SurfLab/bview/#file-format) on UF CISE SurfLab website

## Execution
For UNIX-based system:
```shell
./PolyhedralSplines /path/to/filename.obj
```
Note: test .obj files are in `/testfile`.

User can add option `-d` or `--DEGREE_RAISE` to raise the degree of all patches to a uniform 3x3
```shell
./PolyhedralSplines -d /path/to/filename.obj
```

For Windows:
```
Release\PolyhedralSplines.exe \path\to\filename.obj
```

```
Release\PolyhedralSplines.exe -d \path\to\filename.obj
```

## View .bv file
Users can display .bv files by using either the online or the desktop version of [BView](https://www.cise.ufl.edu/research/SurfLab/bview/).
BView provides surface inspection tools such as zoom, groupwise patch color, Gaussian curvature, highlight lines and more.

Quick view of .bv file:

1. Go to [WebGL](https://www.cise.ufl.edu/research/SurfLab/bview/webgl3_new_2/)

2. Click on `Load File` > `Choose File` to import .bv file

3. Set `Display` > `Patch Detail` to 16x16
