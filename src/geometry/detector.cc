#include "detector.hh"

#include <pet-materials.hh>

#include <n4-material.hh>
#include <n4-shape.hh>

#include <G4SystemOfUnits.hh>

#include <optional>

G4PVPlacement* detector() {
  auto length = 2*m;
  auto d_min  = 70*cm;
  auto thick  = 2*cm;

  auto air            = n4::material("G4_AIR");
  auto scint_material = pm::csi_with_properties(std::nullopt);

  G4Colour faint {0.1, 0.1, 0.1, 0.5};
  auto lab = n4::box("lab").xy(d_min*1.1).z(length*1.1).vis(true, faint).place(air).now();

  G4Colour yellow = {1, 1, 0.2, 0.5};

  n4::tubs("scintillator")
    .r_inner(d_min/2)
    .r_delta(thick)
    .z(length)
    .vis(true, yellow)
    .place(scint_material)
    .in(lab)
    .now();

  return lab;
}
