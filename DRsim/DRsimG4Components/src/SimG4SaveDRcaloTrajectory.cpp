#include "SimG4SaveDRcaloTrajectory.h"

#include "G4Event.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4TrajectoryContainer.hh"

#include "CLHEP/Units/SystemOfUnits.h"

DECLARE_COMPONENT(SimG4SaveDRcaloTrajectory)

SimG4SaveDRcaloTrajectory::SimG4SaveDRcaloTrajectory(const std::string& aType, const std::string& aName, const IInterface* aParent)
: GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
}

SimG4SaveDRcaloTrajectory::~SimG4SaveDRcaloTrajectory() {}

StatusCode SimG4SaveDRcaloTrajectory::initialize() {
  if (GaudiTool::initialize().isFailure())
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveDRcaloTrajectory::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveDRcaloTrajectory::saveOutput(const G4Event& aEvent) {
  auto* trajContainer = aEvent.GetTrajectoryContainer();

  if ( trajContainer->size() > 0 ) {
    auto* trajHits = m_trajHits.createAndPut();
    auto* trajPtcs = m_trajPtcs.createAndPut();

    for (unsigned int idx = 0; idx < trajContainer->size(); idx++) {
      G4VTrajectory* traj = (*trajContainer)[idx];
      edm4hep::MCParticle edmPtc = trajPtcs->create();
      edmPtc.setPDG(traj->GetPDGEncoding());
      edmPtc.setCharge(traj->GetCharge());

      auto& vtx = traj->GetPoint(0)->GetPosition();
      auto& end = traj->GetPoint(traj->GetPointEntries()-1)->GetPosition();
      auto mom = traj->GetInitialMomentum()*CLHEP::MeV/CLHEP::GeV; // [E] = GeV
      edmPtc.setVertex(edm4hep::Vector3d(vtx.x(),vtx.y(),vtx.z()));
      edmPtc.setEndpoint(edm4hep::Vector3d(end.x(),end.y(),end.z()));
      edmPtc.setMomentum(edm4hep::Vector3f(mom.x(),mom.y(),mom.z()));

      for (int pts = 0; pts < traj->GetPointEntries(); pts++) {
        auto trajPoint = traj->GetPoint(pts)->GetPosition();
        edm4hep::SimTrackerHit edmHit = trajHits->create();
        edmHit.setCellID(0);
        edmHit.setEDep(0.);
        edmHit.setTime(0.);
        edmHit.setPathLength(0.);
        edmHit.setQuality(0);
        edmHit.setPosition(edm4hep::Vector3d(trajPoint.x(),trajPoint.y(),trajPoint.z())); // [L] = mm
        edmHit.setMCParticle(edmPtc);
      }
    }
  }

  return StatusCode::SUCCESS;
}
