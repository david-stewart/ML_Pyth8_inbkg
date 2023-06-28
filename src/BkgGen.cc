#include "BkgGen.h"

using namespace fastjet;
using namespace std;

BkgGen::BkgGen(unsigned int seed) 
  : rng { seed }
{ };

vector<PseudoJet> BkgGen::operator()()
{

  if (fpt == nullptr) {
    fpt = new TF1("fpt",Form("x*exp(-x/%10.8f)",T),minPtCut,10.);
  }

  auto nMax { static_cast<int>(2*maxEta*dNdEta*3/2.0) };
  std::vector<fastjet::PseudoJet> eventBkg;
  for (int i=0;i<nMax;i++)
  {
    const double eta {rng.Uniform(-maxEta,maxEta)};
    const double phi {rng.Uniform(-M_PI,M_PI)};
    const double pt  {fpt->GetRandom()};

    fastjet::PseudoJet particleTemp;
    particleTemp.reset_PtYPhiM(pt,eta,phi,PionMass);

    if (rng.Uniform(0,1)<max_eta) //
    {
      if (rng.Uniform(0,1)<0.5)
        particleTemp.set_user_index(1);
      else
        particleTemp.set_user_index(2);
    }
    else particleTemp.set_user_index(0);

    eventBkg.push_back(particleTemp);
  }
  return eventBkg;
}
