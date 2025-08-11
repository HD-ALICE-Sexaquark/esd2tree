#ifndef E2V_CUTS_H
#define E2V_CUTS_H

#include <AliESDtrack.h>

#include <TMath.h>

#include <AliESDtrack.h>

namespace Cuts {

enum EVertexType { VertexTracks = 0x1, VertexSPD = 0x2, VertexTPC = 0x4 };

namespace Event {
static constexpr double AbsMax_PV_Zv{10.};
}  // namespace Event

namespace MC {
static constexpr double Min_Momentum{0.01};
}  // namespace MC

namespace Track {
static constexpr double AbsMax_NSigma_Pion{3.};
static constexpr double AbsMax_NSigma_Kaon{3.};
static constexpr double AbsMax_NSigma_Proton{3.};
static constexpr bool TurnedOn_StatusCuts{true};
static constexpr bool RejectITS{true};
static constexpr bool StatusTPC{true};

// copied from `AliESDtrackCuts` //

// nomenclature
// - d : default in `AliESDtrackCuts` constructor
// - t : value that turns on a cut
// - a : StandardTPCOnlyTrackCuts
// - b : StandardITSTPCTrackCuts2015PbPb

static constexpr bool RequireTPCRefit{false};                              // (d=false) require TPC refit
static constexpr bool RequireTPCStandalone{true};                          // (d=false) require TPC standalone tracks
static constexpr int Min_NClusterTPC{50};                                  // (d=-1)(a=50)
static constexpr double Max_Chi2PerClusterTPC{2.};                         // (d=1E10)(a=4)
static constexpr double Max_C11{1E10};                                     // (d=1E10)
static constexpr double Max_C22{1E10};                                     // (d=1E10)
static constexpr double Max_C33{1E10};                                     // (d=1E10)
static constexpr double Max_C44{1E10};                                     // (d=1E10)
static constexpr double Max_C55{1E10};                                     // (d=1E10)
static constexpr bool RejectKinks{true};                                   // (d=false)(a=true)
static constexpr double Min_P{0.01};                                       // (d=0.) minimum total momentum
static constexpr double Max_P{100};                                        // (d=1E10) maximum total momentum
static constexpr double Min_Pt{0.01};                                      // (d=0.) minimum transverse momentum
static constexpr double Max_Pt{100};                                       // (d=1E10) maximum transverse momentum
static constexpr double Min_Px{0.01};                                      // (d=-1E10) minimum
static constexpr double Max_Px{100};                                       // (d=1E10) maximum
static constexpr double Min_Py{0.01};                                      // (d=-1E10) minimum
static constexpr double Max_Py{100};                                       // (d=1E10) maximum
static constexpr double Min_Pz{0.01};                                      // (d=-1E10) minimum
static constexpr double Max_Pz{100};                                       // (d=1E10) maximum
static constexpr double AbsMax_Eta{1.};                                    // (|d|=1E10 ??) eta range
static constexpr bool DoDCACut_AsEllipse{false};                           // (d=false)(a=true)
static constexpr double Max_DCAxy_wrtPV{2.};                               // (d=1E10)(a=2.4)
static constexpr double Max_DCAz_wrtPV{2.};                                // (d=1E10)(a=3.2)
static constexpr bool DoDCACut_AsRectangle{false};                         // (d=true)
static constexpr double Min_DCAxy_wrtPV{2.};                               // (d=0.)
static constexpr double Min_DCAz_wrtPV{2.};                                // (d=0.)
static constexpr bool DoDCACut_AsCircle{false};                            // (d=false)
static constexpr double Min_DCA_wrtPV{2.};                                 // (d=0.)
static constexpr double Max_DCA_wrtPV{2.};                                 // (d=1E10)
static constexpr bool RejectSharedTPCClusters{false};                      // (d=false)
static constexpr double Max_FractionSharedTPCClusters{1E10};               // (d=1E10)
static constexpr float Min_NCrossedRowsTPC{-1.};                           // (d=-1)
static constexpr double Min_RatioCrossedRowsOverFindableClustersTPC{-1.};  // (d=-1)

static constexpr bool RequireNSigmaToVertex{true};  // (d=true)
static constexpr double Max_NSigmaToVertex{2.};     // (d=1E10)

static constexpr double Max_Chi2TPCConstrainedVsGlobal{};              // (d=1E10)(t<1E9)
static constexpr EVertexType VertexType_Chi2TPCConstrainedVsGlobal{};  // d(kVertexTracks | kVertexSPD)

// mininum length (in cm) over which the track is sampled in the active volume of the TPC (outside boundaries)
static constexpr double Min_LengthActiveVolumeTPC{};  // (d=120., but never assigned)(t>1.)

// Set combination of the TPC geometrical cut, cut on number of crossed rows and cut on the clusters
// The goal of cut is to avoid of the dead zone regions
// - JIRA discussion and links to presentations -  https://alice.its.cern.ch/jira/browse/ATO-233
// Cut to be used for measurement where the homogenous coverage are not required
// In case good momentum resolution and description of the matching efficiency needed, also for analysis requiring homogenous phi coverage,
// this cut to be considered ??
// Note: this is replacement of single cut ??
static constexpr double DeadZoneWidth{};         // (b=2.) width of the TPC dead zone (missing pads + PRF +ExB)
static constexpr double GeoNcrNclLength{};       // (b=130.)(t>0) cut on the geometical length condition Ngeom(cm)>cutGeoNcrNclLength default=130
static constexpr double GeoNcrNclGeom1Pt{};      // (b=1.5) 1/pt dependence slope
                                                 // cutGeoNcrNclLength:=fCutGeoNcrNclLength-abs(1/pt)^fCutGeoNcrNclGeom1Pt
static constexpr double GeoNcrNclFractionNcr{};  // (b=0.9) relative fraction cut Ncr condition
                                                 // Ncr>cutGeoNcrNclFractionNcr*fCutGeoNcrNclLength
static constexpr double GeoNcrNclFractionNcl{};  // (b=0.7) relative fraction cut Ncr condition Ncl>cutGeoNcrNclFractionNcl

static constexpr bool DistortedRegionTPC{false};  // (d=false)(b=true)
}  // namespace Track

// Return true if the track crosses on of the regions in the TPC in which the field is distorted.
// For the time being, this is a purely geometric cut which will be further refined in the future based on NDregression analysis.
// For the time being, the cuts are hardwired, but will be probably moved to the OADB.
// Rewrite of `AlitrackCuts::IsTrackInDistortedTpcRegion`
inline bool IsTrackInDistortedTpcRegion(const AliESDtrack* track) {

    if (!track->GetInnerParam()) return false;  // non-TPC tracks are not affected

    double eta{track->Eta()};
    double phi_inner{track->GetInnerParam()->Phi()};

    if (eta > 0.) {
        // A-side
        if (1.32 < phi_inner && phi_inner < 1.48) return true;
        if (2.00 < phi_inner && phi_inner < 2.15) return true;
        if (3.07 < phi_inner && phi_inner < 3.21) return true;
    } else {
        // C-side
        if (0.40 < phi_inner && phi_inner < 0.86) return true;
        if (2.10 < phi_inner && phi_inner < 2.34) return true;
        if (3.90 < phi_inner && phi_inner < 4.32) return true;
    }

    return false;
}

// Calculate the number of sigma to the vertex.
// Rewrite of `AlitrackCuts::GetSigmaToVertex`
static double GetSigmaToVertex(const AliESDtrack* const track) {

    float b[2]{};
    float bCov[3]{};
    track->GetImpactParameters(b, bCov);
    if (bCov[0] < 0. || bCov[2] < 0.) {
        bCov[0] = 0.;
        bCov[2] = 0.;
    }

    double bRes[2]{std::sqrt(bCov[0]), std::sqrt(bCov[2])};

    // How to get to a n-sigma cut?
    // The accumulated statistics from 0 to d is
    // ->  Erf(d/Sqrt(2)) for a 1-dim gauss (d = n_sigma)
    // ->  1 - Exp(-d**2) for a 2-dim gauss (d*d = dx*dx + dy*dy != n_sigma)
    // It means that for a 2-dim gauss: n_sigma(d) = Sqrt(2)*ErfInv(1 - Exp((-d**2)/2)
    // Can this be expressed in a different way?
    if (bRes[0] == 0 || bRes[1] == 0) return -1.;

    double d = std::sqrt(std::pow(b[0] / bRes[0], 2) + std::pow(b[1] / bRes[1], 2));

    // workaround precision problem
    // if d is too big, std::exp(...) gets 0, and TMath::ErfInverse(1) that should be infinite, gets 0
    // 1e-15 corresponds to nsigma ~ 7.7
    if (std::exp(-d * d / 2) < 1E-15) return 1'000;

    double nSigma = TMath::ErfInverse(1 - std::exp(-d * d / 2)) * std::sqrt(2);

    return nSigma;
}

}  // namespace Cuts

#endif  // E2V_CUTS_H
