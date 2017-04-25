#include "frame.h"

using namespace std;
using namespace CommonFunc;
using namespace RooStats;

int main(int argc, char **argv){
  //============================ Global booking and setting ==========================
  //   gErrorIgnoreLevel = kError+1;
  map<string,bool> Opt;
  Opt["TWOCURVE"]=false;
  Opt["BAND"]=false;
  Opt["INTERPOLATION"]=false;
  Opt["MUHAT"]=false;
  Opt["MULTICURVE"]=false;
  Opt["PVALUE"]=false;
  Opt["PROFILE"]=false;
  Opt["NEW"]=false;
  TString myOptList="";
  if(argc>1) myOptList=argv[1];
  myOptList.ToUpper();
  if (myOptList != "") {
    for (std::map<std::string,bool>::iterator it = Opt.begin(); it != Opt.end(); it++) it->second = false;

    std::vector<TString> olist = TMVA::gTools().SplitString( myOptList, ',' );
    for (UInt_t i=0; i<olist.size(); i++) {
      std::string Option(olist[i]);
      
      if (Opt.find(Option) == Opt.end()) {
	std::cout << "Option \"" << Option << "\" not known in DrawFrame under this name. Choose among the following:" << std::endl;
	for (std::map<std::string,bool>::iterator it = Opt.begin(); it != Opt.end(); it++) std::cout << it->first << " ";
	std::cout << std::endl;
	return 1;
      }
      Opt[Option] = true;
    }
  }
  SetAtlasStyle();
  //================================= Draw Limit Band ====================================
  if(Opt["BAND"]){
    TString jobname=argc>2 ? argv[2] : "nominal";
    int imode=argc>3 ? atoi(argv[3]) : 1;
    string fileName[5];
    int nsample=2;
//     fileName[0]=jobname.Data();
//     fileName[1]="4l";
//     fileName[2]="llvv";
//     fileName[3]="llqq";

    fileName[0]=(jobname+"_lowmass").Data();
    fileName[1]=(jobname+"_highmass").Data();
    fileName[2]=jobname.Data();
//     fileName[4]="llvv_council_lowmass_rebin6";
//     string massList="lists/masslist";
    TString type=(imode==1)?"llvv":"llqq";
    double mass[5][200];
    double N2S[5][200],N1S[5][200],P1S[5][200],P2S[5][200],Median[5][200],Zero[5][200],Observed[5][200];

    TCanvas *cv = CreateCanvas("canvas", "",1024,768);
    int nmass[5];
//     for(int i=0;i<=20;i++) mass[i]=200+20*i;
    vector<int> temp;
//     for(int Mass=200;Mass<290;Mass+=2) temp.push_back(Mass); 
//     for(int Mass=290;Mass<350;Mass+=5) temp.push_back(Mass);
//     for(int Mass=350;Mass<400;Mass+=10) temp.push_back(Mass);
//     for(int Mass=400;Mass<=600;Mass+=20) temp.push_back(Mass);

//     for(int Mass=200;Mass<=600;Mass+=20) temp.push_back(Mass);
    for(int Mass=200;Mass<=280;Mass+=2){temp.push_back(Mass);} 

//     temp.clear();
//     for(int i=1100;i<1200;i+=10) temp.push_back(i);
//     for(int i=1200;i<1300;i+=5) temp.push_back(i);
//     for(int i=1300;i<1500;i+=10) temp.push_back(i);
//     for(int i=1500;i<2900;i+=20) temp.push_back(i);
//     for(int i=2900;i<3500;i+=50) temp.push_back(i);
//     for(int i=3500;i<4000;i+=100) temp.push_back(i);
//     for(int i=4000;i<=6000;i+=200) temp.push_back(i);

    nmass[0]=temp.size();
    for(int i=0;i<nmass[0];i++) mass[0][i]=temp[i];

    temp.clear();
//     for(int i=1100;i<1200;i+=10) temp.push_back(i);
//     for(int i=1200;i<1300;i+=5) temp.push_back(i);
//     for(int i=1300;i<1500;i+=10) temp.push_back(i);
//     for(int i=1500;i<2900;i+=20) temp.push_back(i);
//     for(int i=2900;i<3500;i+=50) temp.push_back(i);
//     for(int i=3500;i<4000;i+=100) temp.push_back(i);
//     for(int i=4000;i<=6000;i+=200) temp.push_back(i);
    for(int Mass=280;Mass<290;Mass+=2) temp.push_back(Mass); 
    for(int Mass=290;Mass<350;Mass+=5) temp.push_back(Mass);
    for(int Mass=350;Mass<400;Mass+=10) temp.push_back(Mass);
    for(int Mass=400;Mass<=600;Mass+=20) temp.push_back(Mass);
    nmass[1]=temp.size();
    for(int i=0;i<nmass[1];i++) mass[1][i]=temp[i];

    temp.clear();
//     for(int Mass=2000;Mass<2900;Mass+=20) temp.push_back(Mass); 
//     for(int Mass=2900;Mass<3500;Mass+=50) temp.push_back(Mass);
//     for(int Mass=3500;Mass<4000;Mass+=100) temp.push_back(Mass);
//     for(int Mass=4000;Mass<=6000;Mass+=200) temp.push_back(Mass);
    for(int Mass=200;Mass<290;Mass+=2) temp.push_back(Mass); 
    for(int Mass=290;Mass<350;Mass+=5) temp.push_back(Mass);
    for(int Mass=350;Mass<400;Mass+=10) temp.push_back(Mass);
    for(int Mass=400;Mass<=600;Mass+=20) temp.push_back(Mass);
//     for(int Mass=200;Mass<290;Mass+=2){temp.push_back(Mass);} 
//     for(int Mass=290;Mass<300;Mass+=5) temp.push_back(Mass);
    nmass[2]=temp.size();
    for(int i=0;i<nmass[2];i++) mass[2][i]=temp[i];

    temp.clear();
//     for(int Mass=2000;Mass<2900;Mass+=20) temp.push_back(Mass); 
//     for(int Mass=2900;Mass<3500;Mass+=50) temp.push_back(Mass);
//     for(int Mass=3500;Mass<4000;Mass+=100) temp.push_back(Mass);
//     for(int Mass=4000;Mass<=6000;Mass+=200) temp.push_back(Mass);
    temp.push_back(295);
    temp.push_back(300);
    nmass[3]=temp.size();
    for(int i=0;i<nmass[2];i++) mass[3][i]=temp[i];


    system(Form("mkdir -vp fig/limit_plot/%s",jobname.Data()));
    ofstream fout(Form("fig/limit_plot/%s/info_%s.txt",jobname.Data(),type.Data()),ios::out);
    fout<<Form("Mass \t -2S \t -1S \t Med \t +1S \t +2S \t Obs")<<endl;
    double max=0;
    for(int jj=0;jj<nsample;jj++){
      for(int idx=0;idx<nmass[jj];idx++){
// 	Info("Frame: BAND","%d mass points will be processed.",nmass[jj]);
	string infile=Form("out/%s/%3.3d.txt", fileName[jj].c_str(),(int)mass[jj][idx]);
	ifstream fin(infile.c_str(),ios::in);
	if(!fin){
	  cerr<<"File "<<infile.c_str()<<" does not exist."<<endl;
	  continue;
	}
	double n2s,n1s,median,p1s,p2s,observed,dummy;
	fin>>dummy>>observed>>median>>p2s>>p1s>>n1s>>n2s;
	fin.close();
	if(imode==4) mass[jj][idx]/=10;
	cout << "Mass "<<mass[jj][idx]<<" GeV"<<endl;
	cout << "-2 sigma  band " << n2s << endl; N2S[jj][idx]=median-n2s;
	cout << "-1 sigma  band " << n1s << endl; N1S[jj][idx]=median-n1s;
	cout << "Median of band " << median << endl; Median[jj][idx]=median;
	cout << "+1 sigma  band " << p1s << endl; P1S[jj][idx]=p1s-median;
	cout << "+2 sigma  band " << p2s << endl; P2S[jj][idx]=p2s-median;
	cout << "Observed "<<observed<<endl;Observed[jj][idx]=observed;
	//       getchar();
	Zero[jj][idx]=0;
	if(p2s>max)max=p2s;
	if(observed>max)max=observed;
	
	//       fout<<mass[idx]<<" "<<n2s<<" "<<n1s<<" "<<median<<" "<<p1s<<" "<<p2s<<" "<<observed<<endl;
	fout<<Form("%d \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f",(int)mass[jj][idx],n2s,n1s,median,p1s,p2s,observed)<<endl;
      }
    }

    TGraph *likelihd_limit_d[5];
    TGraph *likelihd_limit_c[5];
    TGraphAsymmErrors *likelihd_limit_1sigma[5];
    TGraphAsymmErrors *likelihd_limit_2sigma[5];
    TMultiGraph *likelihd_limit = new TMultiGraph("exclusionlimit_p","CL_{S} Exclusion Limits ;m_{H} [GeV]; 95% CL limit on #sigma/#sigma_{SM}");
//    TMultiGraph *likelihd_limit = new TMultiGraph("exclusionlimit_p",";;");

    TList *leglist=new TList();
    vector<TString> legtext;
    vector<TString> legopt;
    for(int jj=0;jj<nsample;jj++){	// WSN...
//       if(jj==1) continue;
      likelihd_limit_d[jj] = new TGraph(nmass[jj],mass[jj],Observed[jj]);
      if(jj!=0){
// 	likelihd_limit_d[jj]->SetLineColor(ColorWheel(jj));
// 	likelihd_limit_d[jj]->SetMarkerColor(ColorWheel(jj));
      }
      likelihd_limit_d[jj]->SetLineWidth(2);
      likelihd_limit_d[jj]->SetLineStyle(1); 
      likelihd_limit_d[jj]->SetMarkerSize(0.9);
//       cout<<likelihd_limit_d[jj]->GetMarkerSize()<<endl;

      likelihd_limit_c[jj] = new TGraph(nmass[jj],mass[jj],Median[jj]);
//       if(jj!=0){
// 	likelihd_limit_c[jj]->SetLineColor(ColorWheel(jj));
	likelihd_limit_c[jj]->SetLineWidth(2);
//       }
//       else {
// 	likelihd_limit_c[jj]->SetLineWidth(4);
//       }
      likelihd_limit_c[jj]->SetLineStyle(2);
//       likelihd_limit_d[1] = new TGraph(nmass,mass,Observed[1]);
//       likelihd_limit_d[1]->SetLineColor(kBlue);
//       likelihd_limit_d[1]->SetMarkerColor(kBlue);
//       likelihd_limit_d[1]->SetLineWidth(2);
//       likelihd_limit_d[1]->SetLineStyle(2);

//       likelihd_limit_c[1] = new TGraph(nmass,mass,Median[1]);
//       likelihd_limit_c[1]->SetLineColor(kBlue);
//       likelihd_limit_c[1]->SetLineWidth(2);
//       likelihd_limit_c[1]->SetLineStyle(kDotted);

      likelihd_limit_1sigma[jj] = new TGraphAsymmErrors(nmass[jj],mass[jj],Median[jj],Zero[jj],Zero[jj],N1S[jj],P1S[jj]);
      likelihd_limit_1sigma[jj]->SetFillColor(kGreen);
//       likelihd_limit_1sigma[jj]->SetLineColor(jj+1);
//       likelihd_limit_1sigma[jj]->SetLineStyle(3);
      likelihd_limit_2sigma[jj] = new TGraphAsymmErrors(nmass[jj],mass[jj],Median[jj],Zero[jj],Zero[jj],N2S[jj],P2S[jj]);
//       likelihd_limit_2sigma[jj]->SetLineColor(jj+1);
//       likelihd_limit_2sigma[jj]->SetLineStyle(3);
      likelihd_limit_2sigma[jj]->SetFillColor(kYellow);

      likelihd_limit->Add(likelihd_limit_2sigma[jj],"E3");  
      likelihd_limit->Add(likelihd_limit_1sigma[jj],"E3");

      likelihd_limit->Add(likelihd_limit_d[jj], "LP");
      likelihd_limit->Add(likelihd_limit_c[jj], "L");
//       }
//       leglist->Add(likelihd_limit_d[jj]); legtext.push_back(Form("Rebin %d", jj+2)); legopt.push_back("L");
    }


    double x1,y1,x2,y2;
    GetX1Y1X2Y2(cv,x1,y1,x2,y2);



    leglist->Add(likelihd_limit_d[0]);legtext.push_back("Observed");legopt.push_back("LP");
    leglist->Add(likelihd_limit_c[0]);legtext.push_back("Expected");legopt.push_back("L");
    leglist->Add(likelihd_limit_1sigma[0]);legtext.push_back("#pm 1#sigma");legopt.push_back("F");
    leglist->Add(likelihd_limit_2sigma[0]);legtext.push_back("#pm 2#sigma");legopt.push_back("F");
    TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
//     TList *leglist2=new TList();
//     vector<TString> legtext2,legopt2;
//     leglist2->Add(likelihd_limit_d[1]);legtext2.push_back("Period B-K");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[2]);legtext2.push_back("Period L-M");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[1]);legtext2.push_back("H#rightarrowZZ#rightarrow4l");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[2]);legtext2.push_back("H#rightarrowZZ#rightarrowll#nu#nu");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[3]);legtext2.push_back("H#rightarrowZZ#rightarrowllqq");legopt2.push_back("L");

//     TLegend* legend2=CreateLegend(x2-0.4,y1,x2,y1+0.3,leglist2,legtext2,legopt2,0.05);

    vector<TString> pavetext;
    pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
    // It is ATLAS now. 2011-08-12 4:48 p.m.
//     pavetext.push_back("#bf{#it{ATLAS}}");
    if(imode==1) pavetext.push_back("H#rightarrowZZ#rightarrowll#nu#nu");
    else if(imode==2) pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
    else if(imode==4) pavetext.push_back("H#rightarrowZZ");
//     pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
    if(imode==1||imode==2) pavetext.push_back("#scale[0.6]{#int}Ldt=4.7fb^{-1}, #sqrt{s}=7TeV");
    else if(imode==4) pavetext.push_back("#intLdt=2.05-4.8fb^{-1}, #sqrt{s}=7TeV");

    TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);

    cv->cd();

    likelihd_limit->Draw("A");
    likelihd_limit->GetXaxis()->SetTitleSize(0.05);
    likelihd_limit->GetXaxis()->SetTitleOffset(1.3);
    likelihd_limit->GetYaxis()->SetLabelSize(0.045);
    likelihd_limit->GetYaxis()->SetTitleSize(0.05);
//     likelihd_limit->SetMaximum(25);
//     likelihd_limit->SetMaximum(1.2*max);


    //    likelihd_limit->SetMaximum(1e2);
    if(imode==4){likelihd_limit->SetMinimum(1e-1);cv->SetLogy();}
//     likelihd_limit->GetXaxis()->SetLabelSize(0.0);
//     likelihd_limit->GetYaxis()->SetLabelSize(0.0);
//     cv->SetLogy();
//     if(imode==1) likelihd_limit->GetXaxis()->SetLimits(200,360);
    legend->Draw();
//     legend2->Draw();
    text->Draw("same");
    DrawConstantLine(cv,1,mass[2],nmass[2]);
    TLine *line2 = new TLine(280,0.2,280,3.);
    line2->SetLineStyle(2);
    line2->SetLineColor(kBlue);
    line2->SetLineWidth(2);
    line2->Draw();
    PrintCanvas(cv,Form("fig/limit_plot/%s/limitplot",jobname.Data()));
    fout.close();
  }// 
  //======================================================================================
  //================================= Draw Limit Band ====================================
  if(Opt["MULTICURVE"]){
    TString jobname=argc>2 ? argv[2] : "nominal";
    int imode=argc>3 ? atoi(argv[3]) : 1;
    string fileName[5];
    int nsample=2;
    fileName[0]="llvv_combination_interpolation_v5";
    fileName[1]="llvv_combination_interpolation_v4";
//     fileName[0]="llvv_combination_interpolation_opt2";
//     fileName[1]="llvv_combination_interpolation_v4";
    // Coarse rate
//     fileName[0]="llqq_approval"; // 4
//     fileName[1]="llqq_approval_complete_truncated";

//     fileName[0]="llvv_circulation_combined_default"; // 4
//     fileName[1]="llvv_circulation_BtoK_default";
//     fileName[2]="llvv_circulation_LtoM_default";
//     fileName[2]="llvv_approval_combined_rebinopt2"; // 3
//     fileName[1]="llvv_approval_combined_rebinopt1"; // 2
//     fileName[3]="llvv_approval_combined_rebinopt3"; // 1

//     string massList="lists/masslist";
    TString type=(imode==1)?"llvv":"llqq";
    double mass[200];
    double N2S[5][200],N1S[5][200],P1S[5][200],P2S[5][200],Median[5][200],Zero[200],Observed[5][200];

    TCanvas *cv = CreateCanvas("canvas", "",1024,768);
//     int nmass=21;
//     for(int i=0;i<=20;i++) mass[i]=200+20*i;
    vector<int> temp;
//     for(int Mass=200;Mass<290;Mass+=2){ temp.push_back(Mass);} // temp.push_back(295);
//     for(int Mass=290;Mass<350;Mass+=5) temp.push_back(Mass);
//     for(int Mass=350;Mass<400;Mass+=10) temp.push_back(Mass);
//     for(int Mass=400;Mass<=600;Mass+=20) temp.push_back(Mass);
    for(int Mass=200;Mass<=600;Mass+=20) temp.push_back(Mass);
    int nmass=temp.size();
    for(int i=0;i<nmass;i++){
      mass[i]=temp[i];
//       N2S[i]=N1S[i]=P1S[i]=P2S[i]=Median[0][i]=Observed[0][i]=0;
    }
    Info("DrawFrame: BAND","%d mass points will be processed.",nmass);
    system(Form("mkdir fig/limit_plot/%s",jobname.Data()));
    ofstream fout(Form("fig/limit_plot/%s/info_%s.txt",jobname.Data(),type.Data()),ios::out);
    fout<<Form("Mass \t -2S \t -1S \t Med \t +1S \t +2S \t Obs")<<endl;
    double max=0;
    for(int jj=0;jj<nsample;jj++){
      for(int idx=0;idx<nmass;idx++){
	string infile=Form("out/%s/%3.3d.txt", fileName[jj].c_str()// jobname.Data()
			   ,(int)mass[idx]);
	ifstream fin(infile.c_str(),ios::in);
	if(!fin){
	  cerr<<"File "<<infile.c_str()<<" does not exist."<<endl;
	  continue;
	}
	double n2s,n1s,median,p1s,p2s,observed,dummy;
	fin>>dummy>>observed>>median>>p2s>>p1s>>n1s>>n2s;
	fin.close();
	cout << "Mass "<<mass[idx]<<" GeV"<<endl;
	cout << "-2 sigma  band " << n2s << endl; N2S[jj][idx]=median-n2s;
	cout << "-1 sigma  band " << n1s << endl; N1S[jj][idx]=median-n1s;
	cout << "Median of band " << median << endl; Median[jj][idx]=median;
	cout << "+1 sigma  band " << p1s << endl; P1S[jj][idx]=p1s-median;
	cout << "+2 sigma  band " << p2s << endl; P2S[jj][idx]=p2s-median;
	cout << " Observed "<<observed<<endl;Observed[jj][idx]=observed;
	Zero[idx]=0;
 	if(p2s>max)max=p2s;
	//       fout<<mass[idx]<<" "<<n2s<<" "<<n1s<<" "<<median<<" "<<p1s<<" "<<p2s<<" "<<observed<<endl;
	fout<<Form("%d \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f",(int)mass[idx],n2s,n1s,median,p1s,p2s,observed)<<endl;
      }
    }

    TGraph *likelihd_limit_d[5];
    TGraph *likelihd_limit_c[5];
    TGraphAsymmErrors *likelihd_limit_1sigma[5];
    TGraphAsymmErrors *likelihd_limit_2sigma[5];
    TMultiGraph *likelihd_limit = new TMultiGraph("exclusionlimit_p","CL_{S} Exclusion Limits ;m_{H} [GeV]; 95% CL limit on #sigma/#sigma_{SM}");

    TList *leglist=new TList();
    vector<TString> legtext;
    vector<TString> legopt;
    for(int jj=0;jj<nsample;jj++){	// WSN...
//       if(jj==1) continue;
      likelihd_limit_d[jj] = new TGraph(nmass,mass,Observed[jj]);
      likelihd_limit_d[jj]->SetLineColor(ColorWheel(jj));
      likelihd_limit_d[jj]->SetMarkerColor(ColorWheel(jj));
      likelihd_limit_d[jj]->SetMarkerSize(0.9);
      likelihd_limit_d[jj]->SetLineWidth(2);
      likelihd_limit_d[jj]->SetLineStyle(1);


      likelihd_limit_c[jj] = new TGraph(nmass,mass,Median[jj]);
      likelihd_limit_c[jj]->SetLineColor(ColorWheel(jj));
      likelihd_limit_c[jj]->SetLineWidth(2);
      likelihd_limit_c[jj]->SetLineStyle(jj+2);

//       likelihd_limit_d[1] = new TGraph(nmass,mass,Observed[1]);
//       likelihd_limit_d[1]->SetLineColor(kBlue);
//       likelihd_limit_d[1]->SetMarkerColor(kBlue);
//       likelihd_limit_d[1]->SetLineWidth(2);
//       likelihd_limit_d[1]->SetLineStyle(2);

//       likelihd_limit_c[1] = new TGraph(nmass,mass,Median[1]);
//       likelihd_limit_c[1]->SetLineColor(kBlue);
//       likelihd_limit_c[1]->SetLineWidth(2);
//       likelihd_limit_c[1]->SetLineStyle(kDotted);

      likelihd_limit_1sigma[jj] = new TGraphAsymmErrors(nmass,mass,Median[jj],Zero,Zero,N1S[jj],P1S[jj]);
//       likelihd_limit_1sigma[jj]->SetFillColor(kGreen);
      if(jj!=0)likelihd_limit_1sigma[jj]->SetLineColor(jj+1);
      likelihd_limit_1sigma[jj]->SetLineStyle(3);
      likelihd_limit_2sigma[jj] = new TGraphAsymmErrors(nmass,mass,Median[jj],Zero,Zero,N2S[jj],P2S[jj]);
      if(jj!=0)likelihd_limit_2sigma[jj]->SetLineColor(jj+1);
      likelihd_limit_2sigma[jj]->SetLineStyle(3);
//       likelihd_limit_2sigma[jj]->SetFillColor(kYellow);
//       likelihd_limit->Add(likelihd_limit_2sigma,"E3");  
//       likelihd_limit->Add(likelihd_limit_1sigma,"E3");  
//       likelihd_limit->Add(likelihd_limit_2sigma[jj],"L");  
//       likelihd_limit->Add(likelihd_limit_1sigma[jj],"L");
      
      //     likelihd_limit->Add(likelihd_limit_c[0], "L");
      likelihd_limit->Add(likelihd_limit_d[jj], "LP");
      //     likelihd_limit->Add(likelihd_limit_c[1], "L");
      likelihd_limit->Add(likelihd_limit_c[jj], "L");
      leglist->Add(likelihd_limit_d[jj]);  legopt.push_back("LP");// legtext.push_back(Form("Rebin %d", jj+2));
//       leglist->Add(likelihd_limit_c[jj]);  legopt.push_back("L");
    }
    legtext.push_back("New");
    legtext.push_back("Old");

//     legtext.push_back("Nominal (Observed)");
//     legtext.push_back("No theory (Observed)");
//     legtext.push_back("No detector (Observed)");
//     legtext.push_back("No sys. (Observed)");
//     legtext.push_back("Combination (Expected)");
//     legtext.push_back("Period B-K (Expected)");
//     legtext.push_back("Period L-M (Expected)");
//     legtext.push_back("Coarse (30-50-100)");
//     legtext.push_back("Medium (15-25-50)");
//     legtext.push_back("Nominal (Observed)");
//     legtext.push_back("Nominal (Expected)");
//     legtext.push_back("All Sys. (Observed)");
//     legtext.push_back("All Sys. (Expected)");


//     legtext.push_back("Combination (Observed)");
//     legtext.push_back("Combination (Expected)");
//     legtext.push_back("Period B-K (Observed)");
//     legtext.push_back("Period B-K (Expected)");
//     legtext.push_back("Period L-M (Observed)");
//     legtext.push_back("Period L-M (Expected)");
//     legtext.push_back("Coarse (30-50-100)");
//     legtext.push_back("Medium (15-25-50)");
//     legtext.push_back("Fine (10-20-30)");
//     legtext.push_back("Finer (5-10-15)");
    double x1,y1,x2,y2;
    GetX1Y1X2Y2(cv,x1,y1,x2,y2);



//     leglist->Add(likelihd_limit_d[0]);legtext.push_back("Observed");legopt.push_back("LP");
//     leglist->Add(likelihd_limit_c[0]);legtext.push_back("Observed (New)");legopt.push_back("LP");
//     leglist->Add(likelihd_limit_c[1]);legtext.push_back("Observed (Old)");legopt.push_back("LP");
//     leglist->Add(likelihd_limit_1sigma);legtext.push_back("#pm 1#sigma");legopt.push_back("F");
//     leglist->Add(likelihd_limit_2sigma);legtext.push_back("#pm 2#sigma");legopt.push_back("F");
    TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
    vector<TString> pavetext;
    pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
    // It is ATLAS now. 2011-08-12 4:48 p.m.
//     pavetext.push_back("#bf{#it{ATLAS}}");
    if(imode==1) pavetext.push_back("H#rightarrowZZ#rightarrowll#nu#nu");
    else pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
//     pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
    pavetext.push_back("#intLdt=4.7fb^{-1}, #sqrt{s}=7TeV");
//     pavetext.push_back("Scaled from 2.05fb^{-1}");
    TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);

    cv->cd();

