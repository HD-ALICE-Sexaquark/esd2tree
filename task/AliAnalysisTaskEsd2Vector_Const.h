#ifndef E2V_CONSTANTS_H
#define E2V_CONSTANTS_H

#include <string_view>

namespace Const {
static constexpr int NEventsPerSignalMCLog{10};
static constexpr int NReactionsPerEvent{20};
static constexpr std::string_view SimLog_EventHeader{"I-AliGenCocktail::Generate: Generator 3: AliGenSexaquarkReaction"};
static constexpr std::string_view SimLog_ReactionMarker{"I-AliGenSexaquarkReaction::GenerateN:"};
static constexpr int DummyInt{-1};
static constexpr float DummyFloat{-999.};
}  // namespace Const

#endif  // E2V_CONSTANTS_H
