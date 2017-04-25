#include <iostream>
#include "MyParticle.h"

using namespace std;
ClassImp(UserVar);
ClassImp(MyParticle);

MyParticle::MyParticle(const MyParticle & q){
  this->P=q.P;
  this->P_truth=q.P_truth;
  this->Q=q.Q;
  this->pdgID=q.pdgID;
  this->mpdgID=q.mpdgID;
  for(int idx=0;idx<5;idx++) this->userDef[idx]=q.userDef[idx];
}

void MyParticle::SetUserVar(const char *varname, double varvalue,int idx){
  if(idx<0||idx>4){
    Error("SetUserVar","Variable index out of range.");
    exit(0);
  }
  else userDef[idx]=varvalue;
}

double MyParticle::GetUserVar(int idx){
  if(idx<0||idx>4){
    Error("GetUserVar","Variable index out of range.");    
    return -1;
  }
  else return userDef[idx];
}

void MyParticle::SetPtEtaPhiE(double pt, double eta, double phi, double e){
  P.SetPtEtaPhiE(pt,eta,phi,e);
}

void MyParticle::SetPtEtaPhiE_truth(double pt, double eta, double phi, double e){
  P_truth.SetPtEtaPhiE(pt,eta,phi,e);
}

void MyParticle::SetPtEtaPhiM(double pt, double eta, double phi, double m){
  P.SetPtEtaPhiM(pt,eta,phi,m);
}

void MyParticle::SetPtEtaPhiM_truth(double pt, double eta, double phi, double m){
  P_truth.SetPtEtaPhiM(pt,eta,phi,m);
}