//     likelihd_limit->GetXaxis()->SetLabelSize(0.05);
//     likelihd_limit->GetXaxis()->SetTitleSize(0.05);
//     likelihd_limit->GetXaxis()->SetTitleOffset(1.3);
//     likelihd_limit->GetYaxis()->SetLabelSize(0.045);
//     likelihd_limit->GetYaxis()->SetTitleSize(0.05);
//     likelihd_limit->SetMaximum(25);
    likelihd_limit->SetMaximum(1.1*max);

    likelihd_limit->Draw("A");
//     if(imode==1) likelihd_limit->GetXaxis()->SetLimits(200,360);
    legend->Draw();
    text->Draw("same");
    DrawConstantLine(cv,1,mass,nmass);

    PrintCanvas(cv,Form("fig/limit_plot/%s/limitcomp",jobname.Data()));
    fout.close();
  }// 
  //======================================================================================
  //========================= Draw Two Limit in One Plot =================================
  if(Opt["TWOCURVE"]){
    ifstream fin1("fig/limit_plot/llvv_old/info_llvv.txt",ios::in);
    ifstream fin2("fig/limit_plot/llvv_nobtag/info_llvv.txt",ios::in);
    ifstream fin3("fig/limit_plot/llvv_new/info_llvv.txt",ios::in);
    assert(fin1);
    assert(fin2);
    assert(fin3);
    double x[200];
    double obs1[200],obs2[200],obs3[200],med1[200],med2[200],med3[200];
    int nmass=21;
    for(int i=0;i<nmass;i++){
      double mass,n2s,n1s,median,p1s,p2s,observed;
      string dummyline;
      getline(fin1,dummyline);
      fin1>>mass>>n2s>>n1s>>median>>p1s>>p2s>>observed;
      x[i]=mass;obs1[i]=observed;med1[i]=median;
      getline(fin2,dummyline);
      fin2>>mass>>n2s>>n1s>>median>>p1s>>p2s>>observed;
      x[i]=mass;obs2[i]=observed;med2[i]=median;
      getline(fin3,dummyline);
      fin3>>mass>>n2s>>n1s>>median>>p1s>>p2s>>observed;
      x[i]=mass;obs3[i]=observed;med3[i]=median;
    }
    for(int i=0;i<nmass;i++){
      cout<<x[i]<<" "<<obs1[i]<<" "<<med1[i]<<endl;
      cout<<x[i]<<" "<<obs2[i]<<" "<<med2[i]<<endl;
      cout<<x[i]<<" "<<obs3[i]<<" "<<med3[i]<<endl;
    }
    TGraph *limit_d_1=new TGraph(nmass,x,obs1);
    limit_d_1->SetLineColor(kBlue);
    limit_d_1->SetMarkerColor(kBlue);
    limit_d_1->SetLineWidth(2);
    limit_d_1->SetLineStyle(1);
    TGraph *limit_c_1=new TGraph(nmass,x,med1);
    limit_c_1->SetLineColor(kBlue);
    limit_c_1->SetMarkerColor(kBlue);
    limit_c_1->SetLineWidth(2);
    limit_c_1->SetLineStyle(2);
    TGraph *limit_d_2=new TGraph(nmass,x,obs2);
    limit_d_2->SetLineColor(kGreen);
    limit_d_2->SetMarkerColor(kGreen);
    limit_d_2->SetLineWidth(2);
    limit_d_2->SetLineStyle(1);
    TGraph *limit_c_2=new TGraph(nmass,x,med2);
    limit_c_2->SetLineColor(kGreen);
    limit_c_2->SetMarkerColor(kGreen);
    limit_c_2->SetLineWidth(2);
    limit_c_2->SetLineStyle(2);
    TGraph *limit_d_3=new TGraph(nmass,x,obs3);
    limit_d_3->SetLineColor(kRed);
    limit_d_3->SetMarkerColor(kRed);
    limit_d_3->SetLineWidth(2);
    limit_d_3->SetLineStyle(1);
    TGraph *limit_c_3=new TGraph(nmass,x,med3);
    limit_c_3->SetLineColor(kRed);
    limit_c_3->SetMarkerColor(kRed);
    limit_c_3->SetLineWidth(2);
    limit_c_3->SetLineStyle(2);
    TMultiGraph *likelihd_limit = new TMultiGraph("limit comparison","CL_{S} Exclusion Limits ;m_{H} [GeV]; 95% CL limit on #sigma/#sigma_{SM}");
    likelihd_limit->Add(limit_d_1,"LP");
    likelihd_limit->Add(limit_c_1,"L");
    likelihd_limit->Add(limit_d_2,"LP");
    likelihd_limit->Add(limit_c_2,"L");
    likelihd_limit->Add(limit_d_3,"LP");
    likelihd_limit->Add(limit_c_3,"L");

    TList *leglist=new TList();
    vector<TString> legtext;
    vector<TString> legopt;
    leglist->Add(limit_d_1);legtext.push_back("Observed (G)");legopt.push_back("LP");
    leglist->Add(limit_c_1);legtext.push_back("Expected (G)");legopt.push_back("L");
    leglist->Add(limit_d_2);legtext.push_back("Observed (L)");legopt.push_back("LP");
    leglist->Add(limit_c_2);legtext.push_back("Expected (L)");legopt.push_back("L");
    leglist->Add(limit_d_3);legtext.push_back("Observed (L+B)");legopt.push_back("LP");
    leglist->Add(limit_c_3);legtext.push_back("Expected (L+B)");legopt.push_back("L");
    TCanvas *cv = CreateCanvas("canvas", "",1024,768);
    double xy[4];
    GetX1Y1X2Y2(cv,xy);
    double x1=xy[0]; double y1=xy[1]; double x2=xy[2]; double y2=xy[3];
    TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
    vector<TString> pavetext;
    pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
    // It is ATLAS now. 2011-08-12 4:48 p.m.
//     pavetext.push_back("#bf{#it{ATLAS}}");
    pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
    pavetext.push_back("H#rightarrowZZ#rightarrowll#nu#nu");
    TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);
    cv->cd();
    likelihd_limit->Draw("A");
    legend->Draw();
    text->Draw("same");
    DrawConstantLine(cv,1,x,nmass);
    gROOT->ProcessLine(".!mkdir -vp fig/llvv");
    PrintCanvas(cv,"fig/llvv/limitcomp");
  }
  //======================================================================================
  //========================= Draw Two Limit in One Plot =================================
  if(Opt["INTERPOLATION"]){

    int mint=atoi(argv[2]);

    TFile *f[4];
    f[0]=new TFile("interpolate/hist/LimitHistograms.DPhiPtJ.NoWC.Rebin10.340.2052.7TeV.root","read");
    f[1]=new TFile(Form("interpolate/hist/Interpol/LimitHistograms.DPhiPtJ.NoWC.Rebin10.%d.2052.7TeV.root",mint),"read");
    f[2]=new TFile("interpolate/hist/LimitHistograms.DPhiPtJ.NoWC.Rebin10.300.2052.7TeV.root","read");
    f[3]=new TFile("interpolate/hist/LimitHistograms.DPhiPtJ.NoWC.Rebin10.320.2052.7TeV.root","read");
    TH1D *h[4][2];
//     for(int i=0;i<4;i++){
//       h[i][0]=(TH1D*) f[i]->Get("signal");
// //       h[i][0]->Sumw2();
//       h[i][1]=(TH1D*) f[i]->Get("signalOrig");
// //       h[i][1]->Sumw2();
//     }
    h[0][0]=(TH1D*)f[1]->Get("signal");
    h[1][0]=(TH1D*)f[1]->Get("signalOrig");
    h[1][0]->Rebin(10);
    h[2][0]=(TH1D*)f[2]->Get("signal");
    h[3][0]=(TH1D*)f[3]->Get("signal");
    TCanvas *c[3];
    h[0][0]->SetLineColor(kBlack);
    h[1][0]->SetLineStyle(kDotted);
    h[2][0]->SetLineColor(kRed);
    h[3][0]->SetLineColor(kGreen);
//     h[0][1]->SetLineColor(kBlack);
//     h[1][1]->SetLineStyle(kDotted);
//     h[2][1]->SetLineColor(kRed);
//     h[3][1]->SetLineColor(kGreen);
    ofstream fout("statistics.txt",ios::out);
    vector<TString> legtext;
    legtext.push_back(Form("%d (Coarse)",mint));
    legtext.push_back(Form("%d (Fine)",mint));
//     legtext.push_back("305");
    legtext.push_back("300");
    legtext.push_back("320");

    for(int j=0;j<1;j++){
      c[j]=CreateCanvas(h[0][j]->GetName(),h[0][0]->GetName(),1024,768);
      double x1,y1,x2,y2;
      GetX1Y1X2Y2(c[j],x1,y1,x2,y2);
      c[j]->cd();
      TList *leglist=new TList();

      vector<TString> legopt;
      for(int i=0;i<4;i++){

// 	if(j==1){
// 	  h[i][j]->Rebin(10);
// 	  h[i][j]->SetAxisRange(150,900);
// 	}
// 	cout<<"okay here"<<endl;
	fout<<Form("%s: %s Area: %f RMS: %f",legtext[i].Data(),h[i][j]->GetName(),h[i][j]->GetSumOfWeights(),h[i][j]->GetRMS())<<endl;
	if(i==0){
	  h[i][j]->SetMaximum(12);
	  h[i][j]->Draw("hist");
	}
	else{
	  h[i][j]->Draw("hist same");
// 	  if(i==1&&j==0) {
// 	    h[i][1]->SetLineColor(kBlue);
// 	    h[i][1]->Draw("hist same");
// 	  }
	}
	leglist->Add(h[i][j]);
	legopt.push_back("LF");
      }
      
      TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
      legend->Draw("same");
      vector<TString> pavetext;
      pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
      pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
      pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
      pavetext.push_back(Form("M_H=%d",mint));
      TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);
      text->Draw("same");
    }

    fout.close();
