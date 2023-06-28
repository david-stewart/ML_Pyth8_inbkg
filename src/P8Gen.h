#ifndef P8Gen_h
#define P8Gen_h

// Code slighted refactored from J. Putschke in msg, 2023.06.27

#include "Pythia8/Pythia.h"
#include "fastjet/PseudoJet.hh"
#include "TRandom3.h"
#include "TF1.h"
#include <vector>

struct P8Gen {
  double sNN      { 200 };
  double pTHatMin { 10. };
  double pTHatMax { 2000. };
  double maxEta   { 1.0 };
  double minPtCut { 0.2 };
  double PionMass { 0.13957 };
  int    seed;
  bool   usePionMass { false }; // will use actual particle mass
  std::string name_type { "pp" };

  Pythia8::Pythia pythia;

  P8Gen(int _seed) : seed {_seed} {};
  bool is_init { false };
  void init();
  std::pair<std::vector<fastjet::PseudoJet>,std::vector<fastjet::PseudoJet>> operator()();
};

#endif
