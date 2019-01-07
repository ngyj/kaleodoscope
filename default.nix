{ pkgs ? (import <nixpkgs> {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

clangStdenv.mkDerivation rec {
    name = "kaleo-${version}";
    inherit version;
    src = lib.cleanSource source;
    buildInputs = [ makeWrapper ];
    enableParallelBuilding = true;
}
