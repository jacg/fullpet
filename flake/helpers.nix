{pkgs} :
{
  shell-shared = {
    G4_DIR = "${pkgs.geant4}";

    shellHook = ''
      export fullpet_lib_PATH=$PWD/install/fullpet/lib
      export LD_LIBRARY_PATH=$fullpet_lib_PATH:$LD_LIBRARY_PATH;
      export PKG_CONFIG_PATH=$fullpet_lib_PATH/pkgconfig:$PKG_CONFIG_PATH;
    '';
  };
}
