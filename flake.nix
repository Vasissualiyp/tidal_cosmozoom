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
        customFftw_single = pkgs.fftw.override {
          precision = "single";
          #enableMpi = true;
          #mpi = pkgs.openmpi;
        };
        
        customFftw_double = pkgs.fftw.override {
          precision = "double"; 
          #enableMpi = true;
          #mpi = pkgs.openmpi;
        };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            customFftw_double
            customFftw_single
          ];
        };
        shellHook = ''
            echo "Welcome to the tidal_cosmozoom development shell!"
            '';

      }
    );
}