//     c[2]=CreateCanvas("canvas","canvas",1024,768);
//     h[0][0]->Draw("hist");
//     h[0][1]->Draw("E same");
    system("mkdir -vp fig/interpolate/");
    PrintCanvas(c[0], Form("fig/interpolate/results_%d",mint));
//     PrintCanvas(c[1], "fig/interpolate/320/results_fine");
//     PrintCanvas(c[2], "fig/interpolate/320/results_comp");
  }
  //======================================================================================
  //========================= Draw Two Limit in One Plot =================================
  if(Opt["MUHAT"]){

//     ifstream *fin[7];    vector<TString> legtext;
//     fin[0]=new ifstream("forSwag/ccl_v2_lvqq_mu.txt",ios::in); legtext.push_back("l#nuqq");
//     fin[1]=new ifstream("forSwag/ccl_v2_lvlv_mu.txt",ios::in); legtext.push_back("l#nul#nu");
//     fin[2]=new ifstream("forSwag/ccl_v2_llvv_mu.txt",ios::in); legtext.push_back("ll#nu#nu");
//     fin[3]=new ifstream("forSwag/ccl_v2_llqq_mu.txt",ios::in); legtext.push_back("llqq");
//     fin[4]=new ifstream("forSwag/ccl_v2_llll_mu.txt",ios::in); legtext.push_back("llll");
//     fin[5]=new ifstream("forSwag/ccl_v2_lh_mu.txt",ios::in);  legtext.push_back("lh");
//     fin[6]=new ifstream("forSwag/ccl_v2_gg_mu.txt",ios::in); legtext.push_back("#gamma#gamma");
//     double Mass[200];
//     double Muhat[7][200];
//     int nmass[7];
//     for(int i=0;i<7;i++){
//       int mass;
//       double obs, errhi, errlo;
//       int index=0;
//       while(!fin[i]->eof()){
// 	(*fin[i])>>mass>>obs>>errhi>>errlo;
// 	Mass[index]=mass;
// 	Muhad[i][index]=obs;
// 	index++;
//       }
//       nmass[i]=index;
//     }
//     TGraph *gr[7];
//     for(int i=0;i<7;i++){
    
//     }  
//       TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
//       legend->Draw("same");
//       vector<TString> pavetext;
//       pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
//       pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
//       pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
//       pavetext.push_back(Form("M_H=%d",mint));
//       TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);
//       text->Draw("same");
//     }

//     fout.close();
// //     c[2]=CreateCanvas("canvas","canvas",1024,768);
// //     h[0][0]->Draw("hist");
// //     h[0][1]->Draw("E same");
//     system("mkdir -vp fig/interpolate/");
//     PrintCanvas(c[0], Form("fig/interpolate/results_%d",mint));
// //     PrintCanvas(c[1], "fig/interpolate/320/results_fine");
// //     PrintCanvas(c[2], "fig/interpolate/320/results_comp");
  }
  //======================================================================================
  if(Opt["PVALUE"]){
    TString jobname=argc>2 ? argv[2] : "nominal";
    int imode=argc>3 ? atoi(argv[3]) : 1;
    int readinput=argc>4 ? atoi(argv[4]) : 0;
    string fileName[5];
    fileName[0]=jobname.Data();
    int nsample=1;
    TString type=(imode==1)?"llvv":"llqq";
    double mass[5][200];
    double pvalue[5][200];

    TCanvas *cv = CreateCanvas("canvas", "",1024,768);
    int nmass[5];
//     for(int i=0;i<=20;i++) mass[i]=200+20*i;
    vector<int> temp;
//     for(int Mass=200;Mass<290;Mass+=2){temp.push_back(Mass);} 
//     for(int Mass=290;Mass<350;Mass+=5) temp.push_back(Mass);
//     for(int Mass=350;Mass<400;Mass+=10) temp.push_back(Mass);
//     for(int Mass=400;Mass<=600;Mass+=20) temp.push_back(Mass);
    for(int Mass=200;Mass<=600;Mass+=20) temp.push_back(Mass);
    nmass[0]=temp.size();
    for(int i=0;i<nmass[0];i++) mass[0][i]=temp[i];
    system(Form("mkdir -vp fig/limit_plot/%s",jobname.Data()));

    if(readinput){
      ifstream fin(Form("fig/limit_plot/%s/pvalue.txt",jobname.Data()),ios::in);
      for(int jj=0;jj<nsample;jj++){
	assert(fin);
	nmass[jj]=0;
	string dummy;
	getline(fin,dummy);
	while(fin>>mass[jj][nmass[jj]]>>pvalue[jj][nmass[jj]]){
// 	  fin>>mass[jj][nmass[jj]]>>pvalue[jj][nmass[jj]];
	  nmass[jj]++;
	}
      }
      fin.close();
    }
    else{
      ofstream fout(Form("fig/limit_plot/%s/pvalue.txt",jobname.Data()),ios::out);
      fout<<Form("Mass \t Pvalue")<<endl;
      for(int jj=0;jj<nsample;jj++){
	for(int idx=0;idx<nmass[jj];idx++){
	  TString infile=Form("workspace/%s/%s/%s_%3.3d_combined_allsys_model.root", type.Data(),fileName[jj].c_str(),type.Data(),(int)mass[jj][idx]);
	  TFile *fFile=OpenFile(infile,"read");
	  RooWorkspace* fWorkspace = (RooWorkspace*) fFile->Get("combined");
	  ModelConfig* fMC = (ModelConfig*) fWorkspace->obj("ModelConfig");
	  RooAbsData* fData = fWorkspace->data("obsData");
	  RooRealVar* firstPOI = (RooRealVar*) fMC->GetParametersOfInterest()->first();
	  RooDataSet* poiToTest = new RooDataSet("poiToTest","",*fMC->GetParametersOfInterest());
	  firstPOI->setVal(0.);
	  RooArgSet poi(*firstPOI);
	  RooArgSet* nullParams = (RooArgSet*) poi.snapshot();
	  RooAbsPdf* pdf = fMC->GetPdf();
	  pvalue[jj][idx]=(CalculatePValue(fData, pdf, nullParams, poi)).second;

	  cout << "Mass "<<mass[jj][idx]<<" GeV"
	       << " Pvalue "<<pvalue[jj][idx]<<endl;

	  fout<<Form("%d \t %.2f",(int)mass[jj][idx],pvalue[jj][idx])<<endl;
	  fFile->Close();
	  // 	SafeDelete(fWorkspace);
	  // 	SafeDelete(fMC);
	  // 	SafeDelete(fData);
	  SafeDelete(fFile);
	}
      }
      fout.close();
    }
    TGraph *plot_pvalue[5];

    TList *leglist=new TList();
    vector<TString> legtext;
    vector<TString> legopt;
    TMultiGraph *likelihd_limit = new TMultiGraph("pvalue","pvalue;m_{H} [GeV];p_{0}");
    for(int jj=0;jj<nsample;jj++){	// WSN...
      plot_pvalue[jj]=new TGraph(nmass[jj],mass[jj],pvalue[jj]);
      likelihd_limit->Add(plot_pvalue[jj]);
      leglist->Add(plot_pvalue[jj]);legtext.push_back("Observed");legopt.push_back("LP");
    }


    double x1,y1,x2,y2;
    GetX1Y1X2Y2(cv,x1,y1,x2,y2);
    TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);

    vector<TString> pavetext;
    pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
    // It is ATLAS now. 2011-08-12 4:48 p.m.
