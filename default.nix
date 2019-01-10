{ pkgs ? (import <nixpkgs> {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

stdenvNoCC.mkDerivation {
    name = "kaleo-${version}";
    inherit version;
    src = lib.cleanSource source;
    buildInputs = [ clang_7 makeWrapper ];
    enableParallelBuilding = true;
}
