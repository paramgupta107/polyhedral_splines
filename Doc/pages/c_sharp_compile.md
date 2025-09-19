\page c_sharp_compile Building C# Library

Clone git repositry and run the following commands in the project directory.
```bash
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build
cmake --install build
dotnet build ./csharp
```
Then, copy both DLL files in ./csharp and the OpenMesh DLL into your project.