
{ pkgs ? import <nixpkgs> {} }:

with pkgs;

gcc8Stdenv.mkDerivation {
  name = "ehb-env";
  buildInputs = [
    bison
    cmake
    ffmpeg
    gdb
    libGLU_combined
    libxdg_basedir
    openscenegraph
    # (enableDebugging openscenegraph)
    pkgconfig
    re2c
    llvmPackages_7.clang-unwrapped
    llvmPackages_7.libclang
    llvmPackages_7.llvm
  ];

  LIBCLANG_LIBRARY = "${llvmPackages_7.libclang}/lib/libclang.so";
  LIBCLANG_INCLUDE_DIR = "${llvmPackages_7.clang-unwrapped}/include";
  CLANG_BINARY = "${llvmPackages_7.llvm}/bin/llvm-config";
}
