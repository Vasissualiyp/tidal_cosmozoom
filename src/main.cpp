#include "params.hh"
#include "gravity.hh"
#include "arrays.hh"

void perform_cutting_and_padding(REAL*& overdensity, Parameters& params) {
	int dn = params.get<int>("dn");
	int padding = params.get<int>("padding");
	int current_pad = params.get<int>("current_padding");
	int n_cut = params.get<int>("n") - 2*(dn + current_pad);
	int n_pad = n_cut + 2*(padding + current_pad);
	if (n_cut >= params.get<int>("min_n")) {
		REAL *overdensity_cut = new REAL[n_cut*n_cut*n_cut];
		REAL *overdensity_pad = new REAL[n_pad*n_pad*n_pad];

		Parameters cut_params = cut_boundaries(overdensity, overdensity_cut, params);
		int* max_loc = find_loc_of_max_in_array(overdensity_cut, n_cut, true);
		int* min_loc = find_loc_of_max_in_array(overdensity_cut, n_cut, false);
		int idx_max = max_loc[0] * n_cut * n_cut + max_loc[1] * n_cut + max_loc[2];
		int idx_min = min_loc[0] * n_cut * n_cut + min_loc[1] * n_cut + min_loc[2];
		REAL max_arr = overdensity_cut[idx_max];
		REAL min_arr = overdensity_cut[idx_min];
		REAL mid_arr = (max_arr - min_arr) / 2;
		Parameters pad_params = pad_boundaries(overdensity_cut, overdensity_pad, 
											   cut_params, mid_arr);

		delete[] overdensity;
		delete[] overdensity_cut;
		calculate_and_save_fields_from_overdensity(overdensity_pad, pad_params);
		overdensity = overdensity_pad;
		params = pad_params;
	}
}

int main(int argc, char *argv[]) {
	using namespace std::complex_literals;
	const char* filename;
	int seed;

	// CLI: pass path to the parameter file and read the file
	if (argc < 2) {
		filename = "params.txt";
		seed = 0;
	} else if (argc == 2) {
		filename = argv[1];
		seed = 0;
	} else {
		filename = argv[1];
		seed = atoi(argv[2]);
	}
	Parameters params;
	params.read_params_from_file(filename);
	if (seed != 0)
		params.set_seed(seed);

	int n = params.get<int>("n");
	REAL *overdensity = new REAL[n*n*n];
	generate_overdensity_field(overdensity, params);
	int prepend_header = 1;
	calculate_and_save_fields_from_overdensity(overdensity, params, prepend_header);
	int num_boundary_cutoffs = params.get<int>("num_bnd_cutoffs");
	for (int i=0; i<num_boundary_cutoffs; i++) {
		perform_cutting_and_padding(overdensity, params);
	}
	
	return 0;
}
