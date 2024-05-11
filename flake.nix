{
  description = "seashell";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell.override { stdenv = pkgs.gcc13Stdenv; } {
      packages = with pkgs; [ 
        clang-tools_18
        valgrind
        gdb
      ];

      buildInputs = with pkgs; [
        fmt
        lyra
      ];

      nativeBuildInputs = with pkgs; [
        pkg-config
        ninja
        meson
      ];

      shellHook = ''
      '';
    };
  };
}
