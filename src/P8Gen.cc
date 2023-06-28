#include "P8Gen.h"
#include <iostream>
#include "TRandom3.h"

using namespace Pythia8;
using namespace fastjet;
using namespace std;

void P8Gen::init() {
  is_init = true;

  int idA, idB;
  if      (name_type == "pp" )  { idA = 2212; idB = 2212; }
  else if (name_type == "pAu")  { idA = 2212; idB = 1000822080; }
  else if (name_type == "AuAu") { idA = 1000822080; idB = 1000822080; }

  for (auto str : vector<string>{ 
      Form("Beams:eCM = %f",sNN),  // run at 7 GeV for now
      "HardQCD:all = on",
      Form("PhaseSpace:pTHatMin = %f",pTHatMin),
      Form("PhaseSpace:pTHatMax = %f",pTHatMax),
      Form("Beams:idA = %i", idA), // moving in +z direction
      Form("Beams:idB = %i", idB), // moving in +z direction
      /* "Beams:idB = 1000822080", // moving in -z direction AuAu */
      /* "Beams:idB = 2212", // moving in -z direction AuAu */
                             // Au ion (from http://home.thep.lu.se/~torbjorn/pythia82html/BeamParameters.html#section0 )
      "Random:setSeed = on",
      "ParticleDecays:limitRadius = on",
      Form("ParticleDecays:rMax = %f", 10.),
      Form("Random:seed = %i",seed)
  }) pythia.readString(str.c_str());
  pythia.init();
};

pair<vector<PseudoJet> /*charged*/,vector<PseudoJet> /*neutral*/> P8Gen::operator()() {
  assert(is_init);
  bool good_event = pythia.next();
  while (!good_event) {
    /* cout << "Warning: skipping bad `Pythia::next()` return value" << endl; */
    cout << " . ";
    good_event = pythia.next();
  } 

  auto& event = pythia.event ;

  std::vector <fastjet::PseudoJet> ch_vec; // charged vec
  std::vector <fastjet::PseudoJet> ne_vec; // neutral vec

  for (int i {0}; i < event.size(); ++i) {
    if (!event[i].isFinal()) continue;
    auto& e     { event[i] };
    double pAbs { e.pAbs() };
    if (pAbs < minPtCut) continue;
    double eta { e.eta() };
    double pt  { e.pT()  };
    if ( fabs(eta) > maxEta ) continue;
    double phi { e.phi()  };
    // do we want actual location or 
    fastjet::PseudoJet p;
    p.reset_PtYPhiM(pt, eta, phi, usePionMass ? PionMass : e.m());
    (e.isCharged() ? ch_vec : ne_vec).push_back(p);
  }
  return {ch_vec, ne_vec};
};

