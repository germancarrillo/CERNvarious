#ifndef MakeAsimovData_H
#define MakeAsimovData_H

#include "CommonHead.h"
#include "RooStats/ModelConfig.h"

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

#include "RooWorkspace.h"
#include "RooStats/RooStatsUtils.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooMinimizer.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;

int makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData, double muval,double profileMu);
void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);
string createAsimovData(const char* infile="", const char* wsName="combined", const char* mcName ="ModelConfig", const char* dataName="obsData", int imass=125);
#endif
