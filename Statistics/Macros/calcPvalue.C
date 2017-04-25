using namespace std;
using namespace RooFit;
using namespace RooStats;

double minimize(ModelConfig *mc, RooAbsData *data, int strat){
  RooAbsReal* nll=mc->GetPdf()->createNLL(*data, Constrain(*mc->GetNuisanceParameters()), GlobalObservables(*mc->GetGlobalObservables()));
  nll->enableOffsetting(true);
  RooMinimizer minim(*nll);
  minim.setStrategy(strat);
  minim.setPrintLevel(0);
  minim.setEps(1);
  int status=minim.minimize("Minuit2");
  return nll->getVal();
}

void constSet(RooArgSet* set, bool flag, RooArgSet* snapshot=NULL){
  TIterator *iter = set -> createIterator();
  RooRealVar* parg = NULL;
//   if(snapshot!=NULL) recoverSet(set,snapshot);
  if(snapshot!=NULL) *set=*snapshot;
  while((parg=(RooRealVar*)iter->Next()) ){
    parg->setConstant(flag); 
  }
  // SafeDelete(iter);
}

void calcPvalue(TString inputWSFileName="Comb_v00.root", Double_t mX=400, Double_t ma=0.7){
  TFile *f=TFile::Open(inputWSFileName);

  // ------------- Change workspace, ModelConfig, and dataset names to the one you use -----------------
  RooWorkspace *w=(RooWorkspace*)f->Get("combWS");
  ModelConfig *mc=(ModelConfig*)w->obj("ModelConfig");
  RooAbsData *data=w->data("combData");
  int strat=-1;
  
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
  w->loadSnapshot("ucmles");
  // constSet((RooArgSet*)w->allVars().selectByName("gamma_*"), true);
  // constSet((RooArgSet*)w->allVars().selectByName("alpha_*"), true);
  // constSet((RooArgSet*)w->allVars().selectByName("ATLAS_*"), true);
 
  w->var("mX")->setVal(mX);
  w->var("mX")->setConstant(true);
  w->var("ma")->setVal(ma);
  w->var("ma")->setConstant(true);
  strat=1;
  
  w->var("xs")->setConstant(false);

  double nllMuHat=minimize(mc, data, strat);

  w->var("xs")->setVal(0);
  w->var("xs")->setConstant(true);

  double nllMuFix=minimize(mc, data, strat);

  double dnll=2*(nllMuFix-nllMuHat);
  double pvalue =  1-ROOT::Math::normal_cdf(sqrt(dnll),1,0);

  cout<<"local-p-value: "<<pvalue<<endl;
  cout<<"significance:  "<<RooStats::PValueToSignificance(pvalue)<<endl;
}

