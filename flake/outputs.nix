{ self
, nixpkgs # <---- This `nixpkgs` has systems removed e.g. legacyPackages.zlib
, nain4
, pet-materials
, ...
}: let
  inherit (nixpkgs.legacyPackages) pkgs;
  inherit (import ./helpers.nix {inherit pkgs;}) shell-shared;
  inherit (nain4.deps) args-from-cli make-app;
  petmat = pet-materials.packages.pet-materials;
  extra-packages = [ petmat pkgs.hdf5-cpp pkgs.hdf5-cpp.dev pkgs.highfive ];
  in {

    packages.default = self.packages.fullpet;

    # Executed by `nix run <URL of this flake>#fullpet -- <args?>`
    # TODO: switch to clang environment
    packages.fullpet = pkgs.stdenv.mkDerivation {
      pname = "fullpet";
      version = "0.0.0";
      src = "${self}/src";
      postInstall = "${pkgs.coreutils}/bin/cp -r ${self}/macs $out";
      nativeBuildInputs = [];
      buildInputs = [ nain4.packages.nain4 ] ++ extra-packages;
    };

    # Executed by `nix run <URL of this flake> -- <args?>`
    apps.default = self.apps.fullpet;

    # Executed by `nix run <URL of this flake>#fullpet`
    apps.fullpet = make-app {
      executable = "fullpet";
      args = "--macro-path ${self}/macs ${args-from-cli}";
      package = self.packages.default;
    };

    # Used by `direnv` when entering this directory (also by `nix develop <URL to this flake>`)
    devShell = self.devShells.clang;

    # Activated by `nix develop <URL to this flake>#clang`
    devShells.clang = pkgs.mkShell.override { stdenv = nain4.packages.clang_16.stdenv; } (shell-shared // {
      name = "fullpet-clang-devenv";
      packages = nain4.deps.dev-shell-packages ++ extra-packages ++ [ nain4.packages.clang_16 ];
    });

    # Activated by `nix develop <URL to this flake>#gcc`
    devShells.gcc = pkgs.mkShell (shell-shared // {
      name = "fullpet-gcc-devenv";
      packages = nain4.deps.dev-shell-packages ++ extra-packages;
    });

    # 1. `nix build` .#singularity
    # 2. `scp result <me>@lxplus7.cern.ch:hello.img`
    # 3. [on lxplus] `singularity run hello.img`
    packages.singularity = pkgs.singularity-tools.buildImage {
      name = "fullpet";
      contents = [ self.apps.fullpet.program ];
      runScript = "${self.apps.fullpet.program} $@";
      diskSize = 10240;
      memSize = 5120;
    };

  }
