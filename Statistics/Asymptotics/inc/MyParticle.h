#ifndef MYPARTICLE_H
#define MYPARTICLE_H

#include "CommonHead.h"

class UserVar : public TObject{
 public:
/* struct UserDef{ */
  TString name;
  double value;
 public:
  UserVar(){name="None";value=0;}
/*   UserVar(const UserVar& var){name=var.name;value=var.value;} */
  virtual ~UserVar(){}
  ClassDef(UserVar,1);
};

/* typedef struct UserDef UserVar; */

class MyParticle : public TObject{ 
 public:
  TLorentzVector P;      // 4 momentum
  TLorentzVector P_truth;      // Vertex
  int Q;
  int pdgID;
  int mpdgID;
  double userDef[5];

 public:
  MyParticle(){Q=pdgID=mpdgID=998;}  // Only 998!
  MyParticle(const MyParticle & q);
  virtual ~MyParticle() { }
  void SetUserVar(const char *varname, double varvalue,int idx);
  double GetUserVar(int idx);

  double Px(){return P.Px();}
  double Py(){return P.Py();}
  double Pz(){return P.Pz();}
  double E(){return P.E();}
  double Pt(){return P.Pt();}
  double Phi(){return P.Phi();}
  double Eta(){return P.Eta();}
  double M(){return P.M();}
  void SetE(double e){P.SetE(e);}

  double Px_truth(){return P_truth.Px();}
  double Py_truth(){return P_truth.Py();}
  double Pz_truth(){return P_truth.Pz();}
  double E_truth(){return P_truth.E();}
  double Pt_truth(){return P_truth.Pt();}
  double Phi_truth(){return P_truth.Phi();}
  double Eta_truth(){return P_truth.Eta();}
  double M_truth(){return P_truth.M();}
  void SetE_truth(double e){P_truth.SetE(e);}

  void SetPtEtaPhiE(double pt, double eta, double phi, double e);
  void SetPtEtaPhiE_truth(double pt, double eta, double phi, double e);
  void SetPtEtaPhiM(double pt, double eta, double phi, double m);
  void SetPtEtaPhiM_truth(double pt, double eta, double phi, double m);
  void SetP(TLorentzVector p){P=p;}
  void SetP_truth(TLorentzVector p_truth){P_truth=p_truth;}
  inline MyParticle & operator = (const MyParticle& q);
  inline MyParticle operator + (const MyParticle& q) const;

  ClassDef(MyParticle,5)    //Bilibili~
};

inline MyParticle MyParticle::operator+ (const MyParticle &q) const{
  MyParticle p(*this);
  p.P+=q.P;
  p.P_truth+=q.P_truth;
  return p;
}

inline MyParticle & MyParticle::operator= (const MyParticle &q){
  this->P=q.P;
  this->P_truth=q.P_truth;
  this->Q=q.Q;
  this->pdgID=q.pdgID;
  this->mpdgID=q.mpdgID;
  for(int idx=0;idx<5;idx++) this->userDef[idx]=q.userDef[idx];
  return *this;
}

#endif
