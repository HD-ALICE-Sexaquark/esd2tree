#pragma once

namespace Cuts {

namespace Event {
static constexpr double AbsMax_PV_Z = 10.;
}  // namespace Event

namespace Track {
static constexpr double AbsMax_NSigma_PID = 5.;          // PENDING: commented out
static constexpr unsigned short Min_TPC_NClusters = 50;  //
static constexpr double Max_TPC_Chi2NClusters = 8.;      // nominal: 4
static constexpr double Min_P = 0.05;                    // (GeV/c) minimum total momentum
static constexpr double Max_P = 10.;                     // (GeV/c) maximum total momentum
// static constexpr double Min_Pt = 0.15;              // minimum transverse momentum
// static constexpr double Max_Pt = 10.;     // (GeV/c) maximum transverse momentum
// static constexpr double AbsMax_Pz = 10.;  // (GeV/c) maximum longitudinal momentum
static constexpr double AbsMax_Eta = 1.;  // eta range
// static constexpr double AbsMax_Cov = 1E4;           //
// static constexpr double AbsMin_DCAxy = 15.;  // PENDING: turned off for now
// static constexpr double AbsMin_DCAz = 20.;   // PENDING: turned off for now
}  // namespace Track

}  // namespace Cuts
