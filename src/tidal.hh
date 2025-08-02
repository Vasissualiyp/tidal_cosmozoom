#pragma once
#include <Eigen/Dense>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <array>
#include "io.hh"

template<typename T>
class TensorField {
public:
	using Tensor3x3 = Eigen::Matrix<T, 3, 3>;
	using Vector3 = Eigen::Matrix<T, 3, 1>;
	// Allocate memory in SoA layout
	TensorField(size_t nx, size_t ny, size_t nz, bool symmetric=false) 
		: nx(nx), ny(ny), nz(nz), 
		  symmetric(symmetric),
		  num_components(symmetric ? 6 : 9),
		  components(num_components, std::vector<T>(nx * ny * nz)) {
			if (symmetric == true) {
				tensor_comp_id = {0, 1, 2, 1, 3, 4, 2, 4, 5}; 
				subscripts = {"xx", "xy", "xz", "yy", "yz", "zz"}; 
			} else {
				tensor_comp_id = {0, 1, 2, 3, 4, 5, 6, 7, 8}; 
				subscripts = {"xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"}; 
			}

		  }

	~TensorField() {
	}
	
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

	bool is_symmetric() { return symmetric; }
	
	// Component accessors
	T& access_by_id_in_tensor(size_t idx, int tensor_component){ 
		return components[tensor_comp_id[tensor_component]][idx]; 
	};
	T& xx(size_t idx){ return access_by_id_in_tensor(idx, 0); };
	T& xy(size_t idx){ return access_by_id_in_tensor(idx, 1); };
	T& xz(size_t idx){ return access_by_id_in_tensor(idx, 2); };
	T& yx(size_t idx){ return access_by_id_in_tensor(idx, 3); };
	T& yy(size_t idx){ return access_by_id_in_tensor(idx, 4); };
	T& yz(size_t idx){ return access_by_id_in_tensor(idx, 5); };
	T& zx(size_t idx){ return access_by_id_in_tensor(idx, 6); };
	T& zy(size_t idx){ return access_by_id_in_tensor(idx, 7); };
	T& zz(size_t idx){ return access_by_id_in_tensor(idx, 8); };
	
	// Component-wise pointers (for FFT)
	T* ptr_by_id_in_tensor(int tensor_component){ 
		return components[tensor_comp_id[tensor_component]].data(); 
	};
	T* xx_ptr(){ return ptr_by_id_in_tensor(0); };
	T* xy_ptr(){ return ptr_by_id_in_tensor(1); };
	T* xz_ptr(){ return ptr_by_id_in_tensor(2); };
	T* yx_ptr(){ return ptr_by_id_in_tensor(3); };
	T* yy_ptr(){ return ptr_by_id_in_tensor(4); };
	T* yz_ptr(){ return ptr_by_id_in_tensor(5); };
	T* zx_ptr(){ return ptr_by_id_in_tensor(6); };
	T* zy_ptr(){ return ptr_by_id_in_tensor(7); };
	T* zz_ptr(){ return ptr_by_id_in_tensor(8); };

	// Component setters
	void set_by_id_in_tensor(size_t idx, int tensor_component, T value){ 
		components[tensor_comp_id[tensor_component]][idx] = value; 
	};
	void xx_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 0, value); };
	void xy_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 1, value); };
	void xz_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 2, value); };
	void yx_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 3, value); };
	void yy_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 4, value); };
	void yz_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 5, value); };
	void zx_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 6, value); };
	void zy_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 7, value); };
	void zz_set(size_t idx, T value){ return set_by_id_in_tensor(idx, 8, value); };
	
	// Tensor operations
	Vector3 eigen_values(size_t i, size_t j, size_t k) {
		Eigen::SelfAdjointEigenSolver<Tensor3x3> solver((*this)(i,j,k));
		return solver.eigenvalues();
	}

	int tensor_idx_from_component_id(int i) {
		const int n = static_cast<int>(nx);
		int idx;
		for (idx = 0; idx < n; idx++) {
        	if (tensor_comp_id[idx] == i) {
        		break;
        	}
    	}
		return idx;
	}

	void write_tensor_to_binary_files(int n, std::string postfix = "", int write_output = 1) {
		for (int i=0; i<num_components; i++) {
		    std::string extension = ".bin";
		    std::string prefix = "out/T";
			std::string subscript = subscripts[i];
			int idx = tensor_idx_from_component_id(i);
			T* pointer = ptr_by_id_in_tensor(idx);
			std::string fname = prefix.append(subscript).append(postfix).append(extension);
			write_field_to_binary_file(pointer, n, fname, write_output);
		}
	}

	void print_tensor_table_at_loc(int idx, int n) {
		std::cout << nx;
		std::cout << "," << n;
		for (int i=0; i<num_components; i++) {
			int component_idx = tensor_idx_from_component_id(i);
			T T_value = access_by_id_in_tensor(idx, component_idx);
			std::cout << "," << T_value;
		}
		std::cout << std::endl;
	}

	void print_tensor_table_header() {
		std::cout << "n";
		for (int i=0; i<num_components; i++) {
			std::string subscript = subscripts[i];
			std::cout << ",T" << subscript;
		}
		std::cout << std::endl;
	}

	int get_num_components() { return num_components; }

private:
	size_t nx, ny, nz;
	bool symmetric;
	int num_components;
	std::array<int,9> tensor_comp_id;
	std::vector<std::string> subscripts;
	std::vector<std::vector<T>> components;  // [6 or 9][nx*ny*nz] 
};

// TEMPLATE END

void tidal_tensor_from_potential(FFTW::complex_type* potential_fft,
								 TensorField<std::complex<REAL>>& TidalTensor_fft,
								 Parameters params);

void ifft_tidal_tensor(TensorField<std::complex<REAL>>& TidalTensor_fft,
					   TensorField<REAL>& TidalTensor,
					   Parameters params);
