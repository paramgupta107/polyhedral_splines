using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace PolyhedralNetSplines
{
    /// <summary>
    /// Editable polygonal control net.
    /// </summary>
    public class PNSControlMesh : IDisposable
    {
        internal IntPtr Handle { get; private set; }
        private bool disposed = false;

        public PNSControlMesh()
        {
            Handle = MeshCreate_Interop();
        }

        ~PNSControlMesh()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                MeshDestroy_Interop(Handle);
                Handle = IntPtr.Zero;
                disposed = true;
            }
        }

        /// <summary>
        /// Load a control mesh from an OBJ, OFF, or any format supported by OpenMesh::IO.
        /// </summary>
        /// <param name="filename">Path to mesh file on disk</param>
        /// <returns>Newly-constructed control net</returns>
        public static PNSControlMesh FromFile(string filename)
        {
            IntPtr meshHandle = MeshFromFile_Interop(filename);
            if (meshHandle == IntPtr.Zero)
                throw new Exception($"Failed to load mesh from file: {filename}");
                
            var mesh = new PNSControlMesh();
            mesh.Handle = meshHandle;
            return mesh;
        }

        /// <summary>
        /// Build a control mesh from raw data.
        /// </summary>
        /// <param name="vertices">Cartesian vertex coordinates</param>
        /// <param name="faces">Each inner array stores vertex indices forming a single face</param>
        /// <returns>Newly-constructed control net</returns>
        public static PNSControlMesh FromData(float[,] vertices, int[][] faces)
        {
            int vertexCount = vertices.GetLength(0);
            
            // Flatten vertices for native call
            float[] flatVertices = new float[vertexCount * 3];
            for (int i = 0; i < vertexCount; i++)
            {
                flatVertices[i*3] = vertices[i, 0];
                flatVertices[i*3+1] = vertices[i, 1];
                flatVertices[i*3+2] = vertices[i, 2];
            }

            // Prepare face data for native call
            int[] faceSizes = new int[faces.Length];
            int totalIndices = 0;
            for (int i = 0; i < faces.Length; i++)
            {
                faceSizes[i] = faces[i].Length;
                totalIndices += faces[i].Length;
            }

            int[] flatFaces = new int[totalIndices];
            int index = 0;
            for (int i = 0; i < faces.Length; i++)
            {
                for (int j = 0; j < faces[i].Length; j++)
                {
                    flatFaces[index++] = faces[i][j];
                }
            }

            IntPtr meshHandle = MeshFromData_Interop(
                flatVertices, vertexCount, 
                flatFaces, faceSizes, faces.Length);
                
            var mesh = new PNSControlMesh();
            mesh.Handle = meshHandle;
            return mesh;
        }

        /// <summary>
        /// Move a single control point.
        /// </summary>
        /// <param name="index">Zero-based vertex index</param>
        /// <param name="x">New x coordinate</param>
        /// <param name="y">New y coordinate</param>
        /// <param name="z">New z coordinate</param>
        public void SetVertex(int index, float x, float y, float z)
        {
            MeshSetVertex_Interop(Handle, index, x, y, z);
        }

        /// <summary>
        /// Fetch coordinates of a single control point.
        /// </summary>
        /// <param name="index">Zero-based vertex index</param>
        /// <returns>Tuple containing (x, y, z) coordinates</returns>
        public (float X, float Y, float Z) GetVertex(int index)
        {
            float[] coords = new float[3];
            MeshGetVertex_Interop(Handle, index, coords);
            return (coords[0], coords[1], coords[2]);
        }

        /// <summary>
        /// Get all control-net vertices.
        /// </summary>
        /// <returns>Array of vertex coordinates</returns>
        public (float X, float Y, float Z)[] GetVertices()
        {
            int count = MeshGetVertexCount_Interop(Handle);
            if (count <= 0) return Array.Empty<(float, float, float)>();
            
            float[] coords = new float[count * 3];
            MeshGetVertices_Interop(Handle, coords, count);
            
            var result = new (float, float, float)[count];
            for (int i = 0; i < count; i++)
            {
                result[i] = (coords[i*3], coords[i*3+1], coords[i*3+2]);
            }
            return result;
        }

        /// <summary>
        /// Get polygonal face connectivity.
        /// </summary>
        /// <returns>Array where each inner array stores vertex indices of one face</returns>
        public int[][] GetFaces()
        {
            int faceCount = MeshGetFaceCount_Interop(Handle);
            if (faceCount <= 0) return Array.Empty<int[]>();
            
            int[] faceSizes = new int[faceCount];
            MeshGetFaceSizes_Interop(Handle, faceSizes, faceCount);
            
            int totalIndices = 0;
            for (int i = 0; i < faceCount; i++) totalIndices += faceSizes[i];
            
            int[] allIndices = new int[totalIndices];
            MeshGetFaceIndices_Interop(Handle, allIndices, totalIndices);
            
            int[][] result = new int[faceCount][];
            int currentIndex = 0;
            
            for (int i = 0; i < faceCount; i++)
            {
                int size = faceSizes[i];
                result[i] = new int[size];
                for (int j = 0; j < size; j++)
                {
                    result[i][j] = allIndices[currentIndex++];
                }
            }
            
            return result;
        }
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr MeshCreate_Interop();
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshDestroy_Interop(IntPtr mesh);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr MeshFromFile_Interop(string filename);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr MeshFromData_Interop(float[] vertices, int vertexCount, int[] faceIndices, int[] faceSizes, int faceCount);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshSetVertex_Interop(IntPtr mesh, int index, float x, float y, float z);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshGetVertex_Interop(IntPtr mesh, int index, [Out] float[] coords);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int MeshGetVertexCount_Interop(IntPtr mesh);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshGetVertices_Interop(IntPtr mesh, [Out] float[] coords, int count);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int MeshGetFaceCount_Interop(IntPtr mesh);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshGetFaceSizes_Interop(IntPtr mesh, [Out] int[] sizes, int count);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void MeshGetFaceIndices_Interop(IntPtr mesh, [Out] int[] indices, int totalCount);
    }

    /// <summary>
    /// Represents a PNS Patch. Based on the current control net vertex positions, generates one or more Patches.
    /// </summary>
    public class PatchBuilder : IDisposable
    {
        internal IntPtr Handle { get; private set; }
        private bool disposed = false;

        internal PatchBuilder(IntPtr handle)
        {
            Handle = handle;
        }

        ~PatchBuilder()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                PatchBuilderDestroy_Interop(Handle);
                Handle = IntPtr.Zero;
                disposed = true;
            }
        }

        /// <summary>
        /// Indices of PNS patch neighborhood that are used to generate patches.
        /// </summary>
        public int[] NeighborVerts
        {
            get
            {
                int count = PatchBuilderGetNeighborVertCount_Interop(Handle);
                int[] indices = new int[count];
                PatchBuilderGetNeighborVerts_Interop(Handle, indices, count);
                return indices;
            }
        }

        /// <summary>
        /// Linear transform to generate Patches from neighborhood.
        /// </summary>
        public float[,] Mask
        {
            get
            {
                int rows = PatchBuilderGetMaskRows_Interop(Handle);
                int cols = PatchBuilderGetMaskCols_Interop(Handle);
                float[] flatMask = new float[rows * cols];
                PatchBuilderGetMask_Interop(Handle, flatMask, rows * cols);
                
                float[,] mask = new float[rows, cols];
                for (int i = 0; i < rows; i++)
                {
                    for (int j = 0; j < cols; j++)
                    {
                        mask[i, j] = flatMask[i * cols + j];
                    }
                }
                return mask;
            }
        }

        /// <summary>
        /// Number of patches generated by this builder.
        /// </summary>
        public int NumPatches => PatchBuilderGetNumPatches_Interop(Handle);

        /// <summary>
        /// The U-degree of generated patches.
        /// </summary>
        public int DegU => PatchBuilderGetDegU_Interop(Handle);

        /// <summary>
        /// The V-degree of generated patches.
        /// </summary>
        public int DegV => PatchBuilderGetDegV_Interop(Handle);

        /// <summary>
        /// Name of the patch type
        /// </summary>
        public string PatchType
        {
            get
            {
                IntPtr typePtr = PatchBuilderGetPatchType_Interop(Handle);
                string type = Marshal.PtrToStringAnsi(typePtr);
                return type ?? string.Empty;
            }
        }

        /// <summary>
        /// Construct Bézier patches for the supplied control mesh.
        /// </summary>
        /// <param name="mesh">Control mesh</param>
        /// <returns>List of Patches</returns>
        public List<Patch> BuildPatches(PNSControlMesh mesh)
        {
            if (mesh == null) throw new ArgumentNullException(nameof(mesh));

            int count = PatchBuilderGetPatchCount_Interop(Handle, mesh.Handle);
            IntPtr[] patchHandles = new IntPtr[count];
            IntPtr patchArray = Marshal.AllocHGlobal(IntPtr.Size * count);
            
            try
            {
                PatchBuilderBuildPatches_Interop(Handle, mesh.Handle, patchArray);
                
                for (int i = 0; i < count; i++)
                {
                    patchHandles[i] = Marshal.ReadIntPtr(patchArray, i * IntPtr.Size);
                }
                
                List<Patch> patches = new List<Patch>();
                foreach (IntPtr patchHandle in patchHandles)
                {
                    patches.Add(new Patch(patchHandle));
                }
                
                return patches;
            }
            finally
            {
                Marshal.FreeHGlobal(patchArray);
            }
        }
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchBuilderDestroy_Interop(IntPtr builder);

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetNeighborVertCount_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchBuilderGetNeighborVerts_Interop(IntPtr builder, [Out] int[] indices, int count);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetMaskRows_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetMaskCols_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchBuilderGetMask_Interop(IntPtr builder, [Out] float[] mask, int size);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetNumPatches_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetDegU_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetDegV_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr PatchBuilderGetPatchType_Interop(IntPtr builder);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchBuilderGetPatchCount_Interop(IntPtr builder, IntPtr mesh);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchBuilderBuildPatches_Interop(IntPtr builder, IntPtr mesh, IntPtr patchArray);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr PatchBuilder_Clone_Interop(IntPtr builder);
    }


    /// <summary>
    /// Single Bézier patch produced by a PatchBuilder.
    /// </summary>
    public class Patch : IDisposable
    {
        internal IntPtr Handle { get; private set; }
        private bool disposed = false;

        internal Patch(IntPtr handle)
        {
            Handle = handle;
        }

        ~Patch()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                PatchDestroy_Interop(Handle);
                Handle = IntPtr.Zero;
                disposed = true;
            }
        }

        /// <summary>
        /// The U-degree of the patch.
        /// </summary>
        public int DegU => PatchGetDegU_Interop(Handle);

        /// <summary>
        /// The V-degree of the patch.
        /// </summary>
        public int DegV => PatchGetDegV_Interop(Handle);

        /// <summary>
        /// The group name of the patch.
        /// </summary>
        public string Group
        {
            get
            {
                IntPtr groupPtr = PatchGetGroup_Interop(Handle);
                string group = Marshal.PtrToStringAnsi(groupPtr);
                return group ?? string.Empty;
            }
        }

        /// <summary>
        /// Whether the patch is valid.
        /// </summary>
        public bool IsValid => PatchIsValid_Interop(Handle);

        /// <summary>
        /// Bernstein-Bézier control points.
        /// </summary>
        /// <returns>2D array of control points indexed as [i][j]</returns>
        public (float X, float Y, float Z)[,] GetBBCoefs()
        {
            int degU = DegU;
            int degV = DegV;
            int rows = degU + 1;
            int cols = degV + 1;
            float[] flatCoefs = new float[rows * cols * 3];
            
            PatchGetBBCoefs_Interop(Handle, flatCoefs, rows * cols * 3);
            
            var coefs = new (float, float, float)[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    int baseIndex = (i * cols + j) * 3;
                    coefs[i, j] = (
                        flatCoefs[baseIndex],
                        flatCoefs[baseIndex + 1],
                        flatCoefs[baseIndex + 2]
                    );
                }
            }
            
            return coefs;
        }

        /// <summary>
        /// Elevate degree up to 3.
        /// </summary>
        public void DegRaise()
        {
            PatchDegRaise_Interop(Handle);
        }
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchDestroy_Interop(IntPtr patch);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchGetDegU_Interop(IntPtr patch);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int PatchGetDegV_Interop(IntPtr patch);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr PatchGetGroup_Interop(IntPtr patch);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool PatchIsValid_Interop(IntPtr patch);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchGetBBCoefs_Interop(IntPtr patch, [Out] float[] coefs, int size);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchDegRaise_Interop(IntPtr patch);
    }


    /// <summary>
    /// Abstract class for writing Patch objects.
    /// </summary>
    public abstract class PatchConsumer : IDisposable
    {
        internal IntPtr Handle { get; private set; }
        private bool disposed = false;
        
        protected PatchConsumer(IntPtr handle)
        {
            Handle = handle;
        }

        ~PatchConsumer()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (Handle != IntPtr.Zero)
                {
                    PatchConsumerDestroy_Interop(Handle);
                    Handle = IntPtr.Zero;
                }
                disposed = true;
            }
        }

        /// <summary>
        /// Start the consumer.
        /// </summary>
        public void Start()
        {
            PatchConsumerStart_Interop(Handle);
        }

        /// <summary>
        /// Stop the consumer.
        /// </summary>
        public void Stop()
        {
            PatchConsumerStop_Interop(Handle);
        }

        /// <summary>
        /// Write a patch.
        /// </summary>
        /// <param name="patch">The patch to write</param>
        public void Consume(Patch patch)
        {
            if (patch == null) throw new ArgumentNullException(nameof(patch));
            PatchConsumerConsume_Interop(Handle, patch.Handle);
        }
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchConsumerDestroy_Interop(IntPtr consumer);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchConsumerStart_Interop(IntPtr consumer);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchConsumerStop_Interop(IntPtr consumer);
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PatchConsumerConsume_Interop(IntPtr consumer, IntPtr patch);
    }
    
    /// <summary>
    /// .bv writer
    /// </summary>
    public class BVWriter : PatchConsumer
    {
        public BVWriter(string filename) : base(BVWriterCreate_Interop(filename))
        {
            if (Handle == IntPtr.Zero)
                throw new Exception($"Failed to create BVWriter for file: {filename}");
        }

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr BVWriterCreate_Interop(string filename);
    }
    
    /// <summary>
    /// .igs (IGES) surface writer
    /// </summary>
    public class IGSWriter : PatchConsumer
    {
        public IGSWriter(string filename) : base(IGSWriterCreate_Interop(filename))
        {
            if (Handle == IntPtr.Zero)
                throw new Exception($"Failed to create IGSWriter for file: {filename}");
        }
        
        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr IGSWriterCreate_Interop(string filename);
    }

    /// <summary>
    /// .step surface writer
    /// </summary>
    public class STEPWriter : PatchConsumer
    {
        public STEPWriter(string filename) : base(STEPWriterCreate_Interop(filename))
        {
            if (Handle == IntPtr.Zero)
                throw new Exception($"Failed to create STEPWriter for file: {filename}");
        }

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr STEPWriterCreate_Interop(string filename);
    }



    
    public static class PNS
    {
        /// <summary>
        /// Constructs a PNS surface using a control net and writes it using the given consumer.
        /// </summary>
        /// <param name="controlMesh">Input control net</param>
        /// <param name="consumer">Active consumer instance (e.g., BVWriter)</param>
        /// <param name="degreeRaise">Raise degree to 3. Defaults to false</param>
        public static void ProcessMesh(PNSControlMesh controlMesh, PatchConsumer consumer, bool degreeRaise = false)
        {
            if (controlMesh == null)
                throw new ArgumentNullException(nameof(controlMesh));
            if (consumer == null)
                throw new ArgumentNullException(nameof(consumer));

            ProcessMesh_Interop(controlMesh.Handle, consumer.Handle, degreeRaise);
        }

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ProcessMesh_Interop(IntPtr mesh, IntPtr consumer, bool degreeRaise);


        /// <summary>
        /// Creates a PatchBuilder for each PNS patch type that are found in the control mesh.
        /// </summary>
        /// <param name="mesh">Input control net</param>
        /// <returns>List of patch builders</returns>
        public static List<PatchBuilder> GetPatchBuilders(PNSControlMesh mesh)
        {
            if (mesh == null) throw new ArgumentNullException(nameof(mesh));

            int count = GetPatchBuildersCount_Interop(mesh.Handle);
            IntPtr[] builderHandles = new IntPtr[count];
            IntPtr handleArray = Marshal.AllocHGlobal(IntPtr.Size * count);

            try
            {
                GetPatchBuilders_Interop(mesh.Handle, handleArray, count);

                for (int i = 0; i < count; i++)
                {
                    builderHandles[i] = Marshal.ReadIntPtr(handleArray, i * IntPtr.Size);
                }

                List<PatchBuilder> builders = new List<PatchBuilder>(count);
                foreach (IntPtr handle in builderHandles)
                {
                    builders.Add(new PatchBuilder(PatchBuilder.PatchBuilder_Clone_Interop(handle)));
                }

                return builders;
            }
            finally
            {
                Marshal.FreeHGlobal(handleArray);
            }
        }

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetPatchBuildersCount_Interop(IntPtr mesh);

        [DllImport("PolyhedralSplinesLib", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetPatchBuilders_Interop(IntPtr mesh, IntPtr builders, int count);

        /// <summary>
        /// Generate a BV file from a mesh file.
        /// </summary>
        /// <param name="controlMeshFile">Path to the control net</param>
        /// <param name="outputFile">Destination .bv filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Defaults to false</param>
        public static void CreateBVFromFile(string controlMeshFile, string outputFile = "output.bv", bool degreeRaise = false)
        {
            PNSControlMesh mesh = PNSControlMesh.FromFile(controlMeshFile);
            using (var writer = new BVWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(mesh, writer, degreeRaise);
                writer.Stop();
            }
        }

        /// <summary>
        /// Generate an IGS file from a mesh file.
        /// </summary>
        /// <param name="controlMeshFile">Path to the control net</param>
        /// <param name="outputFile">Destination .igs filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Defaults to false</param>
        public static void CreateIGSFromFile(string controlMeshFile, string outputFile = "output.igs", bool degreeRaise = false)
        {
            PNSControlMesh mesh = PNSControlMesh.FromFile(controlMeshFile);
            using (var writer = new IGSWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(mesh, writer, degreeRaise);
                writer.Stop();
            }
        }

        /// <summary>
        /// Generate an STEP file from a mesh file.
        /// </summary>
        /// <param name="controlMeshFile">Path to the control net</param>
        /// <param name="outputFile">Destination .step filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Defaults to false</param>
        public static void CreateSTEPFromFile(string controlMeshFile, string outputFile = "output.step", bool degreeRaise = false)
        {
            PNSControlMesh mesh = PNSControlMesh.FromFile(controlMeshFile);
            using (var writer = new STEPWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(mesh, writer, degreeRaise);
                writer.Stop();
            }
        }

        /// <summary> 
        /// Write a BV file from a control mesh.
        /// </summary>
        /// <param name="controlMesh">Control Net</param>
        /// <param name="outputFile">Destination .bv filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Default is false</param>
        public static void CreateBV(PNSControlMesh controlMesh, string outputFile = "output.bv", bool degreeRaise = false)
        {
            if (controlMesh == null)
                throw new ArgumentNullException(nameof(controlMesh));

            using (var writer = new BVWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(controlMesh, writer, degreeRaise);
                writer.Stop();
            }
        }

        /// <summary>
        /// Write an IGS file from a control mesh.
        /// </summary>
        /// <param name="controlMesh">Control Net</param>
        /// <param name="outputFile">Destination .igs filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Default is false</param>
        public static void CreateIGS(PNSControlMesh controlMesh, string outputFile = "output.igs", bool degreeRaise = false)
        {
            if (controlMesh == null)
                throw new ArgumentNullException(nameof(controlMesh));

            using (var writer = new IGSWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(controlMesh, writer, degreeRaise);
                writer.Stop();
            }
        }

        /// <summary>
        /// Write an STEP file from a control mesh.
        /// </summary>
        /// <param name="controlMesh">Control Net</param>
        /// <param name="outputFile">Destination .step filename</param>
        /// <param name="degreeRaise">Raise degree to 3. Default is false</param>
        public static void CreateSTEP(PNSControlMesh controlMesh, string outputFile = "output.step", bool degreeRaise = false)
        {
            if (controlMesh == null)
                throw new ArgumentNullException(nameof(controlMesh));

            using (var writer = new STEPWriter(outputFile))
            {
                writer.Start();
                ProcessMesh(controlMesh, writer, degreeRaise);
                writer.Stop();
            }
        }

    }
}