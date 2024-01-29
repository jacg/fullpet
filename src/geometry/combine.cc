#include "combine.hh"

#include <n4-place.hh>

#include <G4Box.hh>
#include <G4LogicalVolume.hh>

#include <algorithm>

G4VPhysicalVolume* combine_geometries(G4VPhysicalVolume* phantom, G4VPhysicalVolume* detector) {
  auto detector_envelope = detector -> GetLogicalVolume();
  auto phantom_envelope  =  phantom -> GetLogicalVolume();

  // TODO: not general enough: only uses translation ignores other transformations
  // TODO Can we avoid extracting logical and use physical/placement directly ?
  auto phantom_physical    = phantom_envelope -> GetDaughter(0);
  auto phantom_logical     = phantom_physical -> GetLogicalVolume();
  auto phantom_translation = phantom_physical -> GetTranslation();

  // Check whether phantom envelope fits inside detector envelope, with margin.
  auto& pbox = dynamic_cast<G4Box&>(* phantom_envelope -> GetSolid());
  auto& dbox = dynamic_cast<G4Box&>(*detector_envelope -> GetSolid());
  auto expand = false;
  auto make_space = [&expand](auto p, auto d) {
    auto space_needed = std::max(p * 1.1, d);
    if (space_needed > d) { expand = true; }
    return space_needed;
  };
  auto x = make_space(pbox.GetXHalfLength(), dbox.GetXHalfLength());
  auto y = make_space(pbox.GetYHalfLength(), dbox.GetYHalfLength());
  auto z = make_space(pbox.GetZHalfLength(), dbox.GetZHalfLength());
  // Expand detector envelope if needed
  if (expand) {
    auto new_box = new G4Box(detector_envelope->GetName(), x, y, z);
    detector_envelope -> SetSolid(new_box);
  }

  n4::place(phantom_logical).in(detector_envelope).at(phantom_translation).now();
  delete phantom;

  return detector;
};
