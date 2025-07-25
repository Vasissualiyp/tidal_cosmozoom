#include <Eigen/Dense>
#include <vector>
#include "vars.hh"
#include "tidal.hh"

using Tensor3x3 = Eigen::Matrix<REAL, 3, 3>;
using Vector3 = Eigen::Matrix<REAL, 3, 1>;

class TensorField {
public:
    // Allocate memory in SoA layout
    TensorField(size_t nx, size_t ny, size_t nz) 
        : nx(nx), ny(ny), nz(nz), 
          components(9, std::vector<REAL>(nx * ny * nz)) {}
    
    // Access tensor at grid point (i, j, k)
    Tensor3x3 operator()(size_t i, size_t j, size_t k) const {
        const size_t idx = i + j*nx + k*nx*ny;
        Tensor3x3 T;
        T << 
            xx(idx), xy(idx), xz(idx),
            yx(idx), yy(idx), yz(idx),
            zx(idx), zy(idx), zz(idx);
        return T;
    }
    
    // Component accessors
    REAL& xx(size_t idx) { return components[0][idx]; }
    REAL& xy(size_t idx) { return components[1][idx]; }
    REAL& xz(size_t idx) { return components[2][idx]; }
    REAL& yx(size_t idx) { return components[3][idx]; }
    REAL& yy(size_t idx) { return components[4][idx]; }
    REAL& yz(size_t idx) { return components[5][idx]; }
    REAL& zx(size_t idx) { return components[6][idx]; }
    REAL& zy(size_t idx) { return components[7][idx]; }
    REAL& zz(size_t idx) { return components[8][idx]; }
    
    // Component-wise pointers (for FFT)
    REAL* xx_ptr() { return components[0].data(); }
    REAL* xy_ptr() { return components[1].data(); }
    REAL* xz_ptr() { return components[2].data(); }
    REAL* yx_ptr() { return components[3].data(); }
    REAL* yy_ptr() { return components[4].data(); }
    REAL* yz_ptr() { return components[5].data(); }
    REAL* zx_ptr() { return components[6].data(); }
    REAL* zy_ptr() { return components[7].data(); }
    REAL* zz_ptr() { return components[8].data(); }
    
    // Tensor operations
    Vector3 eigen_values(size_t i, size_t j, size_t k) const {
        Eigen::SelfAdjointEigenSolver<Tensor3x3> solver((*this)(i,j,k));
        return solver.eigenvalues();
    }

private:
    size_t nx, ny, nz;
    std::vector<std::vector<REAL>> components;  // [9][nx*ny*nz]
};
