# - Environment Setting

### Operating systems
Linux (Ubuntu 20.04 LTS) & macOS (Catalina 10.15)

### Dependencies

[Cmake](https://cmake.org/) (3.16.3)
[Eigen](<https://eigen.tuxfamily.org/>) (3.3)
[OpenMesh](<https://www.openmesh.org/>) (8.1)

### Compilers
g++ (9.3.0) & Apple clang++ (11.0)


**Note:**

* CMake will automatically download and install Eigen and OpenMesh into `/Source/External` using the source with commit hash pointing to the tested version.
* Succesfully tested version are listed in parentheses. The program should be compatible to equivalent or higher versions with little or no modification.


# - Building #

```shell
git clone https://bitbucket.org/surflab/semi_structured_spline/src/toms/
cd ~/semi_structured_spline
mkdir build
cd build
cmake ../Source
make
```

**Note:** macOS users need to make sure $PATH includes path to qt5 bin folder

# - Usages #
* Input:  mesh (with semi-structured layout) in .obj file format
* Output: BB-coefficients written in .bv file format.


Please find [BView file introduction](https://www.cise.ufl.edu/research/SurfLab/bview/#file-format) on UF CISE SurfLab website

## Execution

```shell
./semi_structured_spline /path/to/filename.obj
```

## Test multiple object files
Run the bash file to automatically generate .bv file for all .obj files in `/testfile` folder

```shell
cd build
./testAllFiles.sh
```

**Note:** makefile will copy `testAllFiles.sh` into `/build` after compilation. And the output .bv files will be stored in auto generated `/output`

## View .bv file
Users can display .bv files by using either the online or the desktop version of [BView](https://www.cise.ufl.edu/research/SurfLab/bview/).
BView provides surface inspection tools such as patch coloring, Gaussian curvature, highlight lines and more.
