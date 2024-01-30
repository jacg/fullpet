#include <geometry/combine.hh>
#include <geometry/detector.hh>
#include <io/hdf5.hh>
#include <phantoms/sanity.hh>

#include <n4-all.hh>

#include <G4GenericMessenger.hh>

#include <G4PrimaryParticle.hh>
#include <G4String.hh>
#include <G4SystemOfUnits.hh>   // physical units such as `m` for metre
#include <G4Event.hh>           // needed to inject primary particles into an event
#include <G4Box.hh>             // for creating shapes in the geometry
#include <G4Sphere.hh>          // for creating shapes in the geometry
#include <FTFP_BERT.hh>         // our choice of physics list
#include <G4RandomDirection.hh> // for launching particles in random directions
#include <G4ThreeVector.hh>
#include <G4Gamma.hh>

#include <cstdlib>
#include <memory>

auto sanity = sanity_check_phantom();

struct my {
  G4double       straw_radius{0.1 * m};
  G4double      bubble_radius{0.2 * m};
  G4double      socket_rot   {-90 * deg};
  G4String      particle_name{"geantino"};
  G4double      particle_energy{511 * keV};
  G4ThreeVector particle_dir {};
};

auto my_generator(const my& my) {
  return [&](auto ev) { return sanity.generate_primaries(ev); };
}

n4::actions* create_actions(my& my, unsigned& n_event) {
  static auto GAMMA = G4Gamma::Definition();
  auto my_stepping_action = [&] (const G4Step* step) {
    // ----- Magic LXe detector --------------------------------------------------------------------
    // 1. Immediately stop any particle that reaches LXe.
    // 2. Record only (a) gammas (b) which have reached LXe
    static G4String scint_name{"scintillator"};
    auto post_step_pt = step -> GetPostStepPoint();
    auto track        = step -> GetTrack();
    auto particle     = track -> GetParticleDefinition();
    auto process_name = post_step_pt -> GetProcessDefinedStep() -> GetProcessName();

    auto pst_physvol = post_step_pt -> GetPhysicalVolume();
    auto volume_name = pst_physvol ? pst_physvol -> GetName() : "None";
    // Stop as soon as LXe reached
    if (volume_name == scint_name) { track -> SetTrackStatus(G4TrackStatus::fStopAndKill); }
    // Write only gammas entering LXe (not expecting anything other than gamma, before LXe)
    if (volume_name != scint_name || particle != GAMMA || process_name != "Transportation" ) return;
    auto pos = post_step_pt -> GetPosition();
    std::cout << volume_name << " " << pos << std::endl;
  };

  auto my_event_action = [&] (const G4Event*) {
     n_event++;
     std::cout << "end of event " << n_event << std::endl;
  };

  return (new n4::        actions{my_generator(my)  })
 -> set( (new n4::   event_action{                  }) -> end(my_event_action) )
 -> set(  new n4::stepping_action{my_stepping_action});
}

auto my_geometry(const my& my) {
  auto r_bub = my.bubble_radius;
  auto r_str = my.straw_radius;
  auto water  = n4::material("G4_WATER");
  auto air    = n4::material("G4_AIR");
  auto steel  = n4::material("G4_STAINLESS-STEEL");
  auto world  = n4::box("world").cube(2*m).x(3*m).volume(water);

  n4::sphere("bubble").r(r_bub)         .place(air).in(world).at  (1.3*m, 0.8*m, 0.3*m).now();
  n4::tubs  ("straw" ).r(r_str).z(1.9*m).place(air).in(world).at_x(0.2*m              ).now();

  n4       ::sphere("socket-cap" ).r(0.3*m).phi_delta(180*deg)
    .sub(n4::box   ("socket-hole").cube(0.4*m))
    .name("socket")
    .place(steel).in(world).rotate_x(my.socket_rot).at(1*m, 0, 0.7*m).now();

  return n4::place(world).now();
}

int main(int argc, char* argv[]) {
  unsigned n_event = 0;

  my my;

  G4int physics_verbosity = 0;

  std::unique_ptr<hdf5_io> writer;

  // The trailing slash after '/my_geometry' is CRUCIAL: without it, the
  // messenger violates the principle of least surprise.
  auto messenger = new G4GenericMessenger{nullptr, "/my/", "docs: bla bla bla"};
  messenger -> DeclarePropertyWithUnit("straw_radius"      , "m"  , my. straw_radius  );
  messenger -> DeclarePropertyWithUnit("bubble_radius"     , "m"  , my.bubble_radius  );
  messenger -> DeclarePropertyWithUnit("socket_rot"        , "deg", my.socket_rot     );
  messenger -> DeclarePropertyWithUnit("particle_energy"   , "keV", my.particle_energy);
  messenger -> DeclareProperty        ("particle"          ,        my.particle_name  );
  messenger -> DeclareProperty        ("particle_direction",        my.particle_dir   );
  messenger -> DeclareProperty        ("physics_verbosity" ,        physics_verbosity );

  n4::run_manager::create()
    .ui("fullpet", argc, argv)
    .macro_path("macs")
    // .apply_command("/my/something value")
    // .apply_early_macro("early-hard-wired.mac")
    // .apply_cli_early() // CLI --early executed at this point
    // .apply_command(...) // also possible after apply_early_macro

    .physics<FTFP_BERT>(physics_verbosity)
     .geometry          ([&] {return combine_geometries(sanity.geometry(), detector()); })
    // .geometry          ([&] {return sanity.geometry(); })
    //.geometry          (detector)
    .actions           (create_actions(my, n_event))

    // .apply_command("/my/particle e-")
    // .apply_late_macro("late-hard-wired.mac")
    // .apply_cli_late() // CLI --late executed at this point
    // .apply_command(...) // also possible after apply_late_macro

    .run();
}
