import vtk
from vtk.util import numpy_support
import numpy as np
import os

# A small utility to convert PeakPatch overdensity field to VTK format, viewable in ParaView

def add_arrays_to_vtk(output_file, arrays, array_names, dimensions, spacing=(1.0, 1.0, 1.0), origin=None):
    # Create the VTK image data structure (once)
    image_data = vtk.vtkImageData()
    image_data.SetDimensions(dimensions)
    image_data.SetSpacing(spacing)
    if origin is None:
        origin = tuple([-dim/2 for dim in dimensions])
    image_data.SetOrigin(origin)

    # Add each array to the point data
    for array, name in zip(arrays, array_names):
        # Convert numpy array to VTK array (flatten in C-order)
        vtk_array = numpy_support.numpy_to_vtk(
            num_array=array.ravel(),
            deep=True,
            array_type=vtk.VTK_FLOAT
        )
        vtk_array.SetName(name)
        image_data.GetPointData().AddArray(vtk_array)  # Add to point data
    
    # Optionally: Set the first array as active scalars
    if array_names:
        image_data.GetPointData().SetActiveScalars(array_names[0])

    # Write to VTK XML Image Data file (.vti)
    writer = vtk.vtkXMLImageDataWriter()
    writer.SetFileName(output_file)
    writer.SetInputData(image_data)
    writer.Write()

def load_scalar_field(input_file, n):
    total_floats = n * n * n
    parttype=np.float32
    order = 'F'
    with open(input_file, 'rb') as f:
        data = np.fromfile(f, dtype=parttype, count=total_floats)
        if data.size != total_floats:
            raise ValueError(f"File contains {data.size} floats, expected {total_floats}")

    # Reshape to 3D array with Fortran order (column-major)
    data_3d = data.reshape((n, n, n), order=order)
    
    # Convert to C-order (row-major) for VTK compatibility
    if order == 'F':
        data_c_order = data_3d.transpose(2, 1, 0).copy()  # Transpose to (x, y, z)
    elif order == 'C':
        data_c_order = data_3d.copy()
    else:
        raise ValueError(f"Unknown order type: {order}")
    return data_c_order

def convert_binaries_to_vtk(output_file, field_names, n):
    arrays = []
    # Read binary data
    for field_name in field_names:
        input_file = os.path.join("out", field_name + "." + str(n) + ".bin")
        array = load_scalar_field(input_file, n) 
        arrays.append(array)
    add_arrays_to_vtk(output_file, arrays, 
                      field_names, (n, n, n))

if __name__ == "__main__":
    n = 64
    field_names = [ 'overdensity', 'potential', 'Txx', 'Txy', 'Txz', 
                    'Tyy', 'Tyz', 'Tzz' ]
    output_file = "fields"+str(n)+".vti"
    convert_binaries_to_vtk(output_file, field_names, n)
    print(f"Successfully converted to {output_file}")
    n = 44
    output_file = "fields"+str(n)+".vti"
    convert_binaries_to_vtk(output_file, field_names, n)
    print(f"Successfully converted to {output_file}")
    #convert_binaries_to_vtk(output_cut, ['overdensity_cut'], n - 2*dn)
