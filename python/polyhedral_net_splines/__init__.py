from .polyhedral_net_splines import Pns_control_mesh, get_patch_builders, interpret_gradient_handles, set_boundary_gradient, PatchBuilder, Patch, PatchConsumer, BVWriter, IGSWriter, STEPWriter,process_mesh
# from .extras import *

__all__ = ["Pns_control_mesh", "get_patch_builders", "PatchBuilder", "Patch", "PatchConsumer", "BVWriter", "IGSWriter", "process_mesh", "create_bv_from_file", "create_igs_from_file", "create_bv", "create_igs"]

def create_bv_from_file(control_mesh_file, output_file="output.bv", degreeRaise = False):
    """
    Generate a **BV** file from a mesh file.

    Args:
        control_mesh_file (str):
            Path to the control net (any format accepted by
            `Pns_control_mesh.from_file`).
        output_file (str, optional):
            Destination ``*.bv`` file name.
        degree_raise (bool, optional):
             Raise degree to 3. Default is ``False``.

    Returns:
        None
    """
    control_mesh = Pns_control_mesh.from_file(control_mesh_file)
    bvWriter = BVWriter(output_file)
    process_mesh(control_mesh, bvWriter, degreeRaise)

def create_igs_from_file(control_mesh_file, output_file="output.igs", degreeRaise = False):
    """
    Generate an **IGS** (IGES) file from a mesh on disk.

    Args:
        control_mesh_file (str): Path to the input control net.
        output_file (str, optional): Destination ``*.igs`` file name.
        degree_raise (bool, optional):
            Raise degree to 3. Default is ``False``.

    Returns:
        None
    """
    control_mesh = Pns_control_mesh.from_file(control_mesh_file)
    igsWriter = IGSWriter(output_file)
    process_mesh(control_mesh, igsWriter, degreeRaise)

def create_step_from_file(control_mesh_file, output_file="output.step", degreeRaise = False):
    """
    Generate a **STEP** file from a mesh on disk.

    Args:
        control_mesh_file (str): Path to the input control net.
        output_file (str, optional): Destination ``*.step`` file name.
        degree_raise (bool, optional):
            Raise degree to 3. Default is ``False``.

    Returns:
        None
    """
    control_mesh = Pns_control_mesh.from_file(control_mesh_file)
    stepWriter = STEPWriter(output_file)  # Assuming IGSWriter can also handle STEP format
    process_mesh(control_mesh, stepWriter, degreeRaise)

def create_bv(control_mesh, output_file="output.bv", degreeRaise = False):
    """
    Write a **BV** for a Pns surface file from a control mesh.

    Args:
        control_mesh (Pns_control_mesh): control net.
        output_file (str, optional): Destination ``*.bv`` file name.
        degree_raise (bool, optional): Raise degree to 3. Default is ``False``.

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
    Write an **IGS** file from a control mesh.

    Args:
        control_mesh (Pns_control_mesh): control net.
        output_file (str, optional): Destination ``*.igs`` file name.
        degree_raise (bool, optional): Raise degree to 3. Default is ``False``.

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

def create_step(control_mesh, output_file="output.step", degreeRaise = False):
    """
    Write a **STEP** file from a control mesh.

    Args:
        control_mesh (Pns_control_mesh): control net.
        output_file (str, optional): Destination ``*.step`` file name.
        degree_raise (bool, optional): Raise degree to 3. Default is ``False``.

    Returns:
        None
    """
    stepWriter = STEPWriter(output_file)
    stepWriter.start()
    t_PatchBuilders = get_patch_builders(control_mesh)
    for patchBuilder in t_PatchBuilders:
        patches = patchBuilder.build_patches(control_mesh)
        for t_Patch in patches:
            if degreeRaise: 
                t_Patch.degRaise
            stepWriter.consume(t_Patch)
    stepWriter.stop()