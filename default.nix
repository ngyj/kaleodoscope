{ pkgs ? (import (import ./nixpkgs.nix) {})
, source ? ./.
, version ? "dev"
} :

with pkgs;

stdenvNoCC.mkDerivation {
    name = "mangekyou-${version}";
    inherit version;
    src = lib.cleanSource source;
    buildInputs = [ llvm_10 clang_10 makeWrapper ];
    enableParallelBuilding = true;

    buildPhase = ''
      make all
    '';

    installPhase = ''
      mkdir -p $out
      cp mangekyou $out/
    '';
}
