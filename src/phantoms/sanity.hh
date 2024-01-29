#pragma once

#include <generators/generate_primaries.hh>

#include <G4PVPlacement.hh>

#include <vector>

class G4Event;

class sanity_check_phantom {
public:
  sanity_check_phantom();
  void generate_primaries(G4Event* event) const { return ::generate_primaries(*this, event); }
  G4ThreeVector generate_vertex()         const;
  G4PVPlacement* geometry()               const;
private:
  std::vector<G4ThreeVector> sources;
};
