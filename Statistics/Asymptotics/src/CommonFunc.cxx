#include "CommonFunc.h"

using namespace std;

bool CommonFunc::IsNaN(double var){volatile double d=var;return d!=d;}

TH1F* CommonFunc::CreateHist(TString title,TString tag,TString axisx,TString axisy,int nbin,double xmin,double xmax){
  TString histname=title;
  if(tag!="") histname.Append(Form("_%s",tag.Data()));

  TH1F *h=new TH1F(histname,histname,nbin,xmin,xmax);
  h->GetXaxis()->SetTitle(axisx);
  h->GetYaxis()->SetTitle(axisy);
  h->Sumw2();
  h->SetMinimum(0);
  return h;
}

THStack* CommonFunc::CreateStack(TString title,TString tag,TString axisx,TString axisy){
  TString stackname=title;
  if(tag!="") stackname.Append(Form("_%s",tag.Data()));
  THStack *hs=new THStack(stackname.Data(),stackname.Data());
  return hs;
}

TCanvas* CommonFunc::CreateCanvas(TString title,TString tag,double ww,double wh){
  TString canvasname=title;
  if(tag!="") canvasname.Append(Form("_%s",tag.Data()));
  TCanvas *c=new TCanvas(canvasname,canvasname,0,0,ww,wh);
  return c;
}

///Sakuya!!!!
TPad* CommonFunc::CreatePad(TString title,TString tag,double x1,double y1,double x2,double y2){
  TString padname=title;
  if(tag!="") padname.Append(Form("_%s",tag.Data()));
  TPad *p=new TPad(padname,padname,x1,y1,x2,y2);
  p->Draw();
  p->cd();
  p->Modified();
  p->SetFillStyle(0);

  return p;
}

TFile* CommonFunc::OpenFile(TString fileName,TString option){
  TFile* f=new TFile(fileName.Data(),option.Data());
  TDirectory *readdir = gDirectory;
  if (f && f->IsZombie()) SafeDelete(f);
  readdir->cd();
  if(!f){
    Error("OpenFile","Cannot open file %s",fileName.Data());
    return NULL;
  }
  return f;
}

void CommonFunc::PrintCanvas(TCanvas *c, TString name){
  c->Print(Form("%s.root",name.Data()));
  c->Print(Form("%s.eps",name.Data()));
  c->Print(Form("%s.png",name.Data()));
  c->Print(Form("%s.pdf",name.Data()));
}

void CommonFunc::GetX1Y1X2Y2(TPad *c,double x[4]){
  x[0]=c->GetFrame()->GetX1()+c->GetLeftMargin();
  x[1]=c->GetFrame()->GetY1()+c->GetBottomMargin();
  x[2]=c->GetFrame()->GetX2()-c->GetRightMargin();
  x[3]=c->GetFrame()->GetY2()-c->GetTopMargin();
}

void CommonFunc::GetX1Y1X2Y2(TPad *c,double &x1,double &y1,double &x2,double &y2){
  x1=c->GetFrame()->GetX1()+c->GetLeftMargin();
  y1=c->GetFrame()->GetY1()+c->GetBottomMargin();
  x2=c->GetFrame()->GetX2()-c->GetRightMargin();
  y2=c->GetFrame()->GetY2()-c->GetTopMargin();
}
TLegend* CommonFunc::CreateLegend(double x_low, double y_low, double x_up, double y_up, TList* h_list, vector<TString> text, vector<TString> option,double textsize)
{
    TLegend *legend = new TLegend(x_low,y_low,x_up,y_up);
    legend->SetBorderSize(0);
    legend->SetTextFont(42);
    legend->SetTextSize(textsize);
    legend->SetFillStyle(0);
    legend->SetFillColor(0);
    legend->SetLineColor(0);

    TIter next(h_list);
    TObject *obj = 0;
    int i=0;

    while((obj = next()))
      {
	legend->AddEntry(obj, text[i].Data(), option[i].Data());
	i++;
      }

    return legend;
}
void CommonFunc::DrawConstantLine(TCanvas *c, double value,double *mass,int nmass){
  Double_t oney[200]; for(Int_t e=0;e<nmass;e++) oney[e]=value;
  TGraph *one = new TGraph(nmass,mass,oney);
  one->SetLineColor(kRed);
  one->SetLineWidth(2);
  one->SetLineStyle(2);
  c->cd();
  one->Draw("same");
}

