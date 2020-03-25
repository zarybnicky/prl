{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = [
    pkgs.openmpi
    (pkgs.python3.withPackages (ps: with ps; [ numpy scipy matplotlib notebook ]))
  ];
}
