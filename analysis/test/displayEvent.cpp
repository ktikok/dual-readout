#include "TROOT.h"

void displayEvent(TString filename) {
  TFile* file = TFile::Open(filename);

  auto* ntuple_C = (TNtuple*)file->Get("ntuple_C");
  auto* ntuple_S = (TNtuple*)file->Get("ntuple_S");
  auto* ntuple_sim = (TNtuple*)file->Get("ntuple_sim");
  auto* ntuple_traj = (TNtuple*)file->Get("ntuple_traj");

  Float_t x,y,z,charge,pdgid,x0,y0,z0,px,py,pz;
  ntuple_traj->SetBranchAddress("x",&x);
  ntuple_traj->SetBranchAddress("y",&y);
  ntuple_traj->SetBranchAddress("z",&z);
  ntuple_traj->SetBranchAddress("charge",&charge);
  ntuple_traj->SetBranchAddress("PDG",&pdgid);
  ntuple_traj->SetBranchAddress("x0",&x0);
  ntuple_traj->SetBranchAddress("y0",&y0);
  ntuple_traj->SetBranchAddress("z0",&z0);
  ntuple_traj->SetBranchAddress("px",&px);
  ntuple_traj->SetBranchAddress("py",&py);
  ntuple_traj->SetBranchAddress("pz",&pz);

  float prevPdg = 0.;
  float prevCharge = 0.;
  ROOT::Math::XYZVector prevVtx(0.,0.,0.);
  ROOT::Math::XYZVector prevMom(0.,0.,0.);
  std::vector<TPolyLine3D*> trajs;
  std::vector<std::vector<float>> xs, ys, zs;

  float minMomentum = 0.5; // minimum momentum to be displayed [GeV]

  for (int idx = 0; idx < ntuple_traj->GetEntries(); idx++) {
    ntuple_traj->GetEntry(idx);

    ROOT::Math::XYZVector vtx(x0,y0,z0);
    ROOT::Math::XYZVector mom(px,py,pz);

    if (idx==0) { // first particle, initialize trajectory
      std::vector<float> xx,yy,zz;

      xs.push_back(xx);
      ys.push_back(yy);
      zs.push_back(zz);
    }

    if (mom.Mag2() < minMomentum*minMomentum )
      continue;

    if ( prevVtx!=vtx || prevMom!=mom || ( idx+1 == ntuple_traj->GetEntries() ) ) { // end of the trajectory
      if (idx!=0) {
        trajs.push_back(new TPolyLine3D(zs.back().size(),&(zs.back()[0]),&(ys.back()[0]),&(xs.back()[0]))); // z->y->x (why?)
        trajs.back()->SetLineWidth(2);

        // PDG id
        if ( std::abs(prevPdg)==11. || std::abs(prevPdg)==22. )
          trajs.back()->SetLineColor(kRed-7);
        else if ( std::abs(prevPdg)==13. )
          trajs.back()->SetLineColor(kBlue-7);
        else if ( std::abs(prevPdg)==12. || std::abs(prevPdg)==14. || std::abs(prevPdg)==16. )
          trajs.back()->SetLineColor(kGray);
        else
          trajs.back()->SetLineColor(kCyan-3);

        if ( std::abs(prevCharge)==0. )
          trajs.back()->SetLineStyle(7);

        if ( idx+1 != ntuple_traj->GetEntries() ) { // initialize trajectory for the next particle
          std::vector<float> xx,yy,zz;

          xs.push_back(xx);
          ys.push_back(yy);
          zs.push_back(zz);
        }
      }
    } else { // continued trajectory
      xs.back().push_back(x);
      ys.back().push_back(y);
      zs.back().push_back(z);
    }

    prevMom = mom;
    prevVtx = vtx;
    prevPdg = pdgid;
    prevCharge = charge;
  }

  TCanvas* canv = new TCanvas("3dHits", "3d reconstructed hits", 1000,600);
  canv->Divide(2);

  Int_t paletteBlue[50];
  Double_t Red1[]    = {1.0, 1.0, 0.8, 0.5, 0.0, 0.0};
  Double_t Green1[]  = {1.0, 1.0, 0.8, 0.5, 0.0, 0.0};
  Double_t Blue1[]   = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  Double_t Length1[] = {0.,  0.01, 0.05, .25 , 0.5, 1. };
  Int_t FI1 = TColor::CreateGradientColorTable(6, Length1, Red1, Green1, Blue1, 50);
  for (int i=0;i<50;i++) paletteBlue[i] = FI1+i;

  Int_t paletteRed[50];
  Double_t Red2[]    = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  Double_t Green2[]  = {1.0, 1.0, 0.8, 0.5, 0.0, 0.0};
  Double_t Blue2[]   = {1.0, 1.0, 0.8, 0.5, 0.0, 0.};
  Double_t Length2[] = {0.,  0.01, 0.05, .25 , 0.5, 1. };
  Int_t FI2 = TColor::CreateGradientColorTable(6, Length2, Red2, Green2, Blue2, 50);
  for (int i=0;i<50;i++) paletteRed[i] = FI2+i;

  Int_t paletteBird[50];
  Double_t Red3[]    = {1.0, 1.0, 0.3, 0.3, 0.9, 0.9};
  Double_t Green3[]  = {1.0, 1.0, 0.3, 0.75, 0.9, 0.9};
  Double_t Blue3[]   = {1.0, 1.0, 0.8, 0.3, 0.0, 0.};
  Double_t Length3[] = {0.,  0.01, 0.05, .25 , 0.5, 1. };
  Int_t FI3 = TColor::CreateGradientColorTable(6, Length3, Red3, Green3, Blue3, 50);
  for (int i=0;i<50;i++) paletteBird[i] = FI3+i;

  canv->cd(1);
  TView3D *view = (TView3D*) TView::CreateView(1);
  view->SetRange(-3000,-3000,-3000,3000,3000,3000);
  view->ShowAxis();
  gStyle->SetPalette(50,paletteRed);
  ntuple_S->SetMarkerStyle(kFullDotMedium);
  ntuple_S->Draw("x:y:z:E","","same");
  gStyle->SetPalette(50,paletteBlue);
  ntuple_C->SetMarkerStyle(kFullDotMedium);
  ntuple_C->Draw("x:y:z:E","","same");

  for (unsigned int idx = 0; idx < trajs.size(); idx++)
    trajs.at(idx)->Draw("same");

  canv->cd(2);
  TView3D *view2 = (TView3D*) TView::CreateView(1);
  view2->SetRange(-3000,-3000,-3000,3000,3000,3000);
  view2->ShowAxis();
  gStyle->SetPalette(50,paletteBird);
  ntuple_sim->SetMarkerStyle(kFullDotMedium);
  ntuple_sim->Draw("x:y:z:E","","same");

  for (unsigned int idx = 0; idx < trajs.size(); idx++)
    trajs.at(idx)->Draw("same");
}
