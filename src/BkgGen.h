#ifndef BkgGen_h
#define BkgGen_h

// Code slighted refactored from J. Putschke in msg, 2023.06.27

#include "fastjet/PseudoJet.hh"
#include "TRandom3.h"
#include "TF1.h"
#include <vector>

struct BkgGen {
  double maxEta   { 1.0 };
  double dNdEta   { 650 };
  double T        { 0.291 };
  double max_eta  { 1.0  };
  double minPtCut { 0.2 };
  double PionMass { 0.13957 };
  TRandom3 rng;
  TF1* fpt; // =new TF1("fpt","x*exp(-x/[0])",minPtCut,10.0); */
  BkgGen(unsigned int seed);
  std::vector<fastjet::PseudoJet> operator()();
};

#endif
