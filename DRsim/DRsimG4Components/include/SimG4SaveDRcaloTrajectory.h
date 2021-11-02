#ifndef SimG4SaveDRcaloTrajectory_h
#define SimG4SaveDRcaloTrajectory_h 1

#include "GaudiAlg/GaudiTool.h"
#include "k4FWCore/DataHandle.h"
#include "k4Interface/ISimG4SaveOutputTool.h"

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

class SimG4SaveDRcaloTrajectory : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveDRcaloTrajectory(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveDRcaloTrajectory();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  DataHandle<edm4hep::SimTrackerHitCollection> m_trajHits{"SimTrajectoryHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::MCParticleCollection> m_trajPtcs{"SimTrajectoryParticles", Gaudi::DataHandle::Writer, this};
};


#endif
