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

  auto lab = n4::box("lab").half_cube(1.2*d_min).place(air).now();

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
