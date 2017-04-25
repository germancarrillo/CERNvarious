#include "TROOT.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TFile.h"
#include "TStyle.h"
#include "TH2.h"
#include "TMatrixDSym.h"

#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooAddPdf.h"
#include "RooChebychev.h"
#include "RooFitResult.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooExponential.h"
#include "RooProduct.h"
#include "RooMinuit.h"
#include "RooPlot.h"

#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/HybridCalculator.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestPlot.h"

#include "RooStats/NumEventsTestStat.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooStats/SimpleLikelihoodRatioTestStat.h"
#include "RooStats/RatioOfProfiledLikelihoodsTestStat.h"
#include "RooStats/MaxLikelihoodEstimateTestStat.h"
#include "RooStats/NumEventsTestStat.h"

#include "RooStats/HypoTestInverter.h"
#include "RooStats/HypoTestInverterResult.h"
#include "RooStats/HypoTestInverterPlot.h"

using namespace RooFit;
using namespace RooStats;

Int_t limit(TString fName="Comb_v00.root", Double_t mX_n=200.0, Double_t ma_n=2.0, Int_t npoints=100, Double_t poimin=1, Double_t poimax=100, TString wsName="combWS", TString dataName="combData", TString modelSBName="ModelConfig"){
  
  TFile *f = TFile::Open(fName);
  
  RooWorkspace * w = (RooWorkspace*) f->Get(wsName);
  
  //w->Print();

  // Retrieve data and models
  RooAbsData * data = w->data(dataName);
 
  ModelConfig *sbModel = (ModelConfig*) w->obj(modelSBName); 

  // Fix mass values
  w->loadSnapshot("ucmles");

  w->var("mX")->setVal(mX_n);   
  w->var("mX")->setConstant(true);
  
  w->var("ma")->setVal(ma_n);   
  w->var("ma")->setConstant(true);  

  if(!sbModel->GetSnapshot())  sbModel->SetSnapshot( *sbModel->GetParametersOfInterest() );

  ModelConfig *bModel = (ModelConfig*) sbModel->Clone(modelSBName+"_bOnly");

  RooRealVar  *poi = (RooRealVar*) bModel->GetParametersOfInterest()->first();  if(!poi) return 0;
  Double_t oldval = poi->getVal(); 
  poi->setVal(0);
  bModel->SetSnapshot(RooArgSet(*poi));
  poi->setVal(oldval);
  
  sbModel->Print();

  //// (Un)Comment to switch off all NP
  //const RooArgSet * nuisPar  = sbModel->GetNuisanceParameters(); RooStats::SetAllConstant(*nuisPar);
  //const RooArgSet * bnuisPar = bModel ->GetNuisanceParameters(); RooStats::SetAllConstant(*bnuisPar);
  

  // Fit result
  RooFitResult * fitres = sbModel->GetPdf()->fitTo(*data,InitialHesse(false),Hesse(false),Minimizer("Migrad"),Strategy(0),SumW2Error(false),Save(true));

  AsymptoticCalculator *hc = new AsymptoticCalculator(*data, *bModel, *sbModel, true); 
  //hc->SetOneSided(true);  

  // Configure HypoTestInverter
  HypoTestInverter calc(*hc);
  calc.SetConfidenceLevel(0.95);
  calc.UseCLs(true);
  calc.SetVerbose(true);
  
  // Auto or By-hand poi scan
  if(npoints>0){ 
    poi->setMax(poimax);
    calc.SetFixedScan(npoints,poimin,poimax);
  }

  // Launch scan
  HypoTestInverterResult *r = (HypoTestInverterResult*) calc.GetInterval();
  
  // Retreive result
  HypoTestInverterPlot *plot = new HypoTestInverterPlot("Plot","Plot",r);
	
  TString plotname = Form("outputs/plot_mX%.1fGeV_ma%.1fGeV",mX_n,ma_n);  plotname.ReplaceAll(".","_"); 
  
  TCanvas *can = new TCanvas("can","can",0,0,600,800);	
  can->SetLogy();
  plot->Draw("CLs");  
  can->SaveSource(plotname+".C");	
  can->SaveSource(plotname+".png");	  
  can->SaveSource(plotname+".pdf");	
  
  std::cout << " observed limit          " << r->UpperLimit()              << std::endl;
  std::cout << " expected limit (median) " << r->GetExpectedUpperLimit(0)  << std::endl;
  std::cout << " expected limit (-1 sig) " << r->GetExpectedUpperLimit(-1) << std::endl;
  std::cout << " expected limit (+1 sig) " << r->GetExpectedUpperLimit(1)  << std::endl;
  std::cout << " expected limit (-2 sig) " << r->GetExpectedUpperLimit(-2) << std::endl;
  std::cout << " expected limit (+2 sig) " << r->GetExpectedUpperLimit(2)  << std::endl;
  
  return 0;   
}

