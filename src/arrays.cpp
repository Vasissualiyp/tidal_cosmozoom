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
	int n_post_cut = cut_params.get<int>("n");
	assert(n_post_cut > 0);
	int lower_bnd = dn;
	int upper_bnd = n_pre_cut - dn;

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
