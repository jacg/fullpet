{
  description = "MC simulation of a full-body PET";

  inputs = {
    nain4        .url     = "github:jacg/nain4";
    pet-materials.url     = "github:gonzaponte/pet-materials";
    nosys        .follows = "nain4/nosys";
    nixpkgs      .follows = "nain4/nixpkgs";
    oldpkgs      .url     = "github:nixos/nixpkgs/nixos-21.11";
  };

  outputs = inputs @ {
    nosys,
    nixpkgs, # <---- This `nixpkgs` still has the `system` e.g. legacyPackages.${system}.zlib
    ...
  }: let outputs = import ./flake/outputs.nix;
         systems = inputs.nain4.contains-systems.systems;
    in nosys (inputs // { inherit systems; }) outputs;

}
