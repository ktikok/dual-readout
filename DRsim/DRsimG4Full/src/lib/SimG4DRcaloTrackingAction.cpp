#include "SimG4DRcaloTrackingAction.h"
#include "G4TouchableHandle.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"

namespace drc {

SimG4DRcaloTrackingAction::SimG4DRcaloTrackingAction()
: G4UserTrackingAction(), fTrajectoryType(2), m_thres(10.) {}

void SimG4DRcaloTrackingAction::PreUserTrackingAction(const G4Track* track) {
  const auto& theTouchable = track->GetTouchableHandle();

  if ( theTouchable->GetHistoryDepth()<2 && track->GetKineticEnergy() > m_thres*CLHEP::MeV ) // only track trajectories outside the detector
    fpTrackingManager->SetStoreTrajectory(fTrajectoryType);
  else
    fpTrackingManager->SetStoreTrajectory(0);
}

void SimG4DRcaloTrackingAction::PostUserTrackingAction(const G4Track*) {}

}
