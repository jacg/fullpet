#include "sanity.hh"

#include <G4SystemOfUnits.hh>

#include <n4-material.hh>
#include <n4-random.hh>
#include <n4-sequences.hh>
#include <n4-shape.hh>

// ===== Asymmetric, cheap to simulate phantom for basic sanity checking ===================
sanity_check_phantom::sanity_check_phantom()
  : sources{{{4*cm, 0   , 0     },
             {0   , 8*cm, 0     },
             {0   , 0   , 12*cm}}}
{}

G4PVPlacement* sanity_check_phantom::geometry() const {
  auto air   = n4::material("G4_AIR");
  auto water = n4::material("G4_WATER");

  G4Colour blue{0, 0, 1, 0.4};
  G4Colour red {1, 0, 0, 0.3};

  auto envelope = n4::box ("Envelope").half_cube(15*cm).vis(true, blue).place (air  ).now();
  auto sphere   = n4::sphere("Sphere").r        ( 2*cm).vis(true, red ).volume(water);

  for (const auto& source : sources) {
    auto [x,y,z] = n4::unpack(source);
    n4::place(sphere).in(envelope).at(x,y,z).now();
  }
  return envelope;
}

G4ThreeVector sanity_check_phantom::generate_vertex() const {
  return sources[n4::random::fair_die(3)];
}
