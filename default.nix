{ pkgs ? (import <nixpkgs> {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

stdenvNoCC.mkDerivation {
    name = "kaleo-${version}";
    inherit version;
    src = lib.cleanSource source;
    buildInputs = [ llvm_7 clang_7 makeWrapper ];
    enableParallelBuilding = true;
}
