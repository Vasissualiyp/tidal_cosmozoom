#include <Eigen/Dense>
#include "vars.hh"
#include <vector>

using Tensor3x3 = Eigen::Matrix<REAL, 3, 3>;
using Vector3 = Eigen::Matrix<REAL, 3, 1>;

class TensorField {
public:
    // Allocate memory in SoA layout
    TensorField(size_t nx, size_t ny, size_t nz);
    
    // Access tensor at grid point (i, j, k)
    Tensor3x3 operator()(size_t i, size_t j, size_t k);
    
    // Component accessors
    REAL& xx(size_t idx);
    REAL& xy(size_t idx);
    REAL& xz(size_t idx);
    REAL& yx(size_t idx);
    REAL& yy(size_t idx);
    REAL& yz(size_t idx);
    REAL& zx(size_t idx);
    REAL& zy(size_t idx);
    REAL& zz(size_t idx);
    
    // Component-wise pointers (for FFT)
    REAL* xx_ptr();
    REAL* xy_ptr();
    REAL* xz_ptr();
    REAL* yx_ptr();
    REAL* yy_ptr();
    REAL* yz_ptr();
    REAL* zx_ptr();
    REAL* zy_ptr();
    REAL* zz_ptr();
    
    // Tensor operations
    Vector3 eigen_values(size_t i, size_t j, size_t k);

private:
    size_t nx, ny, nz;
    std::vector<std::vector<REAL>> components;  // [9][nx*ny*nz]
};
