import vtk
from vtk.util import numpy_support
import numpy as np
import sys

# A small utility to convert PeakPatch overdensity field to VTK format, viewable in ParaView

def convert_binary_to_vtk(input_file, output_file, n):
    total_floats = n * n * n
    parttype=np.float32
    order = 'F'
    
    # Read binary data
    with open(input_file, 'rb') as f:
        data = np.fromfile(f, dtype=parttype, count=total_floats)
        if data.size != total_floats:
            raise ValueError(f"File contains {data.size} floats, expected {total_floats}")

    # Reshape to 3D array with Fortran order (column-major)
    data_3d = data.reshape((n, n, n), order=order)
    
    # Convert to C-order (row-major) for VTK compatibility
    if order == 'F':
        data_c_order = data_3d.transpose(2, 1, 0).copy()  # Transpose to (x, y, z)
    else:
        data_c_order = data_3d.copy()
    
    # Convert numpy array to VTK array
    vtk_array = numpy_support.numpy_to_vtk(
        num_array=data_c_order.ravel(),  # Flatten to 1D in C-order
        deep=True,
        array_type=vtk.VTK_FLOAT
    )
    vtk_array.SetName('overdensity')  # Name for the scalar data

    # Create VTK image data structure
    image_data = vtk.vtkImageData()
    image_data.SetDimensions(n, n, n)  # Grid dimensions (x, y, z)
    image_data.SetSpacing(1.0, 1.0, 1.0)  # Uniform spacing
    image_data.SetOrigin(0.0, 0.0, 0.0)    # Grid origin
    image_data.GetPointData().SetScalars(vtk_array)  # Attach scalar data

    # Write to VTK file (legacy format)
    writer = vtk.vtkStructuredPointsWriter()
    writer.SetFileName(output_file)
    writer.SetInputData(image_data)
    writer.SetFileTypeToBinary()
    writer.Write()

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python convert.py <input_file> <output_file.vtk> <n_total>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    n = int(sys.argv[3])
    
    convert_binary_to_vtk(input_file, output_file, n)
    print(f"Successfully converted to {output_file}")
