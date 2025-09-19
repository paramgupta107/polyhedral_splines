/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 

/** 
 * \defgroup utility Utility
 * @brief General helper utilities for mesh processing.
 */

/** 
 * \defgroup halfedge_helper HalfEdge Operations
 * \ingroup utility
 * @brief Functions for operating on half-edge data structures.
 */

#include "HalfedgeOperation.hpp"
#include "Helper.hpp"

namespace HalfedgeOperation
{

/**
 * \ingroup halfedge_helper
 * @brief Halfedge operation to get one vert. Will return the last vertex handle if multiple get commands are given to get.
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from. This will be modified by the function.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @return The vertex handle obtained after executing the commands.
 * @note This function changes the input halfedge
 */
VertexHandle get_vert(const MeshType& a_Mesh, HalfedgeHandle& a_HalfedgeHandle, const std::vector<int> a_Commands)
{
    VertexHandle t_VertexHandle;
    for(auto CIt=a_Commands.begin(); CIt!=a_Commands.end(); ++CIt)
    {
        switch(*CIt){
            case 1:
                a_HalfedgeHandle = a_Mesh.next_halfedge_handle(a_HalfedgeHandle);
                break;
            case 2:
                a_HalfedgeHandle = a_Mesh.prev_halfedge_handle(a_HalfedgeHandle);
                break;
            case 3:
                a_HalfedgeHandle = a_Mesh.opposite_halfedge_handle(a_HalfedgeHandle);
                break;
            case 4:
                t_VertexHandle = a_Mesh.from_vertex_handle(a_HalfedgeHandle);
                break;
        }
    }
    return t_VertexHandle;
}

/**
 * \ingroup halfedge_helper
 * @brief Halfedge operation to get one vert
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from. This will be modified by the function.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @return The vertex handle obtained after executing the commands.
 * @note This function will not change the input halfedge
 */
VertexHandle get_vert_fixed_halfedge(const MeshType& a_Mesh, HalfedgeHandle a_HalfedgeHandle, const std::vector<int> a_Commands)
{
    return get_vert(a_Mesh, a_HalfedgeHandle, a_Commands);
}

/**
 * \ingroup halfedge_helper
 * @brief Halfedge operation to get multiple vertices.
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from. This will be modified by the function.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @note This function will changle the input halfedge
 */
std::vector<VertexHandle> get_verts(const MeshType& a_Mesh, HalfedgeHandle& a_HalfedgeHandle, const std::vector<int> a_Commands)
{
    std::vector<VertexHandle> t_VertexHandles;
    for(auto CIt=a_Commands.begin(); CIt!=a_Commands.end(); ++CIt)
    {
        switch(*CIt){
            case 1:
                a_HalfedgeHandle = a_Mesh.next_halfedge_handle(a_HalfedgeHandle);
                break;
            case 2:
                a_HalfedgeHandle = a_Mesh.prev_halfedge_handle(a_HalfedgeHandle);
                break;
            case 3:
                a_HalfedgeHandle = a_Mesh.opposite_halfedge_handle(a_HalfedgeHandle);
                break;
            case 4:
                t_VertexHandles.push_back(a_Mesh.from_vertex_handle(a_HalfedgeHandle));
        }
    }
    return t_VertexHandles;
}

/**
 * \ingroup halfedge_helper
 * @brief Halfedge operation to get multiple vertices.
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from. This will be modified by the function.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @note This function will not changle the input halfedge
 */
std::vector<VertexHandle> get_verts_fixed_halfedge(const MeshType& a_Mesh, HalfedgeHandle a_HalfedgeHandle, const std::vector<int> a_Commands)
{
    return get_verts(a_Mesh, a_HalfedgeHandle, a_Commands);
}

/**
 * \ingroup halfedge_helper
 * @brief Halfedge operation to get one face.
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 5=get from face.
 * @note This function changes the input halfedge
 */
FaceHandle get_face(const MeshType& a_Mesh, HalfedgeHandle& a_HalfedgeHandle, const std::vector<int> a_Commands)
{
    FaceHandle t_FaceHandle;
    for(auto CIt=a_Commands.begin(); CIt!=a_Commands.end(); ++CIt)
    {
        switch(*CIt){
            case 1:
                a_HalfedgeHandle = a_Mesh.next_halfedge_handle(a_HalfedgeHandle);
                break;
            case 2:
                a_HalfedgeHandle = a_Mesh.prev_halfedge_handle(a_HalfedgeHandle);
                break;
            case 3:
                a_HalfedgeHandle = a_Mesh.opposite_halfedge_handle(a_HalfedgeHandle);
                break;
            case 5:
                t_FaceHandle = a_Mesh.face_handle(a_HalfedgeHandle);
                break;
        }
    }
    return t_FaceHandle;
}


/**
 * \ingroup halfedge_helper
 * @brief Fill the TargetVertexHandles vector by given the index ordering and commands. The same commands will run `a_OrderOfVertIdx.size()` times.
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from.
 * @param a_TargetVertexHandles The target vector to be filled.
 * @param a_OrderOfVertIdx A vector of vector of indices to indicate the target index in a_TargetVertexHandles. Each inner vector indicates one set of indices for one run of commands.
 * @param a_CommandForOneSet A vector of commands to execute for one set of indices.
 * Comands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @note The input halfedge will not be changed after this function. However, the halfedge will be changed in between each run of commands.
 */
void init_verts(const MeshType& a_Mesh, HalfedgeHandle a_HalfedgeHandle, std::vector<VertexHandle>& a_TargetVertexHandles,
                const std::vector<std::vector<int>>& a_OrderOfVertIdx, const std::vector<int>& a_CommandForOneSet)
{
    for(auto vertIdSetIt=a_OrderOfVertIdx.begin(); vertIdSetIt!=a_OrderOfVertIdx.end(); ++vertIdSetIt)
    {
        auto t_SetOfVectHandles = get_verts(a_Mesh, a_HalfedgeHandle, a_CommandForOneSet);
        for(int i=0; i<t_SetOfVectHandles.size(); i++)
        {
            a_TargetVertexHandles[vertIdSetIt->at(i)] = t_SetOfVectHandles[i];
        }
    }
}

/**
 * \ingroup halfedge_helper
 * @brief Fill the TargetVertexHandles vector by given the index ordering and commands
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from.
 * @param a_TargetVertexHandles The target vector to be filled.
 * @param a_OrderOfVertIdx A vector of indices to indicate the target index in a_TargetVertexHandles.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @note This function will change the input halfedge!
 */
void init_verts(const MeshType& a_Mesh, HalfedgeHandle& a_HalfedgeHandle, std::vector<VertexHandle>& a_TargetVertexHandles,
                const std::vector<int>& a_OrderOfVertIdx, const std::vector<int>& a_Commands)
{
    auto t_VertexHandles = get_verts(a_Mesh, a_HalfedgeHandle, a_Commands);
    for(int i=0; i<t_VertexHandles.size(); i++)
    {
        a_TargetVertexHandles[a_OrderOfVertIdx[i]] = t_VertexHandles[i];
    }
}

/**
 * \ingroup halfedge_helper
 * @brief Fill the TargetVertexHandles vector by given the index ordering and commands
 * @param a_Mesh The mesh containing the halfedge.
 * @param a_HalfedgeHandle The halfedge handle to start from.
 * @param a_TargetVertexHandles The target vector to be filled.
 * @param a_OrderOfVertIdx A vector of indices to indicate the target index in a_TargetVertexHandles.
 * @param a_Commands A vector of commands to execute.
 * Commands: 1=next halfedge, 2=previous halfedge, 3=opposite halfedge, 4=get from vertex
 * @note This function will not change the input halfedge!
 */
void init_verts_fixed_halfedge(const MeshType& a_Mesh, HalfedgeHandle a_HalfedgeHandle, std::vector<VertexHandle>& a_TargetVertexHandles,
                               const std::vector<int>& a_OrderOfVertIdx, const std::vector<int>& a_Commands)
{
    auto t_VertexHandles = get_verts(a_Mesh, a_HalfedgeHandle, a_Commands);
    for(int i=0; i<t_VertexHandles.size(); i++)
    {
        a_TargetVertexHandles[a_OrderOfVertIdx[i]] = t_VertexHandles[i];
    }
}

/**
 * \ingroup halfedge_helper
 * @brief Duplicate commands by specific times
 * @param a_Times The number of times to duplicate the commands
 * @param a_Commands The commands to be duplicated
 */
std::vector<int> duplicate_commands(int a_Times, const std::vector<int>& a_Commands)
{
    return Helper::duplicate_vector(a_Times, a_Commands);
}

} // end of HalfedgeOperation namespace