TPaveText* CommonFunc::CreatePaveText(double x1,double y1,double x2,double y2,vector<TString> text,double textsize){
  TPaveText *tex=new TPaveText();
  tex->SetFillColor(0);tex->SetTextSize(0.05);
  tex->SetFillStyle(0);tex->SetBorderSize(0);
  int n=text.size();
  for(int i=0;i<n;i++) tex->AddText(text[i].Data());
  tex->SetX1NDC(x1);
  tex->SetY1NDC(y1);
  tex->SetX2NDC(x2);
  tex->SetY2NDC(y2);
  tex->SetTextSize(textsize);
  return tex;
}

void CommonFunc::ScaleToOne(TH1 *h){
  double area=h->Integral();
  h->Scale(1/area);
}

TStyle* CommonFunc::AtlasStyle() 
{
  TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.05);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.16);
  atlasStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.1);
  atlasStyle->SetTitleYOffset(1.3);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05; // originally 0.05
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");
  
  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(1.2);
  atlasStyle->SetHistLineWidth((Width_t)3.0);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle(0);
  //atlasStyle->SetOptStat(1111);
  atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  return atlasStyle;

}

void CommonFunc::SetAtlasStyle()
{
  std::cout << "\nApplying ATLAS style settings...\n" << std::endl ;
  TStyle* atlasStyle = AtlasStyle();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
}

vector<TString> CommonFunc::SplitString(const TString& theOpt, const char separator )
{
   // splits the option string at 'separator' and fills the list
   // 'splitV' with the primitive strings
   vector<TString> splitV;
   TString splitOpt(theOpt);
   splitOpt.ReplaceAll("\n"," ");
   splitOpt = splitOpt.Strip(TString::kBoth,separator);
   while (splitOpt.Length()>0) {
      if ( !splitOpt.Contains(separator) ) {
         splitV.push_back(splitOpt);
         break;
      }
      else {
         TString toSave = splitOpt(0,splitOpt.First(separator));
         splitV.push_back(toSave);
         splitOpt = splitOpt(splitOpt.First(separator),splitOpt.Length());
      }
      splitOpt = splitOpt.Strip(TString::kLeading,separator);
   }
   return splitV;
}

void CommonFunc::DrawVerticalLine(TCanvas *c, double value,double ymin,double ymax){
  Double_t onex[100],oney[100]; 
  double step=(ymax-ymin)/100;
  for(Int_t e=0;e<100;e++) {onex[e]=value;oney[e]=ymin+e*step;}
  TGraph *one = new TGraph(100,onex,oney);
  one->SetLineColor(kRed);
  one->SetLineWidth(2);
  one->SetLineStyle(2);
  c->cd();
  one->Draw("same");
}

void CommonFunc::SetAllHistColor(TH1* h, Color_t c){
  h->SetLineColor(c);
  h->SetFillColor(c);
  h->SetMarkerColor(c);
}

double CommonFunc::Cone(double eta_t, double phi_t, double eta_l, double phi_l){
  Double_t DR = sqrt( ((eta_t - eta_l)*(eta_t - eta_l)) 
		      +(DiffPhi(phi_t - phi_l))*(DiffPhi(phi_t - phi_l)));
  return DR;
}

double CommonFunc::Cone(TLorentzVector p_t, TLorentzVector p_l){
  return Cone(p_t.Eta(),p_t.Phi(),p_l.Eta(),p_l.Phi());
}

double CommonFunc::DiffPhi(double dPhi) {
  if (fabs(dPhi) > M_PI) return fabs(2.*M_PI-fabs(dPhi));
  return fabs(dPhi);
}

double CommonFunc::DiffPhi(double phi1,double phi2){
  return DiffPhi(phi1-phi2);
}
