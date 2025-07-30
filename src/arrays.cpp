#include "vars.hh"
#include "params.hh"
#include "arrays.hh"
#include <cassert>

Parameters remove_boundaries(REAL* pre_cut_arr, 
							 REAL* post_cut_arr,
							 Parameters params,
							 int boundary_change) {
	Parameters cut_params(params);
	cut_params.reduce_meshsize(2 * boundary_change);
	int n_pre_cut = params.get<int>("n");
	int n_post_cut = cut_params.get<int>("n");
	assert(n_post_cut > 0);
	int lower_bnd = boundary_change;
	int upper_bnd = n_pre_cut - boundary_change;

	// Loop through relevant points inside of the pre-cut array
	int i_post_cut, j_post_cut, k_post_cut, idx_pre_cut, idx_post_cut;
	for (int i=lower_bnd; i<upper_bnd; i++) {
		i_post_cut = i - lower_bnd;
		for (int j=lower_bnd; j<upper_bnd; j++) {
			j_post_cut = j - lower_bnd;
			for (int k=lower_bnd; k<upper_bnd; k++) {
				k_post_cut = k - lower_bnd;
				idx_pre_cut = i*n_pre_cut*n_pre_cut + j*n_pre_cut + k;
				idx_post_cut = i_post_cut*n_post_cut*n_post_cut + 
							   j_post_cut*n_post_cut + k_post_cut;
				post_cut_arr[idx_post_cut] = pre_cut_arr[idx_pre_cut];
			}
		}
	}

	return cut_params;
}
