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
			gcc
			fftw
			vtk
			customFftw_single
			pythonEnv
		  ];
		};
		shellHook = ''
			echo "Welcome to the tidal_cosmozoom development shell!"
			python -m ipykernel install --user --name=python-env --display-name="Python (Env)"

			'';
	  }
	);
}
