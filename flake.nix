{
  description = "tidal_cosmozoom flake";

  inputs = {
	nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
	flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
	flake-utils.lib.eachDefaultSystem (system:
	  let
		pkgs = import nixpkgs {
		  inherit system;
		  config = {
			allowUnfree = true;
		  };
		};
		python = pkgs.python311Packages.python;
		pythonEnv = (python.withPackages (ps: with ps; [
		  #healpy # HEALPY IS NOT IN NIXPKGS, SO NEED TO MANUALLY PACKAGE IT!
		  pandas
		  matplotlib
		  numpy
		  vtk
		]));
		customFftw_single = pkgs.fftw.override {
		  precision = "single";
		};
	  in
	  {
		devShell = pkgs.mkShell {
		  buildInputs = with pkgs; [
			# Code dependencies
			gcc
			fftw
			customFftw_single
			# Visualsization
			vtk
			pythonEnv
			# Build requirements
			cmake
			ninja
			pkg-config
		  ];
		  shellHook = ''
		  	cmake -B build -G Ninja > /dev/null
		  	echo "Welcome to the tidal_cosmozoom development shell!"
		  	echo ""
		  	echo "To configure the project builder (should already be configured if you use the flake):"
		  	echo "cmake -B build -G Ninja"
		  	echo ""
		  	echo "To build the project:"
		  	echo "cmake --build build"
		  	echo ""
		  	echo "To clean the build files:"
		  	echo "cmake --build build --target clear"
		  	'';
		};
	  }
	);
}
