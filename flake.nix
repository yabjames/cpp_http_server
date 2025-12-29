{
  description = "A C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  };

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
    in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        git
        k6

	nodejs

        gcc
        lcov
        gdb

        # If you want to use clang and llvm-cov instead of gcc, uncomment
          # clang
          # llvmPackages.llvm

        gnumake
        cmake
        conan

        python312
      ];
    };
  };
}
