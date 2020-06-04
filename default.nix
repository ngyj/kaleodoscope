{ pkgs ? (import <nixpkgs> {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

stdenvNoCC.mkDerivation {
    name = "mangekyou-${version}";
    inherit version;
    src = lib.cleanSource source;
    buildInputs = [ llvm_7 clang_7 makeWrapper ];
    enableParallelBuilding = true;

    buildPhase = ''
      make all
    '';

    installPhase = ''
      mkdir -p $out
      cp mangekyou $out/
    '';
}
