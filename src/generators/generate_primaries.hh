#pragma once

#include <G4Event.hh>

void generate_back_to_back_511_keV_gammas(G4Event* event, G4ThreeVector position, G4double time);

template<class PHANTOM>
void generate_primaries(PHANTOM const& phantom, G4Event* event) {
  auto position = phantom.generate_vertex();
  G4double time = 0;
  generate_back_to_back_511_keV_gammas(event, position, time);
}
