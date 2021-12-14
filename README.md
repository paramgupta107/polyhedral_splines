# - Environment Setting

### Recommended Operating Systems
Linux (Ubuntu 20.04 LTS) & macOS (Catalina 10.15) & Windows 10

### Dependencies

[CMake](https://cmake.org/) (>=3.9)
[OpenMesh](<https://www.openmesh.org/>) (8.1)

### Tested Environments
macOS: Apple clang++-11

ubuntu 18.04: g++-6 g++-7 g++-8

ubuntu 20.04: g++-9 g++-10 g++-11

Debian 9: g++-6

Debian 10: g++-7 g++-8

Debian 11: g++-9 g++-10 g++11

CentOS 8: g++-8

Windows 10: Visual Studio 2017

**Note:**

* CMake will automatically download and install OpenMesh into `/Source/External` using the source with commit hash pointing to the tested version.
* The program should be compatible to equivalent or higher versions with little or no modification.


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

1. Launch `x86 Native Tools Command Prompt for VS 2017`

2. Run commands for UNIX-based system except for `make`

3. Launch `PolyhedralSplines.sln` with Visual Studio 2017

4. Set configuration to `Release` mode and switch platform to `Win32`

5. Click `Build Solution`


**Note:** macOS users need to make sure $PATH includes path to qt5 bin folder

# - Usages #
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
```shell
PolyhedralSplines.exe /path/to/filename.obj
```

```shell
PolyhedralSplines.exe -d /path/to/filename.obj
```

## View .bv file
Users can display .bv files by using either the online or the desktop version of [BView](https://www.cise.ufl.edu/research/SurfLab/bview/).
BView provides surface inspection tools such as patch coloring, Gaussian curvature, highlight lines and more.
