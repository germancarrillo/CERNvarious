// Adapted from code originally written by
// Author: Aaron Armbruster
// Date:   2011-07-27 (Updated 2011-09-01)
// Email:  armbrusa@umich.edu
//
// using stringstream constructors.
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "TStyle.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TSystem.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TLine.h"
#include "TSpline.h"
#include "RooAbsData.h"
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
//#include "inc/ModelBase.hpp"
#include "TStopwatch.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooMinimizer.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;

int makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData, bool b_only);
void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);

int main(int argc, char* argv[]){
  
  TString fname,wname,mname, dname,pname;
  //
  //Argument variables
  //
  int imass = (argc>1) ? atoi(argv[1]) : 110;
  int imode = (argc>2) ? atoi(argv[2]) : 1;
  if(imode==1){
    pname="llvv";
    wname = "combined";
    mname="ModelConfig" ;
    dname="obsData";
  }
  else if(imode==2){
    pname="llqq";
    wname = "combined";
    mname="ModelConfig" ;
    dname="obsData";
  }
  else if(imode==3){
    pname="4l";
    wname = "combWS";
    mname="ModelConfig" ;
    dname="obsData";
  }
  else if(imode==4){
    pname="ZZ";
    wname = "combWS";
    mname="ModelConfig" ;
    dname="combData";
  }
  else if(imode==5){
    pname ="monojet";
    wname ="combined";
    mname ="ModelConfig";
    dname ="obsData";
  }
//   cout<<pname.Data()<<endl; getchar();
  fname = (argc>3) ? argv[3] : Form("/data01/montoya/workspace/%s/%s_%d_combined_allsys_model.root",pname.Data(),pname.Data(),imass);


  TFile* file=TFile::Open(fname);//,"update");
  
  RooWorkspace* ws = (RooWorkspace*)file->Get(wname);
  if (!ws){
    cout << "ERROR::Workspace: " << wname << " doesn't exist!" << endl;
    return 0;
  }
  
  ModelConfig* mc = (ModelConfig*)ws->obj(mname);
  if (!mc)  {
    cout << "ERROR::ModelConfig: " << mname << " doesn't exist!" << endl;
    return 0;
  }
  
  RooDataSet* data = (RooDataSet*)ws->data(dname);
  if (!data) {
    cout << "ERROR::Dataset: " << dname << " doesn't exist!" << endl;
    return 0;
  }
  
  bool doConditional=1;
  RooAbsPdf* pdf = mc->GetPdf();
  makeAsimovData(mc, doConditional, ws, pdf, data, 1);
  makeAsimovData(mc, doConditional, ws, pdf, data, 0);
  system(Form("mkdir -vp workspaces/%s",pname.Data()));
  ws->writeToFile(Form("workspaces/%s/%3.3d.root",pname.Data(),imass));
  file->Close();
  return 0;
}

void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter)
{
  if (counter > 50)
  {
    cout << "ERROR::Couldn't unfold constraints!" << endl;
    cout << "Initial: " << endl;
    initial.Print("v");
    cout << endl;
    cout << "Final: " << endl;
    final.Print("v");
    exit(1);
  }
  TIterator* itr = initial.createIterator();
  RooAbsPdf* pdf;
  while ((pdf = (RooAbsPdf*)itr->Next()))
  {
    RooArgSet nuis_tmp = nuis;
    RooArgSet constraint_set(*pdf->getAllConstraints(obs, nuis_tmp, false));
    //if (constraint_set.getSize() > 1)
    //{
    string className(pdf->ClassName());
    if (className != "RooGaussian" && className != "RooLognormal" && className != "RooGamma" && className != "RooPoisson" && className != "RooBifurGauss")
    {
      counter++;
      unfoldConstraints(constraint_set, final, obs, nuis, counter);
    }
    else
    {
      final.add(*pdf);
    }
  }
  delete itr;
}


int makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData, bool b_only)
{
////////////////////
//make asimov data//
////////////////////

  int _printLevel = 0;
  stringstream muStr;
  muStr << "_" << !b_only;

  RooRealVar* mu = (RooRealVar*)mcInWs->GetParametersOfInterest()->first();//combWS->var("mu");
  mu->setVal(!b_only);

  RooArgSet mc_obs = *mcInWs->GetObservables();
  RooArgSet mc_globs = *mcInWs->GetGlobalObservables();
  RooArgSet mc_nuis = *mcInWs->GetNuisanceParameters();

//pair the nuisance parameter to the global observable
  RooArgSet mc_nuis_tmp = mc_nuis;
  RooArgList nui_list("ordered_nuis");
  RooArgList glob_list("ordered_globs");
  RooArgSet constraint_set_tmp(*combPdf->getAllConstraints(mc_obs, mc_nuis_tmp, false));
  RooArgSet constraint_set;
  int counter_tmp = 0;
  unfoldConstraints(constraint_set_tmp, constraint_set, mc_obs, mc_nuis_tmp, counter_tmp);

  TIterator* cIter = constraint_set.createIterator();
  RooAbsArg* arg;
  while ((arg = (RooAbsArg*)cIter->Next()))
  {
    RooAbsPdf* pdf = (RooAbsPdf*)arg;
    if (!pdf) continue;

    //pdf->Print();

    TIterator* nIter = mc_nuis.createIterator();
    RooRealVar* thisNui = NULL;
    RooAbsArg* nui_arg;
    while ((nui_arg = (RooAbsArg*)nIter->Next()))
    {
      if (pdf->dependsOn(*nui_arg))
      {
	thisNui = (RooRealVar*)nui_arg;
	break;
      }
    }
    delete nIter;


//need this incase the observable isn't fundamental. 
//in this case, see which variable is dependent on the nuisance parameter and use that.
    RooArgSet* components = pdf->getComponents();
    //components->Print();
    components->remove(*pdf);
    if (components->getSize())
    {
      TIterator* itr1 = components->createIterator();
      RooAbsArg* arg1;
      while ((arg1 = (RooAbsArg*)itr1->Next()))
      {
	TIterator* itr2 = components->createIterator();
	RooAbsArg* arg2;
	while ((arg2 = (RooAbsArg*)itr2->Next()))
	{
	  if (arg1 == arg2) continue;
	  if (arg2->dependsOn(*arg1))
	  {
	    components->remove(*arg1);
	  }
	}
	delete itr2;
      }
      delete itr1;
    }
    if (components->getSize() > 1)
    {
      cout << "ERROR::Couldn't isolate proper nuisance parameter" << endl;
      return -1;
    }
    else if (components->getSize() == 1)
    {
      thisNui = (RooRealVar*)components->first();
    }



    TIterator* gIter = mc_globs.createIterator();
    RooRealVar* thisGlob = NULL;
    RooAbsArg* glob_arg;
    while ((glob_arg = (RooAbsArg*)gIter->Next()))
    {
      if (pdf->dependsOn(*glob_arg))
      {
	thisGlob = (RooRealVar*)glob_arg;
	break;
      }
    }
    delete gIter;

    if (!thisNui || !thisGlob)
    {
      cout << "WARNING::Couldn't find nui or glob for constraint: " << pdf->GetName() << endl;
      //return;
      continue;
    }

    if (_printLevel >= 1) cout << "Pairing nui: " << thisNui->GetName() << ", with glob: " << thisGlob->GetName() << ", from constraint: " << pdf->GetName() << endl;

    nui_list.add(*thisNui);
    glob_list.add(*thisGlob);
    //thisNui->Print();
    //thisGlob->Print();
  }
  delete cIter;




//save the snapshots of nominal parameters
  combWS->saveSnapshot("nominalGlobs",glob_list);
  combWS->saveSnapshot("nominalNuis", nui_list);

  RooArgSet nuiSet_tmp(nui_list);

  mu->setVal(!b_only);
  mu->setConstant(1);

  int status = 0;
  if (doConditional)
  {
    cout << "Starting minimization.." << endl;
    RooAbsReal* nll = combPdf->createNLL(*combData, RooFit::Constrain(nuiSet_tmp));
    RooMinimizer minim(*nll);
    minim.setStrategy(2);
    minim.setPrintLevel(1);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    if (status != 0)
    {
      cout << "Fit failed for mu = " << mu->getVal() << " with status " << status << endl;
    }
    cout << "Done" << endl;
    //combPdf->fitTo(*combData,Hesse(false),Minos(false),PrintLevel(0),Extended(), Constrain(nuiSet_tmp));
  }
  mu->setConstant(0);



//loop over the nui/glob list, grab the corresponding variable from the tmp ws, and set the glob to the value of the nui
  int nrNuis = nui_list.getSize();
  if (nrNuis != glob_list.getSize())
  {
    cout << "ERROR::nui_list.getSize() != glob_list.getSize()!" << endl;
    return -1;
  }

  for (int i=0;i<nrNuis;i++)
  {
    RooRealVar* nui = (RooRealVar*)nui_list.at(i);
    RooRealVar* glob = (RooRealVar*)glob_list.at(i);

    if (_printLevel >= 1) cout << "nui: " << nui << ", glob: " << glob << endl;
    if (_printLevel >= 1) cout << "Setting glob: " << glob->GetName() << ", which had previous val: " << glob->getVal() << ", to conditional val: " << nui->getVal() << endl;

    glob->setVal(nui->getVal());
  }

//save the snapshots of conditional parameters
  //cout << "Saving conditional snapshots" << endl;
  combWS->saveSnapshot(("conditionalGlobs"+muStr.str()).c_str(),glob_list);
  combWS->saveSnapshot(("conditionalNuis" +muStr.str()).c_str(), nui_list);

  if (!doConditional)
  {
    combWS->loadSnapshot("nominalGlobs");
    combWS->loadSnapshot("nominalNuis");
  }

  //cout << "Making asimov" << endl;
//make the asimov data (snipped from Kyle)
  mu->setVal(!b_only);
  ModelConfig* mc = mcInWs;

  int iFrame=0;

  const char* weightName="weightVar";
  RooArgSet obsAndWeight;
  //cout << "adding obs" << endl;
  obsAndWeight.add(*mc->GetObservables());
  //cout << "adding weight" << endl;

  RooRealVar* weightVar = NULL;
  if (!(weightVar = combWS->var(weightName)))
  {
    combWS->import(*(new RooRealVar(weightName, weightName, 1,0,1000)));
    weightVar = combWS->var(weightName);
  }
  if (_printLevel >= 1) cout << "weightVar: " << weightVar << endl;
  obsAndWeight.add(*combWS->var(weightName));



  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  // MAKE ASIMOV DATA FOR OBSERVABLES

  // dummy var can just have one bin since it's a dummy
  if(combWS->var("ATLAS_dummyX"))  combWS->var("ATLAS_dummyX")->setBins(1);

  if (_printLevel >= 1) cout <<" check expectedData by category"<<endl;
  //RooDataSet* simData=NULL;
  RooSimultaneous* simPdf = dynamic_cast<RooSimultaneous*>(mc->GetPdf());

  map<string, RooDataSet*> asimovDataMap;

    
  //try fix for sim pdf
  RooCategory* channelCat = (RooCategory*)&simPdf->indexCat();//(RooCategory*)combWS->cat("master_channel");//(RooCategory*) (&simPdf->indexCat());
  //    TIterator* iter = simPdf->indexCat().typeIterator() ;
  TIterator* iter = channelCat->typeIterator() ;
  RooCatType* tt = NULL;
  int nrIndices = 0;
  while((tt=(RooCatType*) iter->Next())) {
    nrIndices++;
  }
  for (int i=0;i<nrIndices;i++){
    channelCat->setIndex(i);
    iFrame++;
    // Get pdf associated with state from simpdf
    RooAbsPdf* pdftmp = simPdf->getPdf(channelCat->getLabel()) ;
	
    // Generate observables defined by the pdf associated with this state
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

    if (_printLevel >= 1)
    {
      obstmp->Print();
      cout << "on type " << channelCat->getLabel() << " " << iFrame << endl;
    }

    RooDataSet* obsDataUnbinned = new RooDataSet(Form("combAsimovData%d",iFrame),Form("combAsimovData%d",iFrame),RooArgSet(obsAndWeight,*channelCat),WeightVar(*weightVar));
    RooRealVar* thisObs = ((RooRealVar*)obstmp->first());
    double expectedEvents = pdftmp->expectedEvents(*obstmp);
    double thisNorm = 0;
    for(int jj=0; jj<thisObs->numBins(); ++jj){
      thisObs->setBin(jj);

      thisNorm=pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
      if (thisNorm*expectedEvents <= 0)
      {
	cout << "WARNING::Detected bin with zero expected events (" << thisNorm*expectedEvents << ") ! Please check your inputs. Obs = " << thisObs->GetName() << ", bin = " << jj << endl;
      }
      if (thisNorm*expectedEvents > pow(10.0, -2) && thisNorm*expectedEvents < pow(10.0, 9)) obsDataUnbinned->add(*mc->GetObservables(), thisNorm*expectedEvents);
    }
    
    if (_printLevel >= 1)
    {
      obsDataUnbinned->Print();
      cout <<"sum entries "<<obsDataUnbinned->sumEntries()<<endl;
    }
    if(obsDataUnbinned->sumEntries()!=obsDataUnbinned->sumEntries()){
      cout << "sum entries is nan"<<endl;
      exit(1);
    }

    //((RooRealVar*)obstmp->first())->Print();
    //cout << "expected events " << pdftmp->expectedEvents(*obstmp) << endl;
     

    asimovDataMap[string(channelCat->getLabel())] = obsDataUnbinned;//tempData;

    if (_printLevel >= 1)
    {
      cout << "channel: " << channelCat->getLabel() << ", data: ";
      obsDataUnbinned->Print();
      cout << endl;
    }
  }

  RooDataSet* asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight,*channelCat),Index(*channelCat),Import(asimovDataMap),WeightVar(*weightVar));
  combWS->import(*asimovData);


//bring us back to nominal for exporting
  combWS->loadSnapshot("nominalNuis");
  combWS->loadSnapshot("nominalGlobs");

  return status;
}
