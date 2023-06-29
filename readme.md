# Purpose

This code embed Pythia8 jets into a random background isotropically distributed
in eta<|1|, phi\in[-pi,pi]

# How to build

  * Clone this code locally. Generate directories:
      `./bin`
      `./obj`

  * Make a virtual link to an instalation of fastjet3
  * Update `./Makefile` for paths for libraries for `root6`, `Pythia8`, and `fastjet3`
  * run: `./make`

