#ifndef _CommonFunc_h_
#define _CommonFunc_h_
#include "CommonHead.h"

namespace CommonFunc{
  TH1F* CreateHist(TString title="hist",TString tag=0,TString axisx=0,TString axisy=0,int nbin=100,double xmin=0,double xmax=100);
  TCanvas* CreateCanvas(TString title="canvas",TString tag=0,double ww=800,double wh=600);
  TPad* CreatePad(TString title="pad",TString tag=0,double x1=0,double y1=0.,double x2=1,double y2=1);
  THStack* CreateStack(TString title="stack",TString tag=0,TString axisx=0,TString axisy=0);
  TFile* OpenFile(TString fileName="",TString option="read");
  void PrintCanvas(TCanvas* c, TString name);
  bool IsNaN(double var);
  void GetX1Y1X2Y2(TPad *c,double x[4]);
  void GetX1Y1X2Y2(TPad *c,double &x1,double &y1,double &x2,double &y2);
  TLegend* CreateLegend(double x_low, double y_low, double x_up, double y_up, TList* h_list, std::vector<TString> text, std::vector<TString> option,double textsize);
  void DrawConstantLine(TCanvas *c, double value,double *mass,int nmass);
  void DrawVerticalLine(TCanvas *c, double value,double ymin=0,double ymax=100);
  TPaveText* CreatePaveText(double x1,double y1,double x2,double y2,std::vector<TString> text,double textsize);
  TStyle* AtlasStyle();
  void SetAtlasStyle();
  void ScaleToOne(TH1*);
  void SetAllHistColor(TH1*, Color_t);
  std::vector<TString> SplitString(const TString& theOpt, const char separator );
  double Cone(double,double,double,double);
  double Cone(TLorentzVector, TLorentzVector);
  double DiffPhi(double,double);
  double DiffPhi(double);
};


#endif
