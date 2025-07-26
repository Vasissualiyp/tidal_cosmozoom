#pragma once
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include "write.hh"

template<typename T>
class TensorField {
public:
	using Tensor3x3 = Eigen::Matrix<T, 3, 3>;
	using Vector3 = Eigen::Matrix<T, 3, 1>;
	// Allocate memory in SoA layout
	TensorField(size_t nx, size_t ny, size_t nz) 
		: nx(nx), ny(ny), nz(nz), 
		  components(9, std::vector<T>(nx * ny * nz)) {}
	

	
	// Access tensor at grid point (i, j, k)
	Tensor3x3 operator()(size_t i, size_t j, size_t k) {
		//const size_t idx = i + j*nx + k*nx*ny; // Column-major
		const size_t idx = i*nx*ny + j*ny + k; // Row-major
		Tensor3x3 tensor;
		tensor << 
			xx(idx), xy(idx), xz(idx),
			yx(idx), yy(idx), yz(idx),
			zx(idx), zy(idx), zz(idx);
		return tensor;
	}
	
	// Component accessors
	T& xx(size_t idx){ return components[0][idx]; };
	T& xy(size_t idx){ return components[1][idx]; };
	T& xz(size_t idx){ return components[2][idx]; };
	T& yx(size_t idx){ return components[3][idx]; };
	T& yy(size_t idx){ return components[4][idx]; };
	T& yz(size_t idx){ return components[5][idx]; };
	T& zx(size_t idx){ return components[6][idx]; };
	T& zy(size_t idx){ return components[7][idx]; };
	T& zz(size_t idx){ return components[8][idx]; };
	
	// Component-wise pointers (for FFT)
	T* xx_ptr(){ return components[0].data(); };
	T* xy_ptr(){ return components[1].data(); };
	T* xz_ptr(){ return components[2].data(); };
	T* yx_ptr(){ return components[3].data(); };
	T* yy_ptr(){ return components[4].data(); };
	T* yz_ptr(){ return components[5].data(); };
	T* zx_ptr(){ return components[6].data(); };
	T* zy_ptr(){ return components[7].data(); };
	T* zz_ptr(){ return components[8].data(); };

	// Component setters
	void xx_set(size_t idx, T value){ components[0][idx] = value; };
	void xy_set(size_t idx, T value){ components[1][idx] = value; };
	void xz_set(size_t idx, T value){ components[2][idx] = value; };
	void yx_set(size_t idx, T value){ components[3][idx] = value; };
	void yy_set(size_t idx, T value){ components[4][idx] = value; };
	void yz_set(size_t idx, T value){ components[5][idx] = value; };
	void zx_set(size_t idx, T value){ components[6][idx] = value; };
	void zy_set(size_t idx, T value){ components[7][idx] = value; };
	void zz_set(size_t idx, T value){ components[8][idx] = value; };
	
	// Tensor operations
	Vector3 eigen_values(size_t i, size_t j, size_t k) {
		Eigen::SelfAdjointEigenSolver<Tensor3x3> solver((*this)(i,j,k));
		return solver.eigenvalues();
	}

	void write_tensor_to_binary_files() {
		T* xx = xx_ptr();
		T* xy = xy_ptr();
		T* xz = xz_ptr();
		T* yx = yx_ptr();
		T* yy = yy_ptr();
		T* yz = yz_ptr();
		T* zx = zx_ptr();
		T* zy = zy_ptr();
		T* zz = zz_ptr();
		int n = static_cast<int>(nx);
		write_field_to_binary_file(xx, n, "out/Txx.bin");
		write_field_to_binary_file(xy, n, "out/Txy.bin");
		write_field_to_binary_file(xz, n, "out/Txz.bin");
		write_field_to_binary_file(yx, n, "out/Tyx.bin");
		write_field_to_binary_file(yy, n, "out/Tyy.bin");
		write_field_to_binary_file(yz, n, "out/Tyz.bin");
		write_field_to_binary_file(zx, n, "out/Tzx.bin");
		write_field_to_binary_file(zy, n, "out/Tzy.bin");
		write_field_to_binary_file(zz, n, "out/Tzz.bin");
	}

private:
	size_t nx, ny, nz;
	std::vector<std::vector<T>> components;  // [9][nx*ny*nz]
};
