#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TPad.h>



void Plot_photonEnergy() {
    TFile *f = new TFile("PerEventData_1GeV_withPhoton.root");
    TTree *t = (TTree*)f->Get("Photons");

    double edep;
    t->SetBranchAddress("PhotonEn", &edep);

    TH1F *h = new TH1F("h", "Energy Deposition per Photon ", 100, 0, 0);

    Long64_t nentries = t->GetEntries();

    for (Long64_t i = 0; i < nentries; i++) {
        t->GetEntry(i);
        h->Fill(edep);
    }

    h->GetXaxis()->SetTitle("Energy (MeV)");
    h->GetYaxis()->SetTitle("Counts");
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleSize(0.045);
    h->SetLineWidth(2);
    gStyle->SetOptStat(1110);
    h->Draw();
}
