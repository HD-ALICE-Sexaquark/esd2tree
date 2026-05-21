#pragma once

#include <string_view>

namespace Const {

static constexpr int NCovMatrixComponents = 21;
static constexpr int NEventsInDedicatedMC = 10;     // number of events per file from dedicated MC
static constexpr int NReactionsPerEvent = 20;       // number of injected reactions per event in dedicated MC
static constexpr double PdgMass_Lambda = 1.115683;  // PDG mass of lambda (GeV/c^2)
static constexpr double PdgMass_Proton = 0.938272;  // PDG mass of proton (GeV/c^2)
static constexpr double PdgMass_Kaon = 0.493677;    // PDG mass of kaon (GeV/c^2)
static constexpr double PdgMass_Pion = 0.139570;    // PDG mass of pion (GeV/c^2)
static constexpr std::string_view SimLog_EventHeader = "I-AliGenCocktail::Generate: Generator 3: AliGenSexaquarkReaction";
static constexpr std::string_view SimLog_ReactionMarker = "I-AliGenSexaquarkReaction::GenerateN:";

}  // namespace Const
