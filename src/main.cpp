#include "params.hh"
#include "gravity.hh"
#include "arrays.hh"

void perform_cutting_and_padding(REAL*& overdensity, Parameters& params, std::ofstream& output_stream) {
	int dn = params.get<int>("dn");
	int padding = params.get<int>("padding");
	int current_pad = params.get<int>("current_padding");
	int padding_value_is_box_average = params.get<int>("pad_with_box_avg");
	int n_cut = params.get<int>("n") - 2*(dn + current_pad);
	int n_pad = n_cut + 2*(padding + current_pad);
	REAL padding_value;
	if (n_cut >= params.get<int>("min_n")) {
		REAL *overdensity_cut = new REAL[n_cut*n_cut*n_cut];
		REAL *overdensity_pad = new REAL[n_pad*n_pad*n_pad];

		Parameters cut_params = cut_boundaries(overdensity, overdensity_cut, params);

		if (padding_value_is_box_average==1) {
			int* max_loc = find_loc_of_max_in_array(overdensity_cut, n_cut, true);
			int* min_loc = find_loc_of_max_in_array(overdensity_cut, n_cut, false);
			int idx_max = max_loc[0] * n_cut * n_cut + max_loc[1] * n_cut + max_loc[2];
			int idx_min = min_loc[0] * n_cut * n_cut + min_loc[1] * n_cut + min_loc[2];
			REAL max_arr = overdensity_cut[idx_max];
			REAL min_arr = overdensity_cut[idx_min];
			padding_value = (max_arr - min_arr) / 2;
		} else {
			padding_value = 0.0;
		}
		Parameters pad_params = pad_boundaries(overdensity_cut, overdensity_pad, 
											   cut_params, padding_value);

		delete[] overdensity;
		delete[] overdensity_cut;
		calculate_and_save_fields_from_overdensity(overdensity_pad, pad_params, output_stream);
		overdensity = overdensity_pad;
		params = pad_params;
	} else {
	}
}

void obtain_Tij_table_for_single_realization(Parameters& params) {
	int n = params.get<int>("n");

    // Generate the overdensity field by convolution of white noise with the
    // transfer function (BBKS) and shift it to the largest potential value
    // being centered
    REAL *overdensity = new REAL[n*n*n];
	generate_overdensity_field(overdensity, params);

	// Write the Tij table for the full box
	std::string tij_table_fname = contruct_Tij_table_fname(params);
	std::ofstream output_stream(tij_table_fname);
	if (output_stream.is_open()) {
		int prepend_header = 1;
		calculate_and_save_fields_from_overdensity(overdensity, params, output_stream, prepend_header);

		// Cut the boundaries of the overdensity, and pad the box afterwards
		int num_boundary_cutoffs = params.get<int>("num_bnd_cutoffs");
		for (int i=0; i<num_boundary_cutoffs; i++) {
			perform_cutting_and_padding(overdensity, params, output_stream);
		}
		output_stream.close();
		std::cout << "Saved the output to: " << tij_table_fname << std::endl;
	}
}

int main(int argc, char *argv[]) {
	Parameters params = obtain_params_from_cli(argc, argv);
	int num_of_realizations = params.get<int>("seeds_num");
	int seed = params.get<int>("seed");
	int initial_n = params.get<int>("n");
	for (int i=0; i<num_of_realizations; i++) {
		obtain_Tij_table_for_single_realization(params);
		seed += 1;
		params.set_seed(seed);
		params.reset(initial_n);
	}
	
	return 0;
}
