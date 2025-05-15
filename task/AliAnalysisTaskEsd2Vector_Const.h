#ifndef E2V_CONSTANTS_H
#define E2V_CONSTANTS_H

#include <string_view>

namespace Const {
static constexpr int NEventsPerSignalMCLog{10};
static constexpr int NReactionsPerEvent{20};
static constexpr std::string_view SimLog_EventHeader{"I-AliGenCocktail::Generate: Generator 3: AliGenSexaquarkReaction"};
static constexpr std::string_view SimLog_ReactionMarker{"I-AliGenSexaquarkReaction::GenerateN:"};
static constexpr int DummyInt{-1};
static constexpr double DummyFloat{999.};
}  // namespace Const

namespace Cuts {

namespace Event {
static constexpr double AbsMax_PV_Zv{12.};
}  // namespace Event

namespace MC {
static constexpr double Min_Momentum{0.01};
}  // namespace MC

namespace Track {
static constexpr double Min_Pt{1E-2};
static constexpr double Max_Pt{1E2};
static constexpr double AbsMax_NSigma_Pion{3.};
static constexpr double AbsMax_NSigma_Kaon{3.};
static constexpr double AbsMax_NSigma_Proton{3.};
static constexpr double AbsMax_Eta{1.};
static constexpr int Min_NTPCClusters{50};
static constexpr double Max_Chi2PerNTPCClusters{2.};
static constexpr bool TurnedOn_StatusCuts{true};
static constexpr bool TurnedOn_RejectKinks{false};
static constexpr double Min_DCAxy_wrtPV{2.};
}  // namespace Track

}  // namespace Cuts

#endif  // TASKSEXAQUARK_CONSTANTS_H
