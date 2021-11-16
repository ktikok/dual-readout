#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

#include "podio/ROOTReader.h"
#include "podio/EventStore.h"

#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TNtuple.h"
#include "Math/Vector3D.h"

#include <iostream>
#include <string>

int main(int , char const *argv[]) {
  TString filename = argv[1];

  auto pReader = std::make_unique<podio::ROOTReader>();
  pReader->openFile(static_cast<std::string>(filename));

  auto pStore = std::make_unique<podio::EventStore>();
  pStore->setReader(pReader.get());

  std::string filenameStd = static_cast<std::string>(filename);
  std::string extension = ".root";
  auto where = filenameStd.find(extension);

  if (where != std::string::npos)
    filenameStd.erase(where, extension.length());

  filename = static_cast<TString>(filenameStd);

  int iEvt = std::atoi(argv[2]);

  pStore->clear();
  pReader->goToEvent(iEvt);

  printf("Running visualizer for %dth event ...\n", iEvt);

  auto& edepHits = pStore->get<edm4hep::SimCalorimeterHitCollection>("SimCalorimeterHits");
  auto& caloHits = pStore->get<edm4hep::CalorimeterHitCollection>("DRcalo2dHits");
  auto& calo3dHits = pStore->get<edm4hep::CalorimeterHitCollection>("DRcalo3dHits");
  auto& sim3dHits = pStore->get<edm4hep::SimCalorimeterHitCollection>("Sim3dCalorimeterHits");
  auto& leakages = pStore->get<edm4hep::MCParticleCollection>("Leakages");
  auto& trajHits = pStore->get<edm4hep::SimTrackerHitCollection>("SimTrajectoryHits");
  auto& trajPtcs = pStore->get<edm4hep::MCParticleCollection>("SimTrajectoryParticles");

  float Edep = 0.;
  for (unsigned int iEdep = 0; iEdep < edepHits.size(); iEdep++)
    Edep += edepHits[iEdep].getEnergy();

  float leakage = 0.;
    for (unsigned int iLeak = 0; iLeak < leakages.size(); iLeak++) {
      auto& momentum = leakages[iLeak].getMomentum();
      ROOT::Math::XYZVectorF vec(momentum.x, momentum.y, momentum.z);
      leakage += std::sqrt(vec.Mag2());
    }

  float en_S = 0.; float en_C = 0.;
  for (unsigned int idx = 0; idx < caloHits.size(); idx++) {
    auto& caloHit = caloHits.at(idx);

    int type = caloHit.getType();
    float en = caloHit.getEnergy();
    type ? en_C += en : en_S += en;
  }

  std::cout << "E_S     = " << en_S << " GeV, " << "E_C    = " << en_C << " GeV" << std::endl;
  std::cout << "E_truth = " << Edep << " GeV, " << "E_leak = " << leakage << " GeV" << std::endl;

  TNtuple* tuple3d_C = new TNtuple("ntuple_C","Cherenkov 3D","x:y:z:E");
  TNtuple* tuple3d_S = new TNtuple("ntuple_S","Scint 3D","x:y:z:E");
  TNtuple* tuple3d_sim = new TNtuple("ntuple_sim","Sim 3D","x:y:z:E");
  TNtuple* tuple3d_traj = new TNtuple("ntuple_traj","Traj 3D","x:y:z:charge:PDG:x0:y0:z0:px:py:pz");

  for (unsigned int idx = 0; idx < calo3dHits.size(); idx++) { // 3d hits have different size
    auto& calo3dHit = calo3dHits.at(idx);
    int type = calo3dHit.getType();
    float en = calo3dHit.getEnergy();
    auto& pos = calo3dHit.getPosition();

    type ? tuple3d_C->Fill(pos.x, pos.y, pos.z, en) : tuple3d_S->Fill(pos.x, pos.y, pos.z, en);
  }

  for (unsigned int idx = 0; idx < sim3dHits.size(); idx++) {
    auto& sim3dHit = sim3dHits.at(idx);
    float en = sim3dHit.getEnergy();
    auto& pos = sim3dHit.getPosition();

    tuple3d_sim->Fill(pos.x, pos.y, pos.z, en);
  }

  for (unsigned int idx = 0; idx < trajHits.size(); idx++) {
    auto& trajHit = trajHits.at(idx);
    auto& pos = trajHit.getPosition();
    auto& ptc = trajHit.getMCParticle();
    auto& vtx = ptc.getVertex();
    auto& mom = ptc.getMomentum();

    tuple3d_traj->Fill(pos.x, pos.y, pos.z, ptc.getCharge(), static_cast<float>(ptc.getPDG()), vtx.x, vtx.y, vtx.z, mom.x, mom.y, mom.z);
  }

  TFile* visFile = new TFile(filename+"_vis"+static_cast<TString>(std::to_string(iEvt))+".root","RECREATE");
  visFile->WriteTObject(tuple3d_C);
  visFile->WriteTObject(tuple3d_S);
  visFile->WriteTObject(tuple3d_sim);
  visFile->WriteTObject(tuple3d_traj);

  visFile->Close();

  return 0;
}
