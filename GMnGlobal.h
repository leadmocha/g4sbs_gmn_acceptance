#ifndef GMNGLOBAL_H
#define GMNGLOBAL_H

#include "shrinkGMn.h"

typedef struct {
  ev_small_t ev;
  particle_vars_t evars;
  particle_vars_t pvars;
  particle_vars_t nvars;
  bool e_det;
  bool p_det;
  bool n_det;
} GMnData_t;

typedef struct {
  bool passed_test_cuts;
} GMnStatus_t;

typedef struct {
  double sigmaP;
  double sigmaN;
  double solidP;
  double solidN;
} GMnResultsSigmaSol_t;

typedef struct {
  double voff;
  GMnResultsSigmaSol_t sigmaSol;
  //double countN;
  //double countP;
  double pnvsep;  // Average proton to neutron vertical separation
  double pnvsepE; // (and associated error)
  // Results from identification
  double thetaPQRadius;
  std::vector<double> exclusionZoneWidth;
  std::vector<GMnResultsSigmaSol_t> goodIdentify;
  std::vector<GMnResultsSigmaSol_t> badIdentify;
} GMnResults_t;

typedef struct {
  double nx;
  double ny;
  double px;
  double py;
  double theta;
  double phi;
} GMnResolutionSmearing_t;

#endif //G GMNGLOBAL_H
