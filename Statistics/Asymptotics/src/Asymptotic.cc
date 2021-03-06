//
// Asymptotic.cc [CLsb & CLs Limits based on Asymptotic formulae, using ProfileLikelihood method]
// v1:  9 May  2011 : using qmu
// v2: 28 July 2011 : using qmu_tilde
//
// Author: Sw.Banerjee, U.Wisconsin-Madison
// :( based on code snippets stolen from numerous sources :)
//
// Usage: run parallelly with iread = 1 (Observed Data) and iread = 0 (Asimov Data) and finally run with iread = -1
//
// eg, submit 2 jobs [independently of each other] with iread = 0 and iread = 1; wait till they finish; then run with with iread = -1
//
//Argument variables
//
//  imode = (argc>1) ? atoi(argv[1]) :   1; // > 0 : use snapshot ; < 0 : no snapshot [Always try to include snapshot and use imode > 0]
//  imass = (argc>2) ? atoi(argv[2]) : 130; // imass
//  iread = (argc>3) ? atoi(argv[3]) :  -1; // 1: obs; 0: exp; -1: read
//
#define USING_MINUIT2
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "TStyle.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TLine.h"
#include "TSpline.h"
#include "RooAbsData.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooSimultaneous.h"
#include "RooProdPdf.h"
#include "RooNLLVar.h"
#include "RooProfileLL.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRandom.h"
#include "RooMinuit.h"
#include "TRandom3.h"
#include "RooWorkspace.h"
#include "RooStats/RooStatsUtils.h"
#include "RooStats/ModelConfig.h"
#include "TStopwatch.h"
#include "RooMinimizer.h"
#include "Math/MinimizerOptions.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "AsymPow.h"
#include "HZZ2L2QRooPdfs.h"
#include "HZZ4LRooPdfs.h"
#include "VerticalInterpPdf.h"
#include "CommonHead.h"
#include "CommonFunc.h"
//
using namespace std;
using namespace RooFit;
using namespace RooStats;
using namespace CommonFunc;
//
int imass;
int imode;
int iread;
int iprofile;
//
TString pname; // Particuliers Nom
TString fname; // Input File Name
TString wname; // Workspace Name
TString mname; // ModelConfig Name
TString dname; // Dataset Name
TString aname; // AsimovDataset Name
//
RooWorkspace* w(0);
RooDataSet* data(0);
ModelConfig* mc(0);
RooAbsPdf* pdf(0);
RooRealVar* firstPOI(0);
RooDataSet* asimov(0);
//
double confidenceLevel=0.95;
//
// do a preliminary fit to guess the following
//
double xmin=0.5;
double xmax=0.0;
double xstep=0.25; 
int nbin=50; 
double pll_max=20; // has preceedence over nbin
//
const int ngrid=0;
double vecx_grid[7] = { 1, 2, 2.1, 2.98934, 3.13881, 3.57036, 3.74888};
//
void calc_pvalue(const double qmu, const double qmuA,  double& p_clsb, double& p_clb, double& p_cls) {
  // qmu has been sugarcoated to be = 999 if < 0
  double sqrtqmu  = (qmu >0 && qmu <999) ? sqrt(qmu  ) : 0;
  double sqrtqmuA = (qmuA>0 && qmuA<999) ? sqrt(qmuA) : 0; 
  //
  p_clsb = 1;
  p_clb  = 0;
  if (sqrtqmu <= sqrtqmuA) {
    p_clsb -= ROOT::Math::gaussian_cdf( sqrtqmu, 1);
    p_clb   = ROOT::Math::gaussian_cdf( sqrtqmuA - sqrtqmu, 1);
  } else if (sqrtqmuA != 0) {
    p_clsb -= ROOT::Math::gaussian_cdf((sqrtqmuA * sqrtqmuA + sqrtqmu * sqrtqmu)/(2 * sqrtqmuA), 1);
    p_clb   = ROOT::Math::gaussian_cdf((sqrtqmuA * sqrtqmuA - sqrtqmu * sqrtqmu)/(2 * sqrtqmuA), 1);
  }
  p_cls    = (p_clb) ? p_clsb/p_clb : 1;
}
//
double calc_CLsLimit(TString str, std::vector<double> vecpll_obs, std::vector<double> vecpll_exp, std::vector<double> vec_mu_obs, std::vector<double> vec_mu_exp){
  double cls_min(0.001),cls_max(1.0);
  std::vector<double> vecx;
  std::vector<double> vec_pcls;
  std::vector<double> vec_pcls1;
  int nbin_min = min ( vecpll_obs.size(), vecpll_exp.size() ) -1;
  double pcls_last=1;
  double p_clsb = 1;
  double p_clb  = 0;
  double p_cls  = 1;
  for (int ibin=0;ibin<nbin_min;++ibin) {
    double xval = (fabs(vec_mu_obs[ibin] - vec_mu_exp[ibin])<0.001) ? vec_mu_obs[ibin] : -1;
    if (xval==-1) {cout << "ibin = " << ibin << " mismatch between vec_mu_obs = " << vec_mu_obs[ibin]  << " and vec_mu_exp = " << vec_mu_exp[ibin] << endl; return -2;}
    //
    calc_pvalue(2*vecpll_obs[ibin], 2*vecpll_exp[ibin], p_clsb, p_clb, p_cls);
    //
    if (p_cls > cls_min && p_cls < cls_max && ibin>0 && p_cls < pcls_last ) { 
      vecx.push_back(xval);
      vec_pcls.push_back(p_cls); 
      vec_pcls1.push_back(1-p_cls); // trick to make TSpline happy ...
    }
    //   
    if (p_cls > cls_min) {
      cout << str << " : ";
      cout << " ibin = "     << Form("% 3d",ibin) ;
      cout << " xval = "     << Form("% 8.04f",xval);
      cout << " pll_obs = "  << Form("% 8.04f",vecpll_obs[ibin]) ;
      cout << " pll_exp = "  << Form("% 8.04f",vecpll_exp[ibin]) ;
      cout << " p_clsb = "   << Form("%10.6f",p_clsb);
      cout << " p_clb = "    << Form("%10.6f",p_clb);
      cout << " p_cls = "    << Form("%10.6f",p_cls);
      cout << endl;
    }
    //
    pcls_last = p_cls;
  }
  //
  if (vec_pcls1.size()==0) {
    cout << "vec_pcls1.size()==0 ... exiting ... " << endl;
    return -1;
  }
  //
  int nmin=std::min_element(vec_pcls1.begin(), vec_pcls1.end()) - vec_pcls1.begin();
  int nmax=vecx.size();
  cout << "nmin = " << nmin << " nmax - nmin = " << nmax - nmin
       << " vecx = " << vecx.size() << " : " << vecx.front() << " , " << vecx.back() 
       << " [nmin, nmax-nmin) = " <<  vecx[nmin] << " , " << vecx[nmax-nmin-1]
       << " vec_pcls = " << vec_pcls1.size() << " : " << vec_pcls.front() << " , " << vec_pcls.back() 
       << " [nmin, nmax-nmin) = " <<  vec_pcls[nmin] << " , " << vec_pcls[nmax-nmin-1]
       << endl;
  //
  //  TSpline5 spline_cls("spline_cls",&vec_pcls1[nmin],&vecx[nmin],nmax-nmin);
  TGraph graph_transpose(nmax-nmin,&vec_pcls1[nmin],&vecx[nmin]);
  double CLsLimit = graph_transpose.Eval(confidenceLevel,0,""); // spline_cls.Eval(confidenceLevel);
  //
  TCanvas* tc = new TCanvas("tc","tc",400,400);
  TGraph graph(nmax-nmin,&vecx[nmin],&vec_pcls[nmin]);
  if (str.Contains("obs")) graph.SetTitle(Form("CLs Scan vs Parameter of Interest [Observed at %3.3d GeV]",imass));
  if (str.Contains("exp")) graph.SetTitle(Form("CLs Scan vs Parameter of Interest [Expected at %3.3d GeV]",imass));
  graph.SetMinimum(0);
  graph.SetMaximum(cls_max);
  graph.GetXaxis()->SetLimits( graph_transpose.Eval(1-cls_max,0,"") > 0.9 ? 0.9 : graph_transpose.Eval(1-cls_max,0,""), 
                               graph_transpose.Eval(1-cls_min,0,"") < 1.1 ? 1.1 : graph_transpose.Eval(1-cls_min,0,""));
  //  graph.GetXaxis()->SetLimits( vecx[nmin], vecx[nmax-nmin] ) ;
  graph.SetMarkerStyle(20);
  graph.SetMarkerSize(0.5);
  graph.SetMarkerColor(2);
  graph.SetLineColor(4);
  graph.Draw("AL*");
  TLine xline(graph.GetXaxis()->GetXmin(),1-confidenceLevel,graph.GetXaxis()->GetXmax(),1-confidenceLevel); xline.SetLineColor(6); xline.Draw();
  TLine yline(CLsLimit,0,CLsLimit,1-confidenceLevel); yline.SetLineColor(6); yline.Draw();
  TLine yline1(1,0,1,cls_max); yline1.SetLineStyle(9); yline1.SetLineColor(6); yline1.Draw();
  tc->Update();
  PrintCanvas(tc,Form("fig/%s/imass%3.3d_%s_%s",pname.Data(),imass,"cls",str.Data()));
//   cout << "Saving canvas as " << Form("fig/%s/imass%3.3d_%s_%s.eps",pname.Data(),imass,"cls",str.Data()) << endl;
//   tc->SaveAs(Form("fig/%s/imass%3.3d_%s_%s.eps",pname.Data(),imass,"cls",str.Data()));
//   cout << "Saving canvas as " << Form("fig/%s/imass%3.3d_%s_%s.png",pname.Data(),imass,"cls",str.Data()) << endl;
//   tc->SaveAs(Form("fig/%s/imass%3.3d_%s_%s.png",pname.Data(),imass,"cls",str.Data()));
  delete tc;
  //
  return CLsLimit;
}
//
int ReadInputs(){
  //
  if(imode<3 || imode==5){
    wname = "combined";
    mname = "ModelConfig";
    dname = "obsData";
    aname = "asimovData_0";
  }
  else{
    wname = "combWS";
    mname = "ModelConfig";
    dname = "combData";
    aname = "asimovData_0";
  }    
  
  if (iread==1) cout << "Reading workspace = " << wname << " modelconfig = " << mname << " and data   = " << dname << " from file = " << fname << endl;
  if (iread==0) cout << "Reading workspace = " << wname << " modelconfig = " << mname << " and asimov = " << aname << " from file = " << fname << endl;
  //
  TFile *_file0 = TFile::Open(fname);
  //
  w = (RooWorkspace*) _file0->Get(wname);
  //
  mc = (ModelConfig*) w->obj(mname);
  if (mc) {
    pdf = mc->GetPdf();
    cout << "pdf:: " << endl; pdf->Print();
  } else {
    return -1;
  }
  //
  if (iread==1) {
    data = (RooDataSet*)w->data(dname);
    if (data) {
      cout << "Data:: " << endl; data->Print("v");
      cout << "Data:: isWeighted = " << data->isWeighted() << " isNonPoissonWeighted = " << data->isNonPoissonWeighted() << endl;
      cout << "Data:: numEntries = " << data->numEntries() << " sumEntries = " << data->sumEntries() << endl;
    } else {
      return -1;
    }
  }
  //
  if (iread==0) {
    /*
    double muval=0;
    bool doConditional=1;
    RooAbsPdf* pdf = mc->GetPdf();
    data = (RooDataSet*)w->data(dname);
    int status = makeAsimovData(mc, doConditional, w, pdf, data, muval); 
    */
    asimov = (RooDataSet*)w->data(aname);
    if (asimov) {
      cout << "Asimov:: " << endl; asimov->Print("v");
      cout << "Asimov:: isWeighted = " << asimov->isWeighted() << " isNonPoissonWeighted = " << asimov->isNonPoissonWeighted() << endl;
      cout << "Asimov:: numEntries = " << asimov->numEntries() << " sumEntries = " << asimov->sumEntries() << endl;
    } else {
      return -1;//0
    }
  }

  const RooArgSet* Obs = mc->GetObservables(); cout << "Obs:: " << Obs->getSize() << endl; Obs->Print("v");
  const RooArgSet* globalObs = mc->GetGlobalObservables(); cout << "globalObs:: " << globalObs->getSize() << endl; globalObs->Print("v");
  const RooArgSet* nuisPars = mc->GetNuisanceParameters(); cout << "nuisPars:: " << nuisPars->getSize() << endl; nuisPars->Print("v");

  cout << "ParametersOfInterest:: " << mc->GetParametersOfInterest()->getSize() << endl;
  firstPOI = (RooRealVar*) mc->GetParametersOfInterest()->first();
  cout << "firstPOI:: " ; firstPOI->Print();

  return 0;
}
//
int GetPLL(TString str="obs"){
  //
  RooDataSet *thisdata=NULL;
  if (str.Contains("obs")) thisdata = data ;
  if (str.Contains("exp")) thisdata = asimov;
  cout << endl << "str = " << str << " thisdata:: " ; thisdata->Print("v");
  //
  if (imode>0) { // workspaces produced using MakeAsimovData; they contain snapshots of conditional MLE's at mu=0,1 [and nominal one, of course!] 
    if (str.Contains("obs")) {
      cout << str << " :: loading Snapshots for nominalGlobs and nominalNuis " << endl;
      w->loadSnapshot("nominalGlobs");
      w->loadSnapshot("nominalNuis");
    }
    if (str.Contains("exp")) {
      cout << str << " :: loading Snapshots for conditionalGlobs_0 and conditionalNuis_0 " << endl;
      if(iprofile==0){
	cout<<"Using mu = 0"<<endl;
	w->loadSnapshot("conditionalGlobs_0"); // or conditionalGlobs_1 for asimovData_1
	w->loadSnapshot("conditionalNuis_0"); // or conditionalNuis_1 for asimovData_1
      }
      else if(iprofile==1){
	cout<<"Using mu = 1"<<endl;
	w->loadSnapshot("conditionalGlobs_1"); // or conditionalGlobs_1 for asimovData_1
	w->loadSnapshot("conditionalNuis_1"); // or conditionalNuis_1 for asimovData_1
      }	
      else if(iprofile==2){
	cout<<"Using nominal"<<endl;
	w->loadSnapshot("nominalGlobs"); // or conditionalGlobs_1 for asimovData_1
	w->loadSnapshot("nominalNuis"); // or conditionalNuis_1 for asimovData_1
      }		
    }
  }
  //
  firstPOI->setVal(0);  firstPOI->setConstant(0);   firstPOI->setRange(0, 100);  firstPOI->Print();  
  RooArgSet poi(*firstPOI);
  //
  bool doFit=false;
  double ZeroCorrection=0;
  if (doFit) {
    cout << "pdf fitTo data ... " << endl;
    RooArgSet nuis = *mc->GetNuisanceParameters();
    RooNLLVar* nll = (RooNLLVar*) pdf->createNLL(*thisdata, Constrain(nuis), Extended(true), Verbose(kFALSE));
    //  nll->applyWeightSquared(kTRUE);
    //
    RooMinimizer minim(*nll);
    minim.setStrategy(ROOT::Math::MinimizerOptions::DefaultStrategy());
    minim.setPrintLevel(ROOT::Math::MinimizerOptions::DefaultPrintLevel());
    int fitstatus = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    cout << str << ":: fitstatus = " << fitstatus << endl;
    //
    RooRealVar* firstPOI_fitted = (RooRealVar*)firstPOI->Clone("firstPOI_fitted");
    firstPOI_fitted->Print();
    double POIFitVal = firstPOI->getVal(); 
    double POIFitErr = firstPOI->getError(); 
    double POIFitErrLo = firstPOI->getErrorLo();
    double POIFitErrHi = firstPOI->getErrorHi();
    cout << str << ":: POIFitVal = " << POIFitVal << " POIFitErr = " << POIFitErr << " POIFitErrLo = " << POIFitErrLo << " POIFitErrHi = " << POIFitErrHi << endl;
    //
    if (POIFitVal<=0) {//probably there is a boundary at 0
      firstPOI->setMin(-1.*firstPOI->getMax());
      RooNLLVar* nll_unbounded = (RooNLLVar*) pdf->createNLL(*thisdata, Constrain(nuis), Extended(kTRUE), Verbose(kFALSE)); 
      //  nll_unbounded->applyWeightSquared(kTRUE);
      /*
	RooProfileLL* profile_unbounded = (RooProfileLL*) nll_unbounded->createProfile(*firstPOI);
	firstPOI->setVal(0);
	ZeroCorrection = profile_unbounded->getVal();
	delete profile_unbounded;
      */
      ProfileLikelihoodTestStat testStat_unbounded(*pdf);
      firstPOI->setVal(0);
      ZeroCorrection = testStat_unbounded.Evaluate(*thisdata,poi);
      //    delete nll_unbounded; [Note: if you do applyWeightSquared(kTRUE); then a dirty flag is turned on ; after that dont try to delete]
      firstPOI->setMin(0);
    }
    //  return -1; // quick test to guess nbin, xstep ...
  }
  //
  std::vector<double> vecpll;
  std::vector<double> vecpll_pos;
  std::vector<double> vecx;
  //
  system(Form("mkdir -vp root/%s",pname.Data()));
  TString nname=Form("root/%s/imass%3.3d_%s.root",pname.Data(),imass,str.Data());
  if (xmax>0) nname=Form("root/%s/imass%3.3d_%s_%05.02f_%05.02f.root",pname.Data(),imass,str.Data(),xmin,xmax);
  TFile* nfile=new TFile(nname, "recreate");
  TTree *newtree = new TTree(Form("newtree_%s",str.Data()), Form("newtree_%s",str.Data())); 
  double mu(0),pll(0); 
  newtree->Branch(Form("mu_%s",str.Data()),&mu);
  newtree->Branch(Form("pll_%s",str.Data()),&pll);
  //
  if (ngrid>0) nbin=ngrid;
  double pll_last=0;
  int v_filled=0;
  //
  int ibin0=0;
  for (int ibin=ibin0;ibin<nbin;++ibin) {
    if (ngrid>0) {
      mu=vecx_grid[ibin];
    } else {
      mu=xmin+ibin*xstep;	 
    }
    //    
    if (xmax>0 && mu>=xmax) break;
    firstPOI->setVal(mu);  
    if (imode>0) { // workspaces not produced by factory but using MakeAsimovData; they contain snapshots of conditional MLE's at mu=0,1 [and nominal one, of course!] 
      if (str.Contains("obs")) {
        w->loadSnapshot("nominalNuis");
      }
      if (str.Contains("exp")) {
        w->loadSnapshot("conditionalNuis_0"); // or conditionalNuis_1 for asimovData_1
      }
    }
    /*
      RooProfileLL* profile = (RooProfileLL*) nll->createProfile(*firstPOI);
      pll = profile->getVal(); 
      delete profile;
    */
    ProfileLikelihoodTestStat testStat(*pdf);
    testStat.SetOneSided(true);
    //    cout << "Print :: Start"  << endl;     pdf->getVariables()->Print("v");    cout << "Print :: End"  << endl; 
    pll = testStat.Evaluate(*thisdata,poi);
    //
    newtree->Fill();
    cout << Form("ibin = % 3d",ibin) << " " << Form("mu = % 8.04f",mu) << " " << Form("pll = % 8.04f",pll) << " " << Form("testStat = % 8.04f",2*pll) << endl;
    if (pll > pll_max && ibin>0 && pll > vecpll.back()) break;
    if (isnan(pll) && isnan(pll_last)) break;
    if (pll > 0.1 && pll < 3 && pll > pll_last ) {
      ++v_filled;
      vecx.push_back(mu);
      vecpll.push_back(pll);
    }
    pll_last = pll;
  }
  //
  //  delete nll; [Note: if you do applyWeightSquared(kTRUE); then a dirty flag is turned on ; after that dont try to delete]
  //
  cout << "Writing out " << newtree->GetName() << " to " << nname << endl;
  newtree->Write();
  nfile->Close();
  //
  if (v_filled==0) {
    cout << "v_filled is zero ... skipping ... " << endl;
    return -1;
  }
  //
  int nmin=std::min_element(vecpll.begin(), vecpll.end()) - vecpll.begin();
  int nmax = vecx.size();
  cout << "nmin = " << nmin << " nmax - nmin = " << nmax - nmin 
       << " vecx = " << vecx.size() << " : " << vecx.front() << " , " << vecx.back() 
       << " vecpll = " << vecpll.size() << " : " << vecpll.front() << " , " << vecpll.back() << endl;
  //  TSpline5 spline("TestSpline",&vecpll[nmin],&vecx[nmin],nmax-nmin);
  TGraph graph_transpose(nmax-nmin,&vecpll[nmin],&vecx[nmin]); 
  //
  double threshold = 0.5*pow(ROOT::Math::gaussian_quantile(confidenceLevel,1),2) - ZeroCorrection;
  double UL= graph_transpose.Eval(threshold,0,""); // spline.Eval(threshold);
  cout << firstPOI->GetName() 
       << " :: " << str.Data() 
       << " Threshold = " << threshold 
       << " ZeroCorrection = " << ZeroCorrection 
       << " UL = " << UL << " at CL = " << confidenceLevel*100 << " % " 
       << endl;
  //
  return 0; 
}
int GetCLs(const char *_output_){
  //
  int nmin=0;
  int nmax=0;
  double pll_last=0;
  double pll_obs_1=0;
  double pll_exp_1=0;
  //
  TString str="";
  TString rname="";
  TFile* rfile=0;
  TTree* rtree=0;
  double pll=0;
  TBranch *b_pll=0;
  double mu=0;
  TBranch *b_mu=0;
  //
  double threshold = 0.5*pow(ROOT::Math::gaussian_quantile(confidenceLevel,1),2);
  //
  system(Form("mkdir -vp out/%s",pname.Data()));
  ofstream fout(Form("out/%s/%s",pname.Data(),_output_),ios::out);
  str="obs";
  if (xmax>0) system(Form("./hadd -f root/%s/imass%3.3d_%s.root root/%s/imass%3.3d_%s_*.root",pname.Data(),imass,str.Data(),pname.Data(),imass,str.Data()));
  rname=Form("root/%s/imass%3.3d_%s.root",pname.Data(),imass,str.Data());
  cout << "Opening : " << rname << endl;
  rfile=TFile::Open(rname);
  rtree=(TTree*)rfile->Get(Form("newtree_%s",str.Data()));
  rtree->SetBranchAddress(Form("mu_%s", str.Data()), &mu,  &b_mu);	
  rtree->SetBranchAddress(Form("pll_%s",str.Data()), &pll, &b_pll);	
  std::vector<double> vec_mu_obs;
  std::vector<double> vec_mu_excl;
  std::vector<double> vecpll_obs;
  pll_last=0;
  bool keep = false;
  TString skeep = "false";
  cout << "GetEntries: " << rtree->GetEntries() << endl;
  for (int i=0; i < rtree->GetEntries(); ++i) {
    rtree->GetEntry(i);
    //
    if (mu==1.0) pll_obs_1 = pll;
    //
    if (mu>=0.1 && pll>5) break;
    //
    if (pll_last > 0  && pll > 0 && pll > (pll_last + 0.005) && ( vecpll_obs.size() ==0 || pll > vecpll_obs.back() + 0.005) && mu >= 0.1 ) {
      keep = true;
    } else {
      keep = false;
    }
    //
    if (keep) {
      vec_mu_obs.push_back(mu); 
      vecpll_obs.push_back(pll);
      skeep = "true";
    } else {
      vec_mu_excl.push_back(mu);
      skeep = "false";
    }
    //
    if (pll<20)
      cout << Form("i = % 3d",i) << " " 
           << Form("mu = % 8.04f",mu) << " " 
           << Form("testStat = % 8.04f",2*pll)  << " "
           << Form("pll = % 8.04f",pll) << " " 
           << Form("pll_last = % 8.04f",pll_last) << " " 
           << Form("skeep = %s",skeep.Data()) << " "
           << endl;
    //
    pll_last=pll;
  }
  rtree->ResetBranchAddress(b_mu);
  rtree->ResetBranchAddress(b_pll);
  rfile->Close();
  double UL_obs=0;
  if (vecpll_obs.size()==0) { 
    cout << "vecpll_obs.size() == 0 : exiting ... " << endl; return -1; 
  }else{
    cout << "vecpll_obs.size() == " << vecpll_obs.size() << " read from : " << rname << endl;
    nmin=std::min_element(vecpll_obs.begin(), vecpll_obs.end()) - vecpll_obs.begin();
    nmax = vec_mu_obs.size();
    TGraph gr_obs(nmax-nmin,&vecpll_obs[nmin],&vec_mu_obs[nmin]);
    UL_obs = gr_obs.Eval(threshold,0,"");
    cout << "nmin = " << nmin << " nmax - nmin = " << nmax - nmin
         << " vec_mu_obs = " << vec_mu_obs.size() << " : " << vec_mu_obs.front() << " , " << vec_mu_obs.back()
				 << " [nmin, nmax-nmin) = " << vec_mu_obs[nmin] << " , " << vec_mu_obs[nmax-nmin-1]
         << " vecpll_obs = " << vecpll_obs.size() << " : " << vecpll_obs.front() << " , " << vecpll_obs.back() 
				 << " [nmin, nmax-nmin) = " << vecpll_obs[nmin] << " , " << vecpll_obs[nmax-nmin-1]
         << " threshold = " << threshold << " UL_obs = " << UL_obs << " at CL = " << confidenceLevel*100 << " % " << endl;
    //
    double ymin(0), ymax(5);
    TCanvas* c1 = new TCanvas("c1","c1",400,400);
    TGraph graph(nmax-nmin,&vec_mu_obs[nmin],&vecpll_obs[nmin]); 
    graph.SetTitle(Form("ProfileLikelihood Scan vs Parameter of Interest [Observed at %3.3d GeV]",imass));
    graph.SetMinimum(ymin);
    graph.SetMaximum(ymax);
    graph.GetXaxis()->SetLimits( (gr_obs.Eval(ymin,0,"")) > 0 ? gr_obs.Eval(ymin,0,"") : 0, gr_obs.Eval(ymax,0,""));
    graph.SetMarkerStyle(20);
    graph.SetMarkerSize(0.5);
    graph.SetMarkerColor(2);
    graph.SetLineColor(4);
    graph.Draw("AL*");
    TLine xline(graph.GetXaxis()->GetXmin(),threshold,UL_obs,threshold); xline.SetLineColor(6); xline.Draw();
    TLine yline(UL_obs,0,UL_obs,threshold); yline.SetLineColor(6); yline.Draw();
    c1->Update();
    system(Form("mkdir -vp fig/%s",pname.Data()));
    PrintCanvas(c1,Form("fig/%s/imass%3.3d_pll_%s",pname.Data(),imass,str.Data()));
//     c1->SaveAs(Form("fig/%s/imass%3.3d_pll_%s.eps",pname.Data(),imass,str.Data()));
//     c1->SaveAs(Form("fig/%s/imass%3.3d_pll_%s.png",pname.Data(),imass,str.Data()));
  }
  //
  cout << "vec_mu_excl :: " << vec_mu_excl.size() << " :: ";
  if (vec_mu_excl.size() > 0 ){
    for (unsigned int itemp=0;itemp<vec_mu_excl.size(); ++itemp) {
      cout << vec_mu_excl[itemp] ;
      if (itemp!=vec_mu_excl.size()-1) cout << " , ";
    }
  }
  cout << endl;
  //
  str="exp";
  if (xmax>0) system(Form("./hadd -f root/%s/imass%3.3d_%s.root root/%s/imass%3.3d_%s_*.root",pname.Data(),imass,str.Data(),pname.Data(),imass,str.Data()));
  rname=Form("root/%s/imass%3.3d_%s.root",pname.Data(),imass,str.Data());
  rfile=TFile::Open(rname);
  rtree=(TTree*)rfile->Get(Form("newtree_%s",str.Data()));
  rtree->SetBranchAddress(Form("mu_%s", str.Data()), &mu,  &b_mu);	
  rtree->SetBranchAddress(Form("pll_%s",str.Data()), &pll, &b_pll);	
  std::vector<double> vec_mu_exp;
  std::vector<double> vecpll_exp;
  std::vector<double> vec_mu_exp_only;
  std::vector<double> vecpll_exp_only;
  pll_last=0;
  skeep = "false";
  for (int i=0; i < rtree->GetEntries(); ++i) {
    rtree->GetEntry(i); 
    //
    if (mu==1.0) pll_exp_1 = pll;
    //
    bool excl_match=false;
    int ifind=-1;
    if (vec_mu_excl.size()>0) {
      for (unsigned int idummy=0;idummy<vec_mu_excl.size();++idummy){
        if (fabs(mu-vec_mu_excl[idummy]) < 0.001) {
          ifind=idummy;
          break;
        }
      }
      if (ifind>-1 && ifind < (int)vec_mu_excl.size() ) excl_match = true;
    }
    cout << "vec_mu_excl.size() = " << vec_mu_excl.size() << " ifind = " << ifind << " excl_match = " << excl_match << " mu = " << mu << endl;
    if (excl_match) {
      cout << "skipping mu = " << mu << endl;
    }else{
      vec_mu_exp.push_back(mu); 
      if (!isnan(pll) && pll > 0) {
        vecpll_exp.push_back(pll);
      }else {
        vecpll_exp.push_back(999);
      }
    }
    //
    if (pll_last > 0  && pll > 0 && pll > (pll_last + 0.005) && ( vecpll_exp_only.size() ==0 || pll > vecpll_exp_only.back() + 0.005) && pll<5 ) {
      keep = true;
    } else {
      keep = false;
    }
    //
    if (keep) {
      vec_mu_exp_only.push_back(mu); 
      vecpll_exp_only.push_back(pll);
      skeep = "true";
    } else {
      skeep = "false";
    }
    //
    if (pll<20)
      cout << Form("i = % 3d",i) << " " 
           << Form("mu = % 8.04f",mu) << " " 
           << Form("testStat = % 8.04f",2*pll) << " "
           << Form("pll = % 8.04f",pll) << " " 
           << Form("pll_last = % 8.04f",pll_last) << " " 
           << Form("skeep = %s",skeep.Data()) << " "
           << endl;
    //
    pll_last=pll;
  }
  rtree->ResetBranchAddress(b_mu);
  rtree->ResetBranchAddress(b_pll);
  rfile->Close();
  double UL_exp=0;
  if (vecpll_exp_only.size()==0) { 
    cout << "vecpll_exp_only.size() == 0 : exiting ... " << endl; return -1; 
  }else{
    cout << "vecpll_exp_only.size() == " << vecpll_exp_only.size() << " read from : " << rname << endl;
    nmin=std::min_element(vecpll_exp_only.begin(), vecpll_exp_only.end()) - vecpll_exp_only.begin();
    nmax = vec_mu_exp_only.size();
    TGraph gr_exp(nmax-nmin,&vecpll_exp_only[nmin],&vec_mu_exp_only[nmin]);
    UL_exp = gr_exp.Eval(threshold,0,"");
    cout << "nmin = " << nmin << " nmax - nmin = " << nmax - nmin
         << " vec_mu_exp = " << vec_mu_exp_only.size() << " : " << vec_mu_exp_only.front() << " , " << vec_mu_exp_only.back()
				 << " [nmin, nmax-nmin) = " << vec_mu_exp_only[nmin] << " , " << vec_mu_exp_only[nmax-nmin-1]
         << " vecpll_exp = " << vecpll_exp_only.size() << " : " << vecpll_exp_only.front() << " , " << vecpll_exp_only.back() 
				 << " [nmin, nmax-nmin) = " << vecpll_exp_only[nmin] << " , " << vecpll_exp_only[nmax-nmin-1]
         << " threshold = " << threshold << " UL_exp = " << UL_exp << " at CL = " << confidenceLevel*100 << " % " << endl;
    //
    double ymin(0), ymax(5);
    TCanvas* c2 = new TCanvas("c2","c2",400,400);
    TGraph graph(nmax-nmin,&vec_mu_exp_only[nmin],&vecpll_exp_only[nmin]); 
    graph.SetTitle(Form("ProfileLikelihood Scan vs Parameter of Interest [Expected at %3.3d GeV]",imass));
    graph.SetMinimum(ymin);
    graph.SetMaximum(ymax);
    graph.GetXaxis()->SetLimits( (gr_exp.Eval(ymin,0,"")) > 0 ? gr_exp.Eval(ymin,0,"") : 0, gr_exp.Eval(ymax,0,""));
    graph.SetMarkerStyle(20);
    graph.SetMarkerSize(0.5);
    graph.SetMarkerColor(2);
    graph.SetLineColor(4);
    graph.Draw("AL*");
    TLine xline(graph.GetXaxis()->GetXmin(),threshold,UL_exp,threshold); xline.SetLineColor(6); xline.Draw();
    TLine yline(UL_exp,0,UL_exp,threshold); yline.SetLineColor(6); yline.Draw();
    c2->Update();
    system(Form("mkdir -vp fig/%s",pname.Data()));
    PrintCanvas(c2,Form("fig/%s/imass%3.3d_pll_%s",pname.Data(),imass,str.Data()));
//     c2->SaveAs(Form("fig/%s/imass%3.3d_pll_%s.eps",pname.Data(),imass,str.Data()));
//     c2->SaveAs(Form("fig/%s/imass%3.3d_pll_%s.png",pname.Data(),imass,str.Data()));
  }
  //
  double p_clsb = 1;
  double p_clb  = 0;
  double p_cls  = 1;
  calc_pvalue(2*pll_obs_1,2*pll_exp_1, p_clsb, p_clb, p_cls);
  double CLs_mu1 = p_cls;
  cout << "CLs_mu1 = " << CLs_mu1 << endl;
  //
  if (vecpll_exp.size()==0) { 
    cout << "vecpll_exp.size() == 0 : exiting ... " << endl; return -1; 
  }
  //
  double CLs_obs = calc_CLsLimit("obs",vecpll_obs,vecpll_exp,vec_mu_obs,vec_mu_exp);
  //
  double CLs_exp = calc_CLsLimit("exp",vecpll_exp_only,vecpll_exp_only,vec_mu_exp_only,vec_mu_exp_only);
  //
  nmin=std::min_element(vecpll_exp_only.begin(), vecpll_exp_only.end()) - vecpll_exp_only.begin();
  nmax = vec_mu_exp_only.size();
  TGraph gr_qmuAby2(nmax-nmin,&vec_mu_exp_only[nmin],&vecpll_exp_only[nmin]);
  double qmuA_UL    = 2*gr_qmuAby2.Eval(CLs_exp,0,"");  
  double sigma_s    = CLs_exp/sqrt(qmuA_UL);
  double CLs_plus1  = sigma_s *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel)*ROOT::Math::normal_cdf(+1,1),1)+1);
  double CLs_plus2  = sigma_s *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel)*ROOT::Math::normal_cdf(+2,1),1)+2);
  double CLs_minus1 = sigma_s *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel)*ROOT::Math::normal_cdf(-1,1),1)-1);
  double CLs_minus2 = sigma_s *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel)*ROOT::Math::normal_cdf(-2,1),1)-2);
  cout << Form("CLs  [ obs/exp/+2sigma/+1sigma/-1sigma/-2sigma UL at %2.0f % :: %10s at % 3.3d GeV ] = ",confidenceLevel*100, pname.Data(),imass) 
       << Form(" %5.02f", CLs_obs) 
       << Form(" %5.02f", CLs_exp)
       << Form(" %5.02f", CLs_plus2)
       << Form(" %5.02f", CLs_plus1)
       << Form(" %5.02f", CLs_minus1)
       << Form(" %5.02f", CLs_minus2)
       << Form(" %10.06f",CLs_mu1)
       << endl;
  fout << Form(" %3.3d", imass)
       << Form(" %5.02f", CLs_obs) 
       << Form(" %5.02f", CLs_exp)
       << Form(" %5.02f", CLs_plus2)
       << Form(" %5.02f", CLs_plus1)
       << Form(" %5.02f", CLs_minus1)
       << Form(" %5.02f", CLs_minus2)
       << endl;
  //
  double sigma       = UL_exp /ROOT::Math::gaussian_quantile(confidenceLevel,1);
  double CLsb_exp    = sigma *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel),1)+0); 
  double CLsb_plus1  = sigma *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel),1)+1); 
  double CLsb_plus2  = sigma *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel),1)+2);
  double CLsb_minus1 = sigma *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel),1)-1); 
  double CLsb_minus2 = sigma *(ROOT::Math::gaussian_quantile(1-(1-confidenceLevel),1)-2); 
  cout << Form("CLsb [ obs/exp/+2sigma/+1sigma/-1sigma/-2sigma UL at %2.0f % :: %10s at % 3.3d GeV ] = ",confidenceLevel*100, pname.Data(),imass) 
       << Form(" %5.02f", UL_obs)  
       << Form(" %5.02f", CLsb_exp) 
       << Form(" %5.02f", CLsb_plus2) 
       << Form(" %5.02f", CLsb_plus1) 
       << Form(" %5.02f", CLsb_minus1) 
       << Form(" %5.02f", CLsb_minus2 > 0 ? CLsb_minus2 : 0) 
       << endl; 
  //
  cout << Form("PCL  [ obs/exp/+2sigma/+1sigma/-1sigma/-2sigma UL at %2.0f % :: %10s at % 3.3d GeV ] = ",confidenceLevel*100, pname.Data(),imass) 
       << Form(" %5.02f", UL_obs>CLsb_exp ? UL_obs : CLsb_exp)  
       << Form(" %5.02f", CLsb_exp) 
       << Form(" %5.02f", CLsb_plus2) 
       << Form(" %5.02f", CLsb_plus1) 
       << Form(" %5.02f", CLsb_exp) 
       << Form(" %5.02f", CLsb_exp) 
       << endl; 
  //
  fout.close();
  return 0;
}
int main(int argc, char* argv[]){
  //
  //Argument variables
  //
  imass = (argc>1) ? atoi(argv[1]) : 500; // > 0 : use snapshot ; < 0 : no snapshot [Always try to include snapshot and use imode > 0]
  imode = (argc>2) ? atoi(argv[2]) :  1;
  iread = (argc>3) ? atoi(argv[3]) : -1; // 1: obs; 0: exp; -1: read
  iprofile = (argc>4) ? atoi(argv[4]) : 0; // 0: mu = 0; 1: mu = 1; 2: mu = nominal
  xmin  = (argc>5) ? atof(argv[5]) :  0;
  xmax  = (argc>6) ? atof(argv[6]) :  0;
  //
  // Particuliers  
  //
  if (imode==1) {
    fname =  Form("workspaces/llvv/%3.3d.root",imass);
    pname = "llvv";
    xstep = 0.05;
    nbin = 300;
  }else if (imode==2) {
    fname =  Form("workspaces/llqq/%3.3d.root",imass);
    pname = "llqq";
    xstep = 0.1;
    nbin = 400;
  }else if (imode==3) {
    fname =  Form("workspaces/4l/%3.3d.root",imass);
    pname = "4l";
    xstep = 0.05;
    nbin = 400;
  }else if (imode==4) {
    fname =  Form("workspaces/ZZ/%3.3d.root",imass);
    pname = "ZZ";
//     xstep = 0.2;
    xstep=0.05;
    nbin=100;
//     nbin = 200;
  }else if (imode==5) {
    fname =  Form("workspaces/monojet/%3.3d.root",imass);
    pname = "4l";
    xstep = 0.05;
    nbin = 400;
  }


  //
  TStopwatch timer;
  timer.Start();
// #ifdef USING_MINUIT2
//   // The change to the code makes the ProfileLikelihoodTestStat a bit more configurable. 
//   // by default it should still use the default Minuit and you won't see a speed gain. To see the speed gain, you need to add these lines
//   // before the main calls to the test statistic evaluation.
//   //
//   ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
//   ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
//   ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);  
// #endif
  //
  //  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL); // lowest message level
  //
  if (iread==1) {ReadInputs(); GetPLL("obs");}
  if (iread==0) {ReadInputs(); GetPLL("exp");}
  if (iread==-1) GetCLs(Form("%3.3d.txt",imass));
  //
  timer.Stop(); 
  timer.Print();
  //
  return 0;
  //
}
