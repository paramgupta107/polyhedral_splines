from .polyhedral_net_splines import Pns_control_mesh, get_patch_builders, PatchBuilder, Patch, PatchConsumer, BVWriter, IGSWriter, process_mesh
# from .extras import *

__all__ = ["Pns_control_mesh", "get_patch_builders", "PatchBuilder", "Patch", "PatchConsumer", "BVWriter", "IGSWriter", "process_mesh", "create_bv_from_file", "create_igs_from_file", "create_bv", "create_igs"]

def create_bv_from_file(control_mesh_file, output_file="output.bv", degreeRaise = False):
    """
    Writes a BV file from a control mesh file.
    Parameters:
        control_mesh_file (str): The path to the control mesh file.
    Returns:
        None
    """
    control_mesh = Pns_control_mesh.from_file(control_mesh_file)
    bvWriter = BVWriter(output_file)
    process_mesh(control_mesh, bvWriter, degreeRaise)

def create_igs_from_file(control_mesh_file, output_file="output.igs", degreeRaise = False):
    """
    Writes a IGS file from a control mesh file.
    Parameters:
        control_mesh_file (str): The path to the control mesh file.
    Returns:
        None
    """
    control_mesh = Pns_control_mesh.from_file(control_mesh_file)
    igsWriter = IGSWriter(output_file)
    process_mesh(control_mesh, igsWriter, degreeRaise)

def create_bv(control_mesh, output_file="output.bv", degreeRaise = False):
    """
    Writes a BV file from a control mesh file.
    Parameters:
        control_mesh_file (str): The path to the control mesh file.
    Returns:
        None
    """
    bvWriter = BVWriter(output_file)
    bvWriter.start()
    t_PatchBuilders = get_patch_builders(control_mesh)
    for patchBuilder in t_PatchBuilders:
        patches = patchBuilder.build_patches(control_mesh)
        for t_Patch in patches:
            if degreeRaise: 
                t_Patch.degRaise
            bvWriter.consume(t_Patch)
    bvWriter.stop()

def create_igs(control_mesh, output_file="output.igs", degreeRaise = False):
    """
    Writes a IGS file from a control mesh file.
    Parameters:
        control_mesh_file (str): The path to the control mesh file.
    Returns:
        None
    """
    igsWriter = IGSWriter(output_file)
    igsWriter.start()
    t_PatchBuilders = get_patch_builders(control_mesh)
    for patchBuilder in t_PatchBuilders:
        patches = patchBuilder.build_patches(control_mesh)
        for t_Patch in patches:
            if degreeRaise: 
                t_Patch.degRaise
            igsWriter.consume(t_Patch)
    igsWriter.stop()