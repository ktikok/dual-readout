#ifndef SimG4DRcaloTrackingAction_h
#define SimG4DRcaloTrackingAction_h 1

#include "G4UserTrackingAction.hh"

namespace drc {

class SimG4DRcaloTrackingAction : public G4UserTrackingAction {
public:
  SimG4DRcaloTrackingAction();
  virtual ~SimG4DRcaloTrackingAction() {}

  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);

  void SetTrajectoryType(int trajType) { fTrajectoryType = trajType; }
  void SetTrajectoryThres(const double thres) { m_thres = thres; }

private:
  int fTrajectoryType;
  int m_thres; // minimum kinetic energy of the trajectory in MeV
};

}

#endif
