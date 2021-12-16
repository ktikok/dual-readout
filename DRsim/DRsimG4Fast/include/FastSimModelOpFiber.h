#ifndef FastSimModelOpFiber_h
#define FastSimModelOpFiber_h 1

#include "G4VFastSimulationModel.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4GenericMessenger.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4Material.hh"

class FastSimModelOpFiber : public G4VFastSimulationModel {
public:
  FastSimModelOpFiber(G4String, G4Region*);
  ~FastSimModelOpFiber();

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack&);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);

private:
  void DefineCommands();

  bool checkTotalInternalReflection(const G4Track* track);
  G4double CalculateVelocityForOpticalPhoton(const G4Track* track);
  void setOpBoundaryProc(const G4Track* track);
  void getCoreMaterial(const G4Track* track);
  void reset();

  G4GenericMessenger* fMessenger;
  G4OpBoundaryProcess* pOpBoundaryProc;
  G4Material* fCoreMaterial;
  G4bool fProcAssigned;

  G4int fSafety;
  G4double fTrkLength;
  G4double fNtransport;
  G4double mTransportUnit;
  G4ThreeVector mFiberAxis;
  G4ThreeVector mPrevDir;
  G4ThreeVector mCurrDir;
  G4bool fKill;
  G4int fNtotIntRefl;
  G4int mTrackId;
};

#endif
