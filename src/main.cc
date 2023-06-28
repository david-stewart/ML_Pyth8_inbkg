/*
   This file will do the following:

  Do:
  + Generate truth jet (T) (PYTHIA8) particles
    + Cluster into Truth jets
  + Generate "bkg" particles (B)
    + Add to truth partcles
    + Cluster into "Measured" jets
  + Match truth and embedded jets

*/

#include "BkgGen.h"
#include "P8Gen.h"
#include "JetClusterer.h"
#include "JetIndicesMatcher.h"
#include "RhoMedianBkgClusterer.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TVectorD.h"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

using namespace Pythia8;
using namespace fastjet;
using namespace std;


int main(int nargs, char** argv) {
  /*
   * arguments:
   *   1: number of events
   *   2: ofile name
   *   3: pythia seed
   *   4: background seed
   */

  int n_events          { (nargs>1) ? atoi(argv[1]) : 1 };
  int pyth_seed         { (nargs>2) ? atoi(argv[2]) : 0 }; // -1 is default for Pythia
  unsigned int bkg_seed { static_cast<unsigned int>((nargs>3) ? atoi(argv[3]) :  0) }; //  - is default for TRandom3
  string f_outname      { (nargs>4) ? argv[4] : "sparse_eta_vv_p9" };


  // print arguments:
  bool print_Pythia8_jets { true };
  bool print_Pyth_and_Bkg { true };
  bool print_matched      { true };

  TFile* f_out = new TFile (f_outname.c_str(),"recreate");

  // Pythia 8 particles
  cout << " Pyth8 seed: " << pyth_seed << endl;
  P8Gen p8maker {pyth_seed};
  p8maker.name_type = "pp";
  p8maker.sNN       = 200.;
  p8maker.pTHatMin  = 30.;
  p8maker.pTHatMax  = 2000.;
  p8maker.init();


  // Background particles
  BkgGen bkgmaker { bkg_seed };
  bkgmaker.T = 0.001;

  // JetClusterer
  JetClusterer clusterer {};
  clusterer.calc_area = false;
  clusterer.min_jet_pt = 0.5;
  clusterer.init();

  // i_matcher for jets
  JetIndicesMatcher i_matcher { 0.4 }; 

  // get background rho estimation
  RhoMedianBkgClusterer bkg_est{};
  bkg_est.init();

  for (int nev=0;nev<n_events;++nev) {
    // PYTHIA8 jets
    auto part_P = p8maker();
    auto jets_P = clusterer(part_P.first); // charged jets

    if (print_Pythia8_jets) { 
      cout << " --- Jets from PYTHIA8 --- " << endl;
      for (auto& jet : jets_P) cout << " Jet pt: " << jet.perp() << endl;//<< " area: " << jet.area() << endl;
    }

    // Background added in
    auto part_B = bkgmaker();
    part_B.insert(part_B.end(), part_P.first.begin(), part_P.first.end());
    auto jets_PB = clusterer(part_B);

    if (print_Pyth_and_Bkg) { 
      cout << " --- Pythia8 Jets + background --- " << endl;
      for (auto& jet : jets_PB) cout << " Jet pt: " << jet.perp() << " eta(" << jet.pseudorapidity() << ") phi(" << jet.phi() << ")" << endl;
    }

    // get background estimiation
    cout << " background est: " << bkg_est(part_B) << endl;

    // Match the jets
    i_matcher(jets_P, jets_PB);
    if (print_matched) { 
      cout << " --- matched jets --- " << endl;
      for (auto im : i_matcher.i_matched) {
        int P = im.first;
        int PB = im.second;
        auto& _P = jets_P[P];
        auto& _B = jets_PB[PB];

        float delta_phi = fabs(_P.phi()-_B.phi());
        while (delta_phi > M_PI) delta_phi = fabs(delta_phi - 2*M_PI);

        cout << Form("PJet[%2i] (%5.2f,%5.2f,pT:%5.2f) PBJet[%2i] (%5.2f,%5.2f,pT:%5.2f) Delta(%5.2f,%5.2f,pT:%5.2f,R:%5.3f)",
          P,  _P.pseudorapidity(), _P.phi(), _P.perp(),
          PB, _B.pseudorapidity(), _B.phi(), _B.perp(),
          fabs(_P.pseudorapidity()-_B.pseudorapidity()), delta_phi, _B.perp()-_P.perp(),_P.delta_R(_B)) << endl;
        cout << Form("PJet[%2i] (%5.2f,%5.2f,pT:%5.2f) PBJet[%2i] (%5.2f,%5.2f,pT:%5.2f) Delta(%5.2f,%5.2f,pT:%5.2f,R:%5.3f)",
          P,  _P.rap(), _P.phi(), _P.perp(),
          PB, _B.rap(), _B.phi(), _B.perp(),
          fabs(_P.rap()-_B.rap()), delta_phi, _B.perp()-_P.perp(),_P.delta_R(_B)) << endl;
      }
    }

    
  }

  f_out->Write();
  f_out->Save();
  f_out->Close();

  return 0;
};