//     pavetext.push_back("#bf{#it{ATLAS}}");
    if(imode==1) pavetext.push_back("H#rightarrowZZ#rightarrowll#nu#nu");
    else if(imode==2) pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
    else if(imode==4) pavetext.push_back("H#rightarrowZZ");
//     pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
    if(imode==1||imode==2) pavetext.push_back("#intLdt=4.7fb^{-1}, #sqrt{s}=7TeV");
    else if(imode==4) pavetext.push_back("#intLdt=2.05-4.8fb^{-1}, #sqrt{s}=7TeV");

    TPaveText* text=CreatePaveText(x2-0.4,y1+0.05,x2,y1+0.3,pavetext,0.05);

    cv->cd();


    likelihd_limit->Draw("APL");
    likelihd_limit->GetXaxis()->SetTitleSize(0.05);
    likelihd_limit->GetXaxis()->SetTitleOffset(1.3);
    likelihd_limit->GetYaxis()->SetLabelSize(0.045);
    likelihd_limit->GetYaxis()->SetTitleSize(0.05);
    likelihd_limit->SetMaximum(1);
    likelihd_limit->SetMinimum(1e-5);
    cv->SetLogy();

//     legend->Draw();

    text->Draw("same");
    DrawConstantLine(cv,SIG1,mass[0],nmass[0]);
    DrawConstantLine(cv,SIG2,mass[0],nmass[0]);
    DrawConstantLine(cv,SIG3,mass[0],nmass[0]);
    PrintCanvas(cv,Form("fig/limit_plot/%s/pvalue",jobname.Data()));

  }
  //======================================================================================
  if(Opt["PROFILE"]){
    string workspaceFileName=argv[2];
//     int imode=argc>3 ? atoi(argv[3]) : 1;
//     int mass = argc>4? atoi(argv[4]) : 200;
//     TString type=(imode==1 ? "llvv" : "llqq"); 
//     TString type="";
//     if(imode==1) type="llvv"; else if(imode==2) type="llqq"; else if(imode==3) type="4l";
    

//     string workspaceFileName=Form("/xdata04/yanght/workspace/%s/%s_%d_combined_allsys_model.root",type.Data(),type.Data(),mass);
    TFile *fw=OpenFile(workspaceFileName.c_str(),"read");
    assert(fw);
    RooWorkspace *w=(RooWorkspace *)fw->Get("combined");
    assert(w);
    ModelConfig* mc = (ModelConfig*) w->obj("ModelConfig");
    RooAbsData* data = w->data("obsData");
    assert(data);
    assert(mc);
    RooAbsPdf* pdf = mc->GetPdf();
    RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();
    mu->setConstant();
    mu->setVal(0);
    pdf->fitTo(*data);
    cout<<"Nuisance parameter profiling for mu = "<<mu->getVal()<<":"<<endl;
    mc->GetNuisanceParameters()->Print("v");
    return 0;
  }
  //================================ New limit plot ======================================
  if(Opt["NEW"]){
    TString jobname=argc>2 ? argv[2] : "nominal";
    int imode=argc>3 ? atoi(argv[3]) : 1;
    string fileName[5];
    int nsample=1;
    fileName[0]=jobname.Data(); 
    fileName[1]=jobname.Data();
    fileName[2]=jobname.Data();
    fileName[3]=jobname.Data();
//     fileName[4]="llvv_council_lowmass_rebin6";
//     string massList="lists/masslist";
    TString type=(imode==1)?"llvv":"llqq";
    double mass[5][200];
    double N2S[5][200],N1S[5][200],P1S[5][200],P2S[5][200],Median[5][200],Zero[5][200],Observed[5][200];
    double XSECT[5][200],XSECTU[5][200];
    TCanvas *cv = CreateCanvas("canvas", "",1024,768);
    int nmass[5];

    vector<int> temp;
    for(int Mass=280;Mass<600;Mass+=20) temp.push_back(Mass);
    for(int Mass=600;Mass<=900;Mass+=50) temp.push_back(Mass);

    nmass[0]=temp.size();
    for(int i=0;i<nmass[0];i++) mass[0][i]=temp[i];

    temp.clear();

    system(Form("mkdir -vp fig/limit_plot/%s",jobname.Data()));
    ifstream fxsect("Input/Xsect.txt");
    ofstream fout(Form("fig/limit_plot/%s/info_%s.txt",jobname.Data(),type.Data()),ios::out);
    fout<<Form("Mass \t -2S \t -1S \t Med \t +1S \t +2S \t Obs")<<endl;
    double max=0;
    for(int jj=0;jj<nsample;jj++){
      for(int idx=0;idx<nmass[jj];idx++){
// 	Info("Frame: BAND","%d mass points will be processed.",nmass[jj]);
	string infile=Form("out/%s/%3.3d.txt", fileName[jj].c_str(),(int)mass[jj][idx]);
	ifstream fin(infile.c_str(),ios::in);
	if(!fin){
	  cerr<<"File "<<infile.c_str()<<" does not exist."<<endl;
	  continue;
	}
	double n2s,n1s,median,p1s,p2s,observed,dummy;
	double dummymass,xsect,xsectu;
	fxsect>>dummymass>>xsect>>xsectu;
	fin>>dummy>>observed>>median>>p2s>>p1s>>n1s>>n2s;
	double epsilon=1e-3;
	if(fabs(dummymass-dummy)>epsilon){
	  cout<<"fuck"<<endl;getchar();
	}
	fin.close();
	if(imode==4) mass[jj][idx]/=10;
	cout << "Mass "<<mass[jj][idx]<<" GeV"<<endl;
	cout << "-2 sigma  band " << n2s << endl; N2S[jj][idx]=(median-n2s)*xsect;
	cout << "-1 sigma  band " << n1s << endl; N1S[jj][idx]=(median-n1s)*xsect;
	cout << "Median of band " << median << endl; Median[jj][idx]=(median)*xsect;
	cout << "+1 sigma  band " << p1s << endl; P1S[jj][idx]=(p1s-median)*xsect;
	cout << "+2 sigma  band " << p2s << endl; P2S[jj][idx]=(p2s-median)*xsect;
	cout << "Observed "<<observed<<endl;Observed[jj][idx]=observed*xsect;
	XSECT[jj][idx]=xsect;
	XSECTU[jj][idx]=xsect*xsectu;
	Zero[jj][idx]=0;
	if(p2s>max)max=p2s;
	if(observed>max)max=observed;
	
	//       fout<<mass[idx]<<" "<<n2s<<" "<<n1s<<" "<<median<<" "<<p1s<<" "<<p2s<<" "<<observed<<endl;
	fout<<Form("%d \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f",(int)mass[jj][idx],n2s,n1s,median,p1s,p2s,observed)<<endl;
      }
    }
    fxsect.close();
    TGraph *likelihd_limit_d[5];
    TGraph *likelihd_limit_c[5];
    TGraph *likelihd_limit_x[5];
    TGraphAsymmErrors *likelihd_limit_1sigma[5];
    TGraphAsymmErrors *likelihd_limit_2sigma[5];
    //TGraphErrors *likelihd_limit_xsigma[5];
    TGraphAsymmErrors *likelihd_limit_xsigma[5];
    TMultiGraph *likelihd_limit = new TMultiGraph("exclusionlimit_p","CL_{S} Exclusion Limits ;m_{H} [GeV]; 95% CL limit on #sigma");
//    TMultiGraph *likelihd_limit = new TMultiGraph("exclusionlimit_p",";;");

    TList *leglist=new TList();
    vector<TString> legtext;
    vector<TString> legopt;
    for(int jj=0;jj<nsample;jj++){	// WSN...
//       if(jj==1) continue;
      likelihd_limit_d[jj] = new TGraph(nmass[jj],mass[jj],Observed[jj]);
      likelihd_limit_d[jj]->SetLineWidth(2);
      likelihd_limit_d[jj]->SetLineStyle(1); 
      likelihd_limit_d[jj]->SetMarkerSize(0.9);

      likelihd_limit_c[jj] = new TGraph(nmass[jj],mass[jj],Median[jj]);
      likelihd_limit_c[jj]->SetLineWidth(2);
      
      likelihd_limit_c[jj]->SetLineStyle(2);

      likelihd_limit_1sigma[jj] = new TGraphAsymmErrors(nmass[jj],mass[jj],Median[jj],Zero[jj],Zero[jj],N1S[jj],P1S[jj]);
      likelihd_limit_1sigma[jj]->SetFillColor(kGreen);
      likelihd_limit_2sigma[jj] = new TGraphAsymmErrors(nmass[jj],mass[jj],Median[jj],Zero[jj],Zero[jj],N2S[jj],P2S[jj]);
      likelihd_limit_2sigma[jj]->SetFillColor(kYellow);

      likelihd_limit_x[jj] = new TGraph(nmass[jj],mass[jj],XSECT[jj]);
      likelihd_limit_x[jj]->SetLineWidth(2);
      likelihd_limit_x[jj]->SetLineStyle(2);
      likelihd_limit_x[jj]->SetLineColor(2);

      likelihd_limit_xsigma[jj] = new TGraphAsymmErrors(nmass[jj],mass[jj],XSECT[jj],Zero[jj],Zero[jj],XSECTU[jj],XSECTU[jj]);
      //likelihd_limit_xsigma[jj] = new TGraphErrors(nmass[jj],mass[jj],XSECT[jj],Zero[jj],XSECTU[jj]);
//       likelihd_limit_xsigma[jj]->SetLineStyle(3);
      likelihd_limit_xsigma[jj]->SetFillColor(6);
      likelihd_limit_xsigma[jj]->SetFillStyle(3005);    
      


      likelihd_limit->Add(likelihd_limit_2sigma[jj],"E3");  
      likelihd_limit->Add(likelihd_limit_1sigma[jj],"E3");
      likelihd_limit->Add(likelihd_limit_xsigma[jj],"E3");

      likelihd_limit->Add(likelihd_limit_d[jj], "LP");
      likelihd_limit->Add(likelihd_limit_c[jj], "L");
      likelihd_limit->Add(likelihd_limit_x[jj], "L");
    }


    double x1,y1,x2,y2;
    GetX1Y1X2Y2(cv,x1,y1,x2,y2);



    leglist->Add(likelihd_limit_d[0]);legtext.push_back("Observed");legopt.push_back("LP");
    leglist->Add(likelihd_limit_c[0]);legtext.push_back("Expected");legopt.push_back("L");
    leglist->Add(likelihd_limit_x[0]);legtext.push_back("#sigma_{SM}");legopt.push_back("L");
    leglist->Add(likelihd_limit_1sigma[0]);legtext.push_back("#pm 1#sigma");legopt.push_back("F");
    leglist->Add(likelihd_limit_2sigma[0]);legtext.push_back("#pm 2#sigma");legopt.push_back("F");
    leglist->Add(likelihd_limit_xsigma[0]);legtext.push_back("M_{H}^{3} Error");legopt.push_back("F");
    TLegend* legend=CreateLegend(x2-0.4,y2-0.3,x2,y2-0.01,leglist,legtext,legopt,0.05);
//     TList *leglist2=new TList();
//     vector<TString> legtext2,legopt2;
//     leglist2->Add(likelihd_limit_d[1]);legtext2.push_back("Period B-K");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[2]);legtext2.push_back("Period L-M");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[1]);legtext2.push_back("H#rightarrowZZ#rightarrow4l");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[2]);legtext2.push_back("H#rightarrowZZ#rightarrowll#nu#nu");legopt2.push_back("L");
//     leglist2->Add(likelihd_limit_d[3]);legtext2.push_back("H#rightarrowZZ#rightarrowllqq");legopt2.push_back("L");

//     TLegend* legend2=CreateLegend(x2-0.4,y1,x2,y1+0.3,leglist2,legtext2,legopt2,0.05);

    vector<TString> pavetext;
    pavetext.push_back("#bf{#it{ATLAS}} Preliminary");
    // It is ATLAS now. 2011-08-12 4:48 p.m.
//     pavetext.push_back("#bf{#it{ATLAS}}");
    if(imode==1) pavetext.push_back("H#rightarrowZZ#rightarrowll#nu#nu");
    else if(imode==2) pavetext.push_back("H#rightarrowZZ#rightarrowllqq");
    else if(imode==4) pavetext.push_back("H#rightarrowZZ");
//     pavetext.push_back("#intLdt=2.05fb^{-1}, #sqrt{s}=7TeV");
    if(imode==1||imode==2) pavetext.push_back("#intLdt=4.7fb^{-1}, #sqrt{s}=7TeV");
    else if(imode==4) pavetext.push_back("#intLdt=2.05-4.8fb^{-1}, #sqrt{s}=7TeV");

    TPaveText* text=CreatePaveText(x1,y2-0.3,x1+0.4,y2-0.01,pavetext,0.05);

    cv->cd();

    likelihd_limit->Draw("A");
    likelihd_limit->GetXaxis()->SetTitleSize(0.05);
    likelihd_limit->GetXaxis()->SetTitleOffset(1.3);
    likelihd_limit->GetYaxis()->SetLabelSize(0.045);
    likelihd_limit->GetYaxis()->SetTitleSize(0.05);
    likelihd_limit->SetMinimum(1e-3);
//     likelihd_limit->SetMaximum(1.2*max);


    //    likelihd_limit->SetMaximum(1e2);
    if(imode==4){likelihd_limit->SetMinimum(1e-1);cv->SetLogy();}
//     likelihd_limit->GetXaxis()->SetLabelSize(0.0);
//     likelihd_limit->GetYaxis()->SetLabelSize(0.0);
    cv->SetLogy();
//     if(imode==1) likelihd_limit->GetXaxis()->SetLimits(200,360);
    legend->Draw();
//     legend2->Draw();
    text->Draw("same");
//     DrawConstantLine(cv,1,mass[2],nmass[2]);

    PrintCanvas(cv,Form("fig/limit_plot/%s/newplot",jobname.Data()));
    fout.close();  
  }
  //======================================================================================
}
