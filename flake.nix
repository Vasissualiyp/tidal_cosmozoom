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
			eigen
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
            alias remake="cmake --build build --target clear; cmake -B build -G Ninja; cmake --build build"
            alias reclean="cmake --build build --target clear"
            alias bld="cmake -B build -G Ninja; cmake --build build"
		  	echo "Welcome to the tidal_cosmozoom development shell!"
		  	echo ""
		  	echo "To configure the project builder:"
		  	echo "cmake -B build -G Ninja"
		  	echo ""
		  	echo "To build the project, run remake, or:"
		  	echo "cmake --build build"
		  	echo ""
		  	echo "To clean the build files, run reclean, or:"
		  	echo "cmake --build build --target clear"
			echo ""
			echo "run remake to clean and rebuild the project, bld to just build it"
		  	'';
		};
	  }
	);
}
