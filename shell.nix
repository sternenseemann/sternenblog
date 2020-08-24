{ pkgs ? (import <nixpkgs> {}) }:

pkgs.mkShell {
  name = "sternenblog-env";

  buildInputs = with pkgs; [ gnumake valgrind gdb doxygen strace ];
}
