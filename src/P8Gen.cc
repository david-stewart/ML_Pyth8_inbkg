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

vector<PseudoJet> P8Gen::operator()() {
  assert(is_init);

  int ntries = 1;
  bool good_event = pythia.next();
  while (!good_event) {
    ++ntries;
    if (ntries > nMaxBadGenerations) {
      cout << " Pythia8 failed to generate a good event " << ntries << " successive times." << endl;
      cout << " Aborting program." << endl;
      assert(ntries <= nMaxBadGenerations);
    }
    good_event = pythia.next();
  } 

  auto& event = pythia.event ;

  std::vector <fastjet::PseudoJet> p_vec; // charged vec

  for (int i {0}; i < event.size(); ++i) {
    if (!event[i].isFinal()) continue;
    auto& e     { event[i] };

    bool isCharged = e.isCharged();
    if ((isCharged  && !collect_charged) 
    ||  (!isCharged && !collect_neutral)) continue;

    double pAbs { e.pAbs() };

    double eta { e.eta() };
    if ( fabs(eta) > maxEta ) continue;

    double pt { e.pT() };
    if (pt < minPtCut) continue;

    double phi { e.phi() };

    fastjet::PseudoJet p;
    p.reset_PtYPhiM(pt, eta, phi, usePionMass ? PionMass : e.m());
    p.set_user_index(static_cast<int>(e.charge()));
    p_vec.push_back(p);
  }
  return p_vec;
  
};

