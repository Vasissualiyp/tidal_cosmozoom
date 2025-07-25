#include <Eigen/Dense>
#include <vector>
#include "vars.hh"
#include "tidal.hh"

    // Allocate memory in SoA layout
TensorField::TensorField(size_t nx, size_t ny, size_t nz) 
        : nx(nx), ny(ny), nz(nz), 
          components(9, std::vector<REAL>(nx * ny * nz)) {}
    
// Access tensor at grid point (i, j, k)
Tensor3x3 TensorField::operator()(size_t i, size_t j, size_t k) {
    const size_t idx = i + j*nx + k*nx*ny;
    Tensor3x3 T;
    T << 
        xx(idx), xy(idx), xz(idx),
        yx(idx), yy(idx), yz(idx),
        zx(idx), zy(idx), zz(idx);
    return T;
}

// Component accessors
REAL& TensorField::xx(size_t idx) { return components[0][idx]; }
REAL& TensorField::xy(size_t idx) { return components[1][idx]; }
REAL& TensorField::xz(size_t idx) { return components[2][idx]; }
REAL& TensorField::yx(size_t idx) { return components[3][idx]; }
REAL& TensorField::yy(size_t idx) { return components[4][idx]; }
REAL& TensorField::yz(size_t idx) { return components[5][idx]; }
REAL& TensorField::zx(size_t idx) { return components[6][idx]; }
REAL& TensorField::zy(size_t idx) { return components[7][idx]; }
REAL& TensorField::zz(size_t idx) { return components[8][idx]; }

// Component-wise pointers (for FFT)
REAL* TensorField::xx_ptr() { return components[0].data(); }
REAL* TensorField::xy_ptr() { return components[1].data(); }
REAL* TensorField::xz_ptr() { return components[2].data(); }
REAL* TensorField::yx_ptr() { return components[3].data(); }
REAL* TensorField::yy_ptr() { return components[4].data(); }
REAL* TensorField::yz_ptr() { return components[5].data(); }
REAL* TensorField::zx_ptr() { return components[6].data(); }
REAL* TensorField::zy_ptr() { return components[7].data(); }
REAL* TensorField::zz_ptr() { return components[8].data(); }

// Tensor operations
Vector3 TensorField::eigen_values(size_t i, size_t j, size_t k) {
    Eigen::SelfAdjointEigenSolver<Tensor3x3> solver((*this)(i,j,k));
    return solver.eigenvalues();
}
