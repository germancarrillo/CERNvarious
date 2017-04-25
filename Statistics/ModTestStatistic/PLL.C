//////////////////////////////////////////////////////////////////////////
//
// 06/2016 - G CARRILLO-MONTOYA  PLL (and Test-Stat for H->yy and H->4y)   
// 
/////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooConstVar.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "RooMinuit.h"
#include "RooProfileLL.h"
#include "RooRandom.h"
#include "RooPlot.h"
 
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TLegend.h"

#include "AtlasStyle.C"

using namespace RooFit;

#define NToys 1

void PLL(Double_t mu=1, Double_t ma=0.1, Double_t seedi=1234){   

  TString ma_str = Form("%.1f",ma); 
  TString mu_str = Form("%.1f",mu); 

  SetAtlasStyle();

  RooRandom::randomGenerator()->SetSeed(seedi); 
 
  /*
  //
  //Dummy Signal and Background Models:  
  //  
  // Background Models 
  RooRealVar bexp("expcoef","exp coeficient bkg",-1,-2,-0.5);
  RooExponential  bkg("bkg","bkg",x,bexp);
  // Alt Hypo: 
  RooRealVar meansig1("meansig1","mean of Signal1",10,9,10);
  RooRealVar sigmasig1("sigmasig1","width of Signal1",3*10*ma,2,5*10*ma);
  RooGaussian sig1("sig1","sig1",x,meansig1,sigmasig1);
  // Null Hype (closer to bkg):
  RooRealVar meansig2("meansig2","mean of Signal2",0,0,1);
  RooRealVar sigmasig2("sigmasig2","width of Signal2",3,2,5);
  RooGaussian sig2("sig2","sig2",x,meansig2,sigmasig2);
  */  

  // Observable 1D for the monent
  //
  //Temp Signal and Background Models (just using fside)   
  //
  
  TFile *input = TFile::Open("inputs/allhists.root");

  // Background Models 
  TH1F *bkg_hist = (TH1F*)  input->Get("sherpaDiPhoton_leadingPhoton_fracs1_700_800");
  RooRealVar x("x","F_{side} leading #gamma",bkg_hist->GetXaxis()->GetBinLowEdge(0),bkg_hist->GetXaxis()->GetBinUpEdge(bkg_hist->GetNbinsX()));
  RooDataHist bkg_d("bkg_d","bkg_d",x,bkg_hist);
  RooHistPdf bkg("bkg","bkg",x,bkg_d);
 
  // Alt Hypo: 
  TH1F *sig_alte_hist = (TH1F*)  input->Get("FourGammaReso_leadingPhoton_fracs1_700_800");
  RooDataHist sig_alte_d("sig_alte_d","sig_alte_d",x,sig_alte_hist);
  RooHistPdf sig_alte("sig_alte","sig_alte",x,sig_alte_d);
  
  // Null Hype:
  TH1F *sig_null_hist = (TH1F*)  input->Get("TwoGammaReso_leadingPhoton_fracs1_700_800");
  RooDataHist sig_null_d("sig_null_d","sig_null_d",x,sig_null_hist);
  RooHistPdf sig_null("sig_null","sig_null",x,sig_null_d);
  
  //
  Double_t nbkgP=bkg_hist->Integral();   // Number of predicted Background Events 
  Double_t nsigP=10;                     // Number of predicted Signal Events 

  // Yields, same for both signal models
  RooRealVar nbkg("nbkg","#background events",nbkgP);
  RooRealVar nsig("nsig","#signal  events",nsigP*mu);      

  RooAddPdf  model_alte("model_alte","model_alte",RooArgList(bkg,sig_alte),RooArgList(nbkg,nsig));
  RooAddPdf  model_null("model_null","model_null",RooArgList(bkg,sig_null),RooArgList(nbkg,nsig));
          

  TString fname = Form("outputs/toy_ma"+ma_str+"_mu"+mu_str+"_%d.root",int(seedi));  
  TFile *f = TFile::Open(fname,"RECREATE");
  fname = "T_q_mu";
  TTree *T = new TTree(fname,fname);
  // Variables to keep in output Tree:
  Double_t mu_hat_alte_n  = 0; T->Branch("mu_hat_alte" ,&mu_hat_alte_n ,"mu_hat_alte/D" ); mu_hat_alte_n  = 0;   Double_t mu_hat_null_n  = 0; T->Branch("mu_hat_null" ,&mu_hat_null_n ,"mu_hat_null/D" ); mu_hat_null_n  = 0;
  Double_t nll_alte_1_n   = 0; T->Branch("nll_alte_1"  ,&nll_alte_1_n  ,"nll_alte_1/D"  ); nll_alte_1_n   = 0;   Double_t nll_null_1_n   = 0; T->Branch("nll_null_1"  ,&nll_null_1_n  ,"nll_null_1/D"  ); nll_null_1_n   = 0;
  Double_t nll_alte_2_n   = 0; T->Branch("nll_alte_2"  ,&nll_alte_2_n  ,"nll_alte_2/D"  ); nll_alte_2_n   = 0;   Double_t nll_null_2_n   = 0; T->Branch("nll_null_2"  ,&nll_null_2_n  ,"nll_null_2/D"  ); nll_null_2_n   = 0;
  Double_t pll_alte_1_n   = 0; T->Branch("pll_alte_1"  ,&pll_alte_1_n  ,"pll_alte_1/D"  ); pll_alte_1_n   = 0;   Double_t pll_null_1_n   = 0; T->Branch("pll_null_1"  ,&pll_null_1_n  ,"pll_null_1/D"  ); pll_null_1_n   = 0;
  Double_t pll_alte_2_n   = 0; T->Branch("pll_alte_2"  ,&pll_alte_2_n  ,"pll_alte_2/D"  ); pll_alte_2_n   = 0;   Double_t pll_null_2_n   = 0; T->Branch("pll_null_2"  ,&pll_null_2_n  ,"pll_null_2/D"  ); pll_null_2_n   = 0;
  Double_t q_mu_alte_1_n  = 0; T->Branch("q_mu_alte_1" ,&q_mu_alte_1_n ,"q_mu_alte_1/D" ); q_mu_alte_1_n  = 0;   Double_t q_mu_null_1_n  = 0; T->Branch("q_mu_null_1" ,&q_mu_null_1_n ,"q_mu_null_1/D" ); q_mu_null_1_n  = 0;
  Double_t q_mu_alte_2_n  = 0; T->Branch("q_mu_alte_2" ,&q_mu_alte_2_n ,"q_mu_alte_2/D" ); q_mu_alte_2_n  = 0;   Double_t q_mu_null_2_n  = 0; T->Branch("q_mu_null_2" ,&q_mu_null_2_n ,"q_mu_null_2/D" ); q_mu_null_2_n  = 0;
  Double_t q_tilde_alte_n = 0; T->Branch("q_tilde_alte",&q_tilde_alte_n,"q_tilde_alte/D"); q_tilde_alte_n = 0;   Double_t q_tilde_null_n = 0; T->Branch("q_tilde_null",&q_tilde_null_n,"q_tilde_null/D"); q_tilde_null_n = 0;
  
  ofstream logfile; logfile.open(Form("outputs/file_ma"+ma_str+"_mu"+mu_str+"_%d.txt",int(seedi)));    
  
  for(Int_t i=0; i<NToys; i++){

    // Generate pseudoexperiment with N expected entries
    RooDataSet* data_alte = model_alte.generate(x,nsig.getVal()+nbkg.getVal());
    RooDataSet* data_null = model_null.generate(x,nsig.getVal()+nbkg.getVal());

    // ---------------------------------------------------
    // Construct likelihoods
    RooAbsReal* nll_alte_1 = model_null.createNLL(*data_alte,NumCPU(2));  
    RooAbsReal* nll_alte_2 = model_alte.createNLL(*data_alte,NumCPU(2));  

    RooAbsReal* nll_null_1 = model_null.createNLL(*data_null,NumCPU(2));  
    RooAbsReal* nll_null_2 = model_alte.createNLL(*data_null,NumCPU(2));  

    // Minimize likelihood w.r.t all parameters before making plots
    RooMinuit(*nll_alte_1).migrad();
    RooMinuit(*nll_alte_2).migrad();
		         
    RooMinuit(*nll_null_1).migrad();
    RooMinuit(*nll_null_2).migrad();

    // -----------------------------------------------------------------------
    // The profile likelihood estimator on nll for nsig will minimize nll w.r.t
    // all floating parameters except nsig for each evaluation
    RooProfileLL* pll_alte_1  = (RooProfileLL*) nll_alte_1->createProfile(nsig); 
    RooProfileLL* pll_alte_2  = (RooProfileLL*) nll_alte_2->createProfile(nsig); 

    RooProfileLL* pll_null_1  = (RooProfileLL*) nll_null_1->createProfile(nsig); 
    RooProfileLL* pll_null_2  = (RooProfileLL*) nll_null_2->createProfile(nsig); 

    //
    // Test statistics:
    if(pll_alte_1->getVal()<0 || pll_alte_2->getVal()<0 || pll_null_1->getVal()<0 || pll_null_2->getVal()<0){ i--; continue;} // No negative pll

    mu_hat_alte_n  = pll_alte_1->bestFitObs().getRealValue("nsig")/ nsigP;   
    pll_alte_1_n   = pll_alte_1->getVal(); 
    pll_alte_2_n   = pll_alte_2->getVal();               		         
    nll_alte_1_n   = nll_alte_1->getVal(); 
    nll_alte_2_n   = nll_alte_2->getVal();               		         
    q_mu_alte_1_n  = -2*log(pll_alte_1->getVal());
    //q_mu_alte_1_n  = mu_hat1 <= mu ? -2*log(pll_alte_1->getVal()):0; 
    q_mu_alte_2_n  = -2*log(pll_alte_2->getVal());
    //q_mu_alte_2_n  = mu_hat1 <= mu ? -2*log(pll_alte_2->getVal()):0; 
    q_tilde_alte_n = -2*log(pll_alte_1->getVal()/pll_alte_2->getVal());

    mu_hat_null_n  = pll_null_1->bestFitObs().getRealValue("nsig")/ nsigP;   
    pll_null_1_n   = pll_null_1->getVal(); 
    pll_null_2_n   = pll_null_2->getVal();               		         
    nll_null_1_n   = nll_null_1->getVal(); 
    nll_null_2_n   = nll_null_2->getVal();               		         
    q_mu_null_1_n  = -2*log(pll_null_1->getVal());
    //q_mu_null_1_n  = mu_hat1 <= mu ? -2*log(pll_null_1->getVal()):0; 
    q_mu_null_2_n  = -2*log(pll_null_2->getVal());
    //q_mu_null_2_n  = mu_hat1 <= mu ? -2*log(pll_null_2->getVal()):0; 
    q_tilde_null_n = -2*log(pll_null_1->getVal()/pll_null_2->getVal());

    logfile<<"\t mu_hat_alte_n: " <<mu_hat_alte_n 
 	   <<"\t pll_alte_1_n: "  <<pll_alte_1_n  
 	   <<"\t pll_alte_2_n: "  <<pll_alte_2_n  
 	   <<"\t nll_alte_1_n: "  <<nll_alte_1_n  
 	   <<"\t nll_alte_2_n: "  <<nll_alte_2_n  
 	   <<"\t q_mu_alte_1_n: " <<q_mu_alte_1_n 
 	   <<"\t q_mu_alte_2_n: " <<q_mu_alte_2_n 
 	   <<"\t q_tilde_alte_n: "<<q_tilde_alte_n
 	   <<"\t \t"
 	   <<"\t mu_hat_null_n: " <<mu_hat_null_n 
 	   <<"\t pll_null_1_n: "  <<pll_null_1_n  
 	   <<"\t pll_null_2_n: "  <<pll_null_2_n  
 	   <<"\t nll_null_1_n: "  <<nll_null_1_n  
 	   <<"\t nll_null_2_n: "  <<nll_null_2_n  
 	   <<"\t q_mu_null_1_n: " <<q_mu_null_1_n 
 	   <<"\t q_mu_null_2_n: " <<q_mu_null_2_n 
 	   <<"\t q_tilde_null_n: "<<q_tilde_null_n
 	   <<endl;

    if(i==0){
      TPaveText *label = new TPaveText(0.18,0.79,0.49,0.94,"brNDC");
      label->SetFillColor(0);  label->SetFillStyle(0);  label->SetBorderSize(0);
      label->AddText("#bf{#it{ATLAS}} Internal");
      //label->AddText("#sqrt{s}=13TeV, #scale[0.6]{#int}L dt=3.2+0.41fb^{-1}");
      label->AddText("H#rightarrowaa#rightarrow4#gamma");

      TH1F *s1h  = new TH1F("s1h","s1h",1,0,1); s1h->SetLineColor(kRed);   s1h->SetLineWidth(2);   
      TH1F *s2h  = new TH1F("s2h","s2h",1,0,1); s2h->SetLineColor(kBlue);  s2h->SetLineWidth(2);          
      TH1F *sbh  = new TH1F("sbh","sbh",1,0,1); sbh->SetLineColor(kBlack); sbh->SetLineWidth(2); sbh->SetMarkerStyle(20);     
      TH1F *bh   = new TH1F("bh" ,"bh" ,1,0,1); bh ->SetLineColor(kBlack); bh ->SetLineWidth(2); bh->SetLineStyle(2);        
      
      TCanvas *can1 = new TCanvas("fit","fit",1200, 600);
      can1->Divide(2,1);
      can1->cd(1);
      // Alternate
      TLegend *leg1 = new TLegend(0.65,0.80,0.93,0.93,NULL,"brNDC"); 
      leg1->SetFillColor(0);  
      leg1->SetFillStyle(0); 
      leg1->SetBorderSize(0);      
      leg1->AddEntry(s1h,"H#rightarrow4#gamma","L");
      leg1->AddEntry(bh,"Backround","L");
      leg1->AddEntry(sbh,"Signal+Bkg","L");
      leg1->AddEntry(sbh,"Pseudodata","LPE");
      RooPlot* xframe1 = x.frame();
      data_alte->plotOn(xframe1,Binning(bkg_hist->GetNbinsX()));
      model_alte.plotOn(xframe1,Components(bkg),LineColor(kBlack),LineStyle(2));
      model_alte.plotOn(xframe1,Components(sig_alte),LineColor(kRed),LineStyle(1));
      model_alte.plotOn(xframe1,LineColor(kBlack));     
      xframe1->SetMinimum(0.0001);
      xframe1->Draw();
      label->Draw();
      leg1->Draw();
      can1->cd(2);
      // Null
      TLegend *leg2 = new TLegend(0.65,0.80,0.93,0.93,NULL,"brNDC"); 
      leg2->SetFillColor(0);  
      leg2->SetFillStyle(0); 
      leg2->SetBorderSize(0);      
      leg2->AddEntry(s2h,"H#rightarrow#gamma#gamma","L");
      leg2->AddEntry(bh,"Backround","L");
      leg2->AddEntry(sbh,"Signal+Bkg","L");
      leg2->AddEntry(sbh,"Pseudodata","LPE");
      RooPlot* xframe2 = x.frame();
      data_null->plotOn(xframe2,Binning(bkg_hist->GetNbinsX()));
      model_null.plotOn(xframe2,Components(bkg),LineColor(kBlack),LineStyle(2));
      model_null.plotOn(xframe2,Components(sig_null),LineColor(kBlue),LineStyle(1));
      model_null.plotOn(xframe2,LineColor(kBlack));
      xframe2->SetMinimum(0.0001);
      xframe2->Draw();
      label->Draw();       
      leg2->Draw();
      can1->SaveAs("outputs/fit_ma"+ma_str+"_mu"+mu_str+".pdf");
    }

    T->Fill();       
    delete nll_alte_1;
    delete nll_alte_2;
    delete pll_alte_1;
    delete pll_alte_2;
    delete nll_null_1;
    delete nll_null_2;
    delete pll_null_1;
    delete pll_null_2;
  }
  //  
  T->Write();
  f->Close();
  //
  logfile.close();  
  //
  return;
}


