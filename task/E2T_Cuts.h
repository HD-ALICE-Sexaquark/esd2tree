#pragma once

namespace E2T::Cuts {

namespace Track {
static constexpr double AbsMax_NSigmas_PID = 5.;        // PENDING: to tune!
static constexpr unsigned short Min_TPC_NClusters = 0;  // PENDING: to tune! nominal: 50
static constexpr double Max_TPC_Chi2NClusters = 4.;     // nominal quality cut
static constexpr double Min_Pt = 0.15;                  // (GeV/c) minimum transverse momentum // PENDING: to tune!
static constexpr double Max_Pt = 10.;                   // (GeV/c) maximum transverse momentum // PENDING: to tune!
static constexpr double AbsMax_Pz = 10.;                // (GeV/c) maximum longitudinal momentum // PENDING: to tune!
static constexpr double AbsMax_Eta = 1.;                // eta range // PENDING: to tune!
static constexpr double AbsMin_PreDCAxy = 15.;          // (cm) // PENDING: not used yet
static constexpr double AbsMin_PreDCAz = 20.;           // (cm) // PENDING: not used yet
}  // namespace Track

namespace PreFoundLambda {
static constexpr double AbsMax_DeltaInvariantMass = 0.005;  // (GeV/c^2)
static constexpr double AbsSlope_ArmQtOverArmAlpha = 0.2;
static constexpr double AbsMax_NSigmas_PID = 3.;
static constexpr double Max_DcaV0Daughters = 0.1;  // (cm)
static constexpr double Min_Pt = 0.5;              // (GeV/c)
static constexpr double Max_Pt = 3;                // (GeV/c)
static constexpr double Max_DCA_wrt_PV = 0.15;     // (cm)
static constexpr double Min_Decay_Radius2D = 2.5;  // (cm)
static constexpr double Min_Pt_Pion = 0.1;         // (GeV/c)
}  // namespace PreFoundLambda

}  // namespace E2T::Cuts
