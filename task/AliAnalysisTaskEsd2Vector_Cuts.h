#ifndef E2V_CUTS_H
#define E2V_CUTS_H

namespace Cuts {

namespace Event {
static constexpr double AbsMax_PV_Zv{10.};
}  // namespace Event

namespace MC {
static constexpr double Min_Momentum{0.01};
}  // namespace MC

namespace Track {
static constexpr double AbsMax_NSigma_PID{3.};  //
static constexpr int Min_NClusterTPC{50};       //
static constexpr double Min_P{0.01};            // minimum total momentum
static constexpr double Max_P{6.};              // maximum total momentum
static constexpr double Min_Pt{0.15};           // minimum transverse momentum
static constexpr double Max_Pt{4.};             // maximum transverse momentum
static constexpr double AbsMax_Pz{4.5};         // maximum longitudinal momentum
static constexpr double AbsMax_Eta{1.};         // eta range
static constexpr double AbsMax_Cov{1E4};        //
}  // namespace Track

}  // namespace Cuts

#endif  // E2V_CUTS_H
