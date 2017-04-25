#include "CommonHead.h"

using namespace std;

int main(int argc,char **argv){
  if(argc<3){
    cout<<"Usage: ./Submit [jobname] [type] [mode]"<<endl;
    exit(0);
  }
  TString jobname=argv[1];
  TString type=argv[2];
  int imode= argc>3 ? atoi(argv[3]) : 1;
  bool interpolate= argc>4 ? atoi(argv[4]) : false;
  vector<int> temp;
  if(imode==1){
    cout<<"Switching to llvv..."<<endl;
    system("cat LCie_template.sh | sed 's|CHANNEL|llvv|g' | sed 's|MODE|1|g' > LCie.sh");
  }
  else if(imode==2){
    cout<<"Switching to llqq..."<<endl;
    system("cat LCie_template.sh | sed 's|CHANNEL|llqq|g' | sed 's|MODE|2|g' > LCie.sh");
  }
  else if(imode==3){
    cout<<"Switching to 4l..."<<endl;
    system("cat LCie_template.sh | sed 's|CHANNEL|4l|g' | sed 's|MODE|3|g' > LCie.sh");
  }
  else if(imode==4){
    cout<<"Switching to ZZ..."<<endl;
    system("cat LCie_template.sh | sed 's|CHANNEL|ZZ|g' | sed 's|MODE|4|g' > LCie.sh");
    for(int i=1100;i<1200;i+=10) temp.push_back(i);
    for(int i=1200;i<1300;i+=5) temp.push_back(i);
    for(int i=1300;i<1500;i+=10) temp.push_back(i);
    for(int i=1500;i<2900;i+=20) temp.push_back(i);
    for(int i=2900;i<3500;i+=50) temp.push_back(i);
    for(int i=3500;i<4000;i+=100) temp.push_back(i);
    for(int i=4000;i<=6000;i+=200) temp.push_back(i);
  }
  system(Form("sh prepare.sh %s",jobname.Data()));
  int nmass=temp.size();
//   for(int i=0;i<nmass;i++){
//     int Mass=temp[i];
//     if(Mass<1900||Mass>=1980) continue;
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }    
//   for(int Mass=200;Mass<=600;Mass+=20){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
//   if(interpolate){
//   for(int Mass=200;Mass<290;Mass+=2){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
  system(Form("sh FalCie.sh %s %d %s",jobname.Data(),280,type.Data()));
//   for(int Mass=200;Mass<290;Mass+=2){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
//   for(int Mass=290;Mass<350;Mass+=5){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
//   for(int Mass=350;Mass<400;Mass+=10){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
//   for(int Mass=400;Mass<600;Mass+=20){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
//   for(int Mass=600;Mass<=900;Mass+=50){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }

//   }
//   else{
//   for(int Mass=200;Mass<=600;Mass+=20){
//     system(Form("sh FalCie.sh %s %d %s",jobname.Data(),Mass,type.Data()));
//   }
  //   }
}
