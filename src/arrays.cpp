#include "vars.hh"
#include "params.hh"
#include "arrays.hh"
#include <cassert>
#include <iostream>

Parameters cut_boundaries(REAL* pre_cut_arr, 
						  REAL* post_cut_arr,
						  Parameters params) {
	Parameters cut_params(params);
	cut_params.reduce_meshsize();
	int n_pre_cut = params.get<int>("n");
	int dn = params.get<int>("dn");
	int current_pad = params.get<int>("current_padding");
	int n_post_cut = cut_params.get<int>("n");
	assert(n_post_cut > 0);
	int lower_bnd = dn + current_pad;
	int upper_bnd = n_pre_cut - dn - current_pad;

	// Loop through relevant points inside of the pre-cut array
	int i_post_cut, j_post_cut, k_post_cut;
	for (int i=lower_bnd; i<upper_bnd; i++) {
		i_post_cut = i - lower_bnd;
		for (int j=lower_bnd; j<upper_bnd; j++) {
			j_post_cut = j - lower_bnd;
			for (int k=lower_bnd; k<upper_bnd; k++) {
				k_post_cut = k - lower_bnd;
				const int idx_pre_cut = i*n_pre_cut*n_pre_cut + j*n_pre_cut + k;
				const int idx_post_cut = i_post_cut*n_post_cut*n_post_cut + 
										 j_post_cut*n_post_cut + k_post_cut;
				post_cut_arr[idx_post_cut] = pre_cut_arr[idx_pre_cut];
			}
		}
	}

	return cut_params;
}

Parameters pad_boundaries(REAL* pre_pad_arr, 
						  REAL* post_pad_arr,
						  Parameters params,
						  REAL padding_value) {
	Parameters pad_params(params);
	pad_params.increase_meshsize();
	int n_pre_pad = params.get<int>("n");
	int padding = params.get<int>("padding");
	int n_post_pad = pad_params.get<int>("n");
	assert(n_post_pad > 0);
	int lower_bnd = padding;
	int upper_bnd = n_post_pad - padding -1;

	// Loop through relevant points inside of the pre-pad array
	int i_pre_pad, j_pre_pad, k_pre_pad;
	bool padded_cell_i, padded_cell_j, padded_cell_k;
	REAL cell_value;
	for (int i=0; i<n_post_pad; i++) {
		if (i < lower_bnd || i > upper_bnd) 
			padded_cell_i = true;
		else
			padded_cell_i = false;
		i_pre_pad = i - lower_bnd;
		for (int j=0; j<n_post_pad; j++) {
			if (j < lower_bnd || j > upper_bnd) 
				padded_cell_j = true;
			else
				padded_cell_j = false;
			j_pre_pad = j - lower_bnd;
			for (int k=0; k<n_post_pad; k++) {
				if (k < lower_bnd || k > upper_bnd) 
					padded_cell_k = true;
				else
					padded_cell_k = false;
				k_pre_pad = k - lower_bnd;
				const int idx_post_pad = i*n_post_pad*n_post_pad + j*n_post_pad + k;

				// Assign values to cells
				if (padded_cell_i==true || padded_cell_j==true || padded_cell_k == true)
					cell_value = padding_value;
				else {
					const int idx_pre_pad = i_pre_pad*n_pre_pad*n_pre_pad + 
										 j_pre_pad*n_pre_pad + k_pre_pad;
					cell_value = pre_pad_arr[idx_pre_pad];
				}
				post_pad_arr[idx_post_pad] = cell_value;
			}
		}
	}

	return pad_params;
}

int* find_loc_of_max_in_array(REAL* array, int n, bool max_bool=true) {
	REAL max = array[0];
	REAL value;
	int *max_loc = new int[3];
	max_loc[0] = 0;
	max_loc[1] = 0;
	max_loc[2] = 0;
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			for (int k=0; k<n; k++) {
				const int idx = i*n*n + j*n + k;
				value = array[idx];
				if (value > max && max_bool == true) {
					max = value;
					max_loc[0] = i;
					max_loc[1] = j;
					max_loc[2] = k;
				} else if (value < max) {
					max = value;
					max_loc[0] = i;
					max_loc[1] = j;
					max_loc[2] = k;
				}
			}
		}
	}
	return max_loc;
}
