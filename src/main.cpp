#include "params.hh"
#include "gravity.hh"
#include "arrays.hh"

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

	bool prepend_header=true;
	calculate_and_save_fields_from_overdensity(overdensity, params, prepend_header);
	
	int dn = params.get<int>("dn");
	int smallest_box = params.get<int>("min_n");
	int num_boundary_cutoffs = n / dn;
	for (int i=0; i<num_boundary_cutoffs; i++) {
		int n_cut = params.get<int>("n") - 2*dn;
		if (n_cut > smallest_box) {
			REAL *overdensity_cut = new REAL[n_cut*n_cut*n_cut];
			Parameters cut_params = cut_boundaries(overdensity, overdensity_cut, 
												   params);
			delete[] overdensity;
			calculate_and_save_fields_from_overdensity(overdensity_cut, cut_params);
			overdensity = overdensity_cut;
			params = cut_params;
		}
	}
	
	return 0;
}
