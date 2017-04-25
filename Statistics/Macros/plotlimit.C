#include <stdlib.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TH2.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TRandom.h>
#include <TBenchmark.h>
#include <TCint.h>
#include <RooRandom.h>

#define GeV 1e3

Int_t  tot;

Double_t FMSA[500];

Double_t upper_limit_data[500];
Double_t upper_limit[500];
Double_t m1_sigma_up[500];
Double_t m2_sigma_up[500];
Double_t m1_sigma_down[500];
Double_t m2_sigma_down[500];
Double_t zero[500];
 
#include "inputnumbers.C"

void plotlimit(TString ma="0.7",Double_t lumi=36.7){
  
  for(Int_t e=0;e<tot;e++){
    upper_limit_data[e]=0;
    upper_limit[e]=0;
    m1_sigma_up[e]=0;
    m2_sigma_up[e]=0;
    m1_sigma_down[e]=0;
    m2_sigma_down[e]=0;
    zero[e]=0;
    //--
  }

  inputnumbers();  
  cout<<"Number of poits to plot:"<<tot<<endl;
   
  for(Int_t e=0;e<tot;e++){
    // lumiscale:
    upper_limit[e]  /= sqrt(36.741/3.213);
    m1_sigma_up[e]  /= sqrt(36.741/3.213);
    m1_sigma_down[e]/= sqrt(36.741/3.213);
    m2_sigma_up[e]  /= sqrt(36.741/3.213);
    m2_sigma_down[e]/= sqrt(36.741/3.213);

    //tmp
    m2_sigma_down[e] = upper_limit[e] - m1_sigma_down[e]/2.;

    //
    m1_sigma_up[e]   = m1_sigma_up[e] - upper_limit[e];
    m1_sigma_down[e] = upper_limit[e] - m1_sigma_down[e];
    m2_sigma_up[e]   = m2_sigma_up[e] - upper_limit[e];
  //m2_sigma_down[e] = upper_limit[e] - m2_sigma_down[e];        

  }  
  
  TGraph *likelihd_limit_d = new TGraph(tot,FMSA,upper_limit_data);
  likelihd_limit_d->SetLineColor(kBlack);
  likelihd_limit_d->SetLineWidth(2);
  likelihd_limit_d->SetLineStyle(1);
   
  TGraph *likelihd_limit_c = new TGraph(tot,FMSA,upper_limit);
  likelihd_limit_c->SetLineColor(kBlack);
  likelihd_limit_c->SetLineWidth(2);
  likelihd_limit_c->SetLineStyle(2);
  
  TGraphAsymmErrors *likelihd_limit_1sigma = new  
    TGraphAsymmErrors(tot,FMSA,upper_limit,zero,zero,m1_sigma_down,m1_sigma_up);
  likelihd_limit_1sigma->SetFillColor(kGreen);
  
  TGraphAsymmErrors *likelihd_limit_2sigma = new  
    TGraphAsymmErrors(tot,FMSA,upper_limit,zero,zero,m2_sigma_down,m2_sigma_up);
  likelihd_limit_2sigma->SetFillColor(kYellow);
      
  TMultiGraph *likelihd_limit;
  likelihd_limit = new TMultiGraph("exclusionlimit_p","CL_{S} Exclusion Limits ;m_{H} [GeV]; #sigma_{X#rightarrowaa#rightarrow4#gamma} [pb]");
  
  likelihd_limit->Add(likelihd_limit_2sigma,"E3");  
  likelihd_limit->Add(likelihd_limit_1sigma,"E3");  
  likelihd_limit->Add(likelihd_limit_c, "L");
//likelihd_limit->Add(likelihd_limit_d, "C");

  cout<<"$m_{H}$ & \t Observed \t & \t Expected \t & $-2\\sigma$ \t & $-1\\sigma$ \t & $+1\\sigma$ \t & $+2\\sigma$ \t \\\\ \\hline"<<endl;
  for(Int_t e=0;e<tot;e++){
    cout<<FMSA[e]<<"\t & "
	<<upper_limit_data[e]<<"\t & "
	<<upper_limit[e]<<"\t & "
	<<upper_limit[e]-m2_sigma_down[e]<<"\t & "
	<<upper_limit[e]-m1_sigma_down[e]<<"\t & "
	<<m1_sigma_up[e]+upper_limit[e]<<"\t & "
	<<m2_sigma_up[e]+upper_limit[e]<<"\t & "
	<<"\\\\"<<endl;
  }

  //==============================================================================

  TLegend *legend = new TLegend(.60,.70,.95,.95);
  legend->SetBorderSize(0);
//legend->AddEntry(likelihd_limit_d,"Observed 95% CL limit","PL");
  legend->AddEntry(likelihd_limit_c,"Expected 95% CL limit","L");
  legend->AddEntry(likelihd_limit_1sigma,"#pm1#sigma","F");
  legend->AddEntry(likelihd_limit_2sigma,"#pm2#sigma","F");
  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  TString text; 
  Int_t pos=0; 
  for(Int_t e=0;e<tot;e++){if(pos<upper_limit_data[e]) pos=upper_limit_data[e]; if(pos<m2_sigma_up[e]+upper_limit[e]) pos=m2_sigma_up[e]+upper_limit[e];}
  
  // -----  ATLAS FORMAT -----
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetPalette();
  SetAtlasStyleWZ();
  gStyle->SetOptStat(0);

  
  Double_t ATLASLogx = 0.53;
  Double_t ATLASLogy = 0.89;
  
  TCanvas *can0=new TCanvas("can0","Exclsuion" ,0,0,800,600);
  can0->GetPad(0)->SetLogy();
  likelihd_limit->Draw("A");
  legend->Draw();
  //
  TPaveText *label = new TPaveText(0.18,0.18,0.54,0.42,"brNDC");
  label->SetFillColor(0);  label->SetFillStyle(0);  label->SetBorderSize(0);
  label->AddText("#bf{#it{ATLAS}} Internal");
  label->AddText("#sqrt{s}=13TeV, #scale[0.6]{#int}L dt=36.7fb^{-1}"); 
  label->AddText("H#rightarrowaa#rightarrow4#gamma, m_{a}="+ma+"GeV");
  label->Draw();
  //

  TString fn;
  fn="outputs/limitplot_ma"+ma.ReplaceAll(".","_")+"GeV.png";  can0->SaveAs(fn);
  fn="outputs/limitplot_ma"+ma.ReplaceAll(".","_")+"GeV.eps";  can0->SaveAs(fn);
  fn="outputs/limitplot_ma"+ma.ReplaceAll(".","_")+"GeV.C";    can0->SaveAs(fn);
 
};

void SetAtlasStyleWZ ()
{

  std::cout << "\nApplying ATLAS style settings...\n" << std::endl ;
  TStyle* atlasStyleWZ = AtlasStyleWZ();
}

TStyle* AtlasStyleWZ()
{
  TStyle *atlasStyleWZ = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors

  gStyle->SetOptStat(0);

  Int_t icol=0;
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
  gStyle->SetPaperSize(20,26);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetTitleXOffset(1.3);
  gStyle->SetTitleYOffset(1.3);
  Int_t font=42;
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);
  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2);
  gStyle->SetLineStyleString(2,"[12 12]");
  gStyle->SetEndErrorSize(0.);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(1111111);
  gStyle->SetOptFit(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  return atlasStyleWZ;

}


