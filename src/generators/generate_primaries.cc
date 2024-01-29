#include "generate_primaries.hh"

#include <n4-inspect.hh>
#include <n4-random.hh>

#include <G4SystemOfUnits.hh>

void generate_back_to_back_511_keV_gammas(G4Event* event, G4ThreeVector position, G4double time) {
  static auto random_dir = n4::random::direction();

  auto gamma  = n4::find_particle("gamma");
  auto p      = 511*keV * random_dir.get();
  auto vertex =      new G4PrimaryVertex(position, time);
  vertex->SetPrimary(new G4PrimaryParticle(gamma,  p.x(),  p.y(),  p.z()));
  vertex->SetPrimary(new G4PrimaryParticle(gamma, -p.x(), -p.y(), -p.z()));
  event -> AddPrimaryVertex(vertex);
}
