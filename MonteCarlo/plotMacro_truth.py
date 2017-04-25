#!/usr/bin/env python
import math
import ROOT
from ROOT import *
GeV      = 1000

MadGraph = True
Powheg   = False
Pythia   = True
##        
colorshift=0
#
def diffPhi(dPhi):
    if abs(dPhi) >  math.pi: 
        return abs(2*math.pi - abs(dPhi))
    return abs(dPhi)
##
def deltaR(cand,i,j):
    deltaEta=abs(cand[i].P.Eta() - cand[j].P.Eta())
    deltaPhi=diffPhi(cand[i].P.Phi() - cand[j].P.Phi())
    return math.sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi) 
##
def getcosthetastar(A1,A2):
    H = A1 + A2    
    q = ROOT.TLorentzVector() 
    q.SetPxPyPzE(0,0,(H.E()+H.Pz())/2.,(H.E()+H.Pz())/2.)
    q.Boost(-(H.BoostVector()))
    parton = q.Vect().Unit()    
    A1.Boost(-(H.BoostVector()))
    A2.Boost(-(H.BoostVector()))
    a1 = A1.Vect().Unit()
    a2 = A2.Vect().Unit()    
    return math.cos(parton.Angle(a1))
##
def getelicities(v1,v2,v3,v4):
    A1 = v1+v2
    A2 = v3+v4
    H = A1 + A2    
    q = ROOT.TLorentzVector() 
    q.SetPxPyPzE(0,0,(H.E()+H.Pz())/2.,(H.E()+H.Pz())/2.)
    q.Boost(-(H.BoostVector()))
    parton = q.Vect().Unit()    
    # costhetas
    A1.Boost(-(H.BoostVector()))
    A2.Boost(-(H.BoostVector()))
    a1 = A1.Vect().Unit()
    a2 = A2.Vect().Unit()  
    cthstr =  math.cos(parton.Angle(a1))  
    # Phi and Phi1
    vv1=v1; vv2=v2; vv3=v3; vv4=v4
    vv1.Boost(-(H.BoostVector()))
    vv2.Boost(-(H.BoostVector()))
    vv3.Boost(-(H.BoostVector()))
    vv4.Boost(-(H.BoostVector()))
    v1p = vv1.Vect()
    v2p = vv2.Vect()
    v3p = vv3.Vect()
    v4p = vv4.Vect()
    na = ROOT.TVector3(0,0,1.)
    n1p  = v1p.Cross(v2p).Unit()
    n2p  = v3p.Cross(v4p).Unit()
    nscp = na .Cross(a1 ).Unit()    
    phi  = (a1.Dot(n1p.Cross(n2p ))/abs(a1.Dot(n1p.Cross(n2p )))*math.acos(-n1p.Dot(n2p )))
    phi1 = (a1.Dot(n1p.Cross(nscp))/abs(a1.Dot(n1p.Cross(nscp)))*math.acos( n1p.Dot(nscp)))
    #Costh1,2
    A2_rfr_A1 = A2                       # it's still in H RFR
    A2_rfr_A1.Boost(-(A1.BoostVector())) # now it's in A1 RFR (both A1 and A2 are in H RFR)
    a2_rfr_A1 = A2_rfr_A1.Vect()
    A1_rfr_A2 = A1                       # it's still in H RFR
    A1_rfr_A2.Boost(-(A2.BoostVector())) # now it's in A2 RFR (both A1 and A2 are still in H RFR)
    a1_rfr_A2 = A1_rfr_A2.Vect()
    vv1.Boost(-(A1.BoostVector()))       # A1 and A2 still in H RFR: put photons in their A's reference frame
    vv3.Boost(-(A2.BoostVector()))
    cth1 = -(a2_rfr_A1.Dot(vv1.Vect())/abs(a2_rfr_A1.Mag()*vv1.Vect().Mag()))
    cth2 = -(a1_rfr_A2.Dot(vv3.Vect())/abs(a1_rfr_A2.Mag()*vv3.Vect().Mag()))
    return {"cthstr":cthstr,"phi1":phi1,"cth1":cth1,"cth2":cth2,"phi":phi}
##
def CreateHists(h,sample):
    "Creatining Histrograms"
    h["njets"      ] = ROOT.TH1F("a"+sample+"_njets"      ,";n_{jets};  Normalized to 1"                                        ,12  ,0   ,12  );   
    h["ngams"      ] = ROOT.TH1F("a"+sample+"_ngamma"     ,";n_{#gamma};Normalized to 1"                                        ,10  ,0   ,10  );
    #                                                                                                                                            
    h["m2y"        ] = ROOT.TH1F("a"+sample+"_m2y"        ,";m_{#gamma#gamma} [GeV]; Normalized to 1"                           ,100 ,0   ,20  );
    h["pt2y"       ] = ROOT.TH1F("a"+sample+"_pt2y"       ,";p_{T}^{#gamma#gamma} [GeV]; Normalized to 1"                       ,100 ,0   ,800 );
    h["m4y"        ] = ROOT.TH1F("a"+sample+"_m4y"        ,";m_{4#gamma} [GeV]; Normalized to 1"                                ,140 ,0   ,1400);
    h["pt4y"       ] = ROOT.TH1F("a"+sample+"_pt4y"       ,";p_{T}^{4#gamma} [GeV]; Normalized to 1"                            ,100 ,0   ,500 );
    #                                                                                                                                            
    h["ypt"        ] = ROOT.TH1F("a"+sample+"_ypt"        ,";p_{T}^{#gamma} [GeV]; Normalized to 1"                             ,100 ,0   ,800 );
    h["yeta"       ] = ROOT.TH1F("a"+sample+"_yeta"       ,";#eta_{#gamma}; Normalized to 1"                                    ,20  ,-5  ,5   );
    h["jpt"        ] = ROOT.TH1F("a"+sample+"_jpt"        ,";p_{T}^{jet} [GeV]; Normalized to 1"                                ,100 ,0   ,800 );
    h["jeta"       ] = ROOT.TH1F("a"+sample+"_jeta"       ,";#eta_{jet}; Normalized to 1"                                       ,20  ,-5  ,5   );
    #                                                                                                                                            
    h["yyR"        ] = ROOT.TH1F("a"+sample+"_yyR"        ,";#DeltaR(#gamma#gamma); Normalized to 1"                            ,200 ,0   ,0.10);
    h["yyRoffshel" ] = ROOT.TH1F("a"+sample+"_yyRoffshel" ,";#DeltaR(#gamma#gamma) Offshell H; Normalized to 1"                 ,200 ,0   ,0.10);
    h["Hcosthetas" ] = ROOT.TH1F("a"+sample+"_costhetas"  ,";cos#theta*;  Normalized to 1"                                      ,100 ,-1  ,1   );
    h["Hcostheta1" ] = ROOT.TH1F("a"+sample+"_costheta1"  ,";cos#theta_{1};  Normalized to 1"                                   ,100 ,-1 ,-0.98);
    h["Hcostheta2" ] = ROOT.TH1F("a"+sample+"_costheta2"  ,";cos#theta_{2};  Normalized to 1"                                   ,100 ,-1 ,-0.98);
    h["Hphi1"      ] = ROOT.TH1F("a"+sample+"_phi1"       ,";Elicity #phi_{1};  Normalized to 1"                                ,100 ,-3.2,3.2 );
    h["Hphi"       ] = ROOT.TH1F("a"+sample+"_phi"        ,";Elicity #phi;  Normalized to 1"                                    ,100 ,-3.2,3.2 );
    # matched                                                                                                                                    
    h["ma"         ] = ROOT.TH1F("a"+sample+"_ma"         ,";m_{a} [GeV]; Normalized to 1"                                      ,100 ,0   ,20  );
    h["pta"        ] = ROOT.TH1F("a"+sample+"_pta"        ,";p_{T}^{a} [GeV]; Normalized to 1"                                  ,100 ,0   ,800 );
    h["mH"         ] = ROOT.TH1F("a"+sample+"_mH"         ,";m_{H} [GeV]; Normalized to 1"                                      ,140 ,0   ,1400);
    h["ptH"        ] = ROOT.TH1F("a"+sample+"_ptH"        ,";p_{T}^{H} [GeV]; Normalized to 1"                                  ,100 ,0   ,500 );
    #                                                                                                                          
    h["m2ymatched" ] = ROOT.TH1F("a"+sample+"_m2ymatched" ,";Parent Matched  m_{#gamma#gamma} [GeV]; Normalized to 1"           ,100 ,0   ,20  );
    h["pt2ymatched"] = ROOT.TH1F("a"+sample+"_pt2ymatched",";Parent Matched  p_{T}^{#gamma#gamma} [GeV]; Normalized to 1"       ,100 ,0   ,800 );
    h["m4ymatched" ] = ROOT.TH1F("a"+sample+"_m4ymatched" ,";Parent matched  m_{4#gamma} [GeV]; Normalized to 1"                ,140 ,0   ,1400);
    h["pt4ymatched"] = ROOT.TH1F("a"+sample+"_pt4ymatched",";Parent Matched  p_{T}^{4#gamma} [GeV]; Normalized to 1"            ,100 ,0   ,500 );
    h["yyRmatched" ] = ROOT.TH1F("a"+sample+"_yyRmatched" ,";Parent Matched  #DeltaR(#gamma#gamma); Normalized to 1"            ,200 ,0   ,0.10);
    return
##
def PlotHistograms(H,distro,samples):
    "Plotting Histograms of a given distribution"
    l = TLegend(0.65,0.50 if colorshift==1 else 0.65,0.93,0.92)
    l.SetFillColor(0)  
    l.SetFillStyle(0)
    l.SetBorderSize(0)           
    m = TPaveText(0.50,0.82,0.65,0.92,"brNDC") 
    m.SetFillColor(0)  
    m.SetFillStyle(0)
    m.SetBorderSize(0)           
    m.AddText("#bf{#it{ATLAS}} Internal")
    m.AddText("H_{2}#rightarrowa_{1}a_{1}#rightarrow4#gamma")
    c = ROOT.TCanvas("c")             
    c.SetLogy()
    firsth = True
    for i,sample in enumerate(samples):
        if H[i][distro] is False: continue
        #
        generator = ""
        H[i][distro].SetLineWidth(2) 
        #
        if "MadGraph" in sample[0]:
            colorcode=i+1
            H[i][distro].SetLineColor(colorcode if colorcode<3 else colorcode+1)    
            H[i][distro].SetLineStyle(1)
            generator = "MadGraph HC ggF"
        if "Powheg" in sample[0]:
            colorcode=i-colorshift+1
            H[i][distro].SetLineColor(colorcode if colorcode<3 else colorcode+1)    
            H[i][distro].SetLineStyle(2)
            generator = "Powheg"
        if "ttH" in sample[0]:
            colorcode=i-colorshift+1
            H[i][distro].SetLineColor(colorcode if colorcode<3 else colorcode+1)    
            H[i][distro].SetLineStyle(3)
            generator = "Pythia ttH"       
        #
        l.AddEntry(H[i][distro],generator+" m_{a_{1}}="+sample[1].replace("0","0.")+"GeV","L") 
        H[i][distro].DrawNormalized("hist" if firsth is True else "hist,same") 
        firsth = False
    l.Draw()
    m.Draw()    
    c.SaveAs("plots_truth/"+distro+".png") 
    c.SaveAs("plots_truth/"+distro+".pdf") 
    c.SaveAs("plots_truth/"+distro+".root") 
    return
##
class candidate:
    "objects preselected: jets, electrons, muons, gammas or taus"
    def __init__(self,pt,eta,phi,m):
        self.charge = 0
        self.P = ROOT.TLorentzVector()
        self.P.SetPtEtaPhiM(pt,eta,phi,m) 
##
##
# Main routine
##
ROOT.gROOT.LoadMacro("AtlasStyle.C") 
ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')
if(not ROOT.xAOD.Init().isSuccess()): print "Failed to load xAOD.Init()"

# # General
# samples_org = [
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a01_4y","01"),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a02_4y","02"),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a03_4y","03"),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a05_4y","05"),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a1_4y" ,"1" ),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a2_4y" ,"2" ),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a3_4y" ,"3" ),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a5_4y" ,"5" ),
#     ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_H750_2a10_4y","10"),
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a05_4y"       ,"05"),       
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a1_4y"        ,"1" ),       
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a2_4y"        ,"2" ),       
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a3_4y"        ,"3" ),       
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a5_4y"        ,"5" ),       
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_H750_2a10_4y"       ,"10")       
#     ]

# ttH
samples_org = [
    ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_ggH750_NWA_2a05_4y","05"), 
    ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_ggH750_NWA_2a1_4y" ,"1" ), 
    ("MadGraphPythia8EvtGen_A14NNPDF23LO_NMSSM_ggH750_NWA_2a5_4y" ,"5" ), 
    ("Pythia8EvtGen_A14NNPDF23LO_ttH750_NWA_2a05_4y"              ,"05"),
    ("Pythia8EvtGen_A14NNPDF23LO_ttH750_NWA_2a1_4y"               ,"1" ),
    ("Pythia8EvtGen_A14NNPDF23LO_ttH750_NWA_2a5_4y"               ,"5" )
    ]
colorshift=3
 
# # New MG5
# samples_org = [
#     ("MadGraphPythia8_A14NNPDF23NLO_HC_ggH750_W10p_2a05_4y"  ,"05"),
#     ("MadGraphPythia8_A14NNPDF23NLO_HC_ggH750_W10p_2a1_4y"   ,"1"),
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_ggH750_W10p_2a05_4y" ,"05"),
#     ("PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_ggH750_W10p_2a1_4y"  ,"1")
#     ]
# colorshift=2

samples = []

for sample in samples_org:
    if MadGraph is False and "MadGraph" in sample[0]: continue   
    if Powheg   is False and "Powheg"   in sample[0]: continue   
    if Pythia   is False and "ttH"      in sample[0]: continue   
    samples.append(sample) 

treeName = "CollectionTree" 
H        = [] 
#
for sample in samples:   
    #
    fileNameR = "/data01/montoya/workarea/Xaa4y/MCProd/mc15_13TeV.341000."+sample[0]+".DAOD_TRUTH.e3698/DAOD_TRUTH1.NTUP.0341000._000001.root"    
    print fileNameR
    f = ROOT.TFile.Open(fileNameR)
    print "Making TransientTree"
    t = ROOT.xAOD.MakeTransientTree(f,treeName,0)    
    print "TransientTree DONE" 
    h = {} #
    generator="_"
    if "MadGraph" in sample[0]: generator+="MadGraph"
    if "Powheg"   in sample[0]: generator+="Powheg"
    if "ttH"      in sample[0]: generator+="Pythia"
    CreateHists(h,sample[1]+generator)
    for entry in xrange( 0, t.GetEntries()): 
        t.GetEntry(entry)
        weight = t.EventInfo.mcEventWeight()            
        #
        # object selections:
        #
        # jets:
        jets = []
        for j in xrange(t.AntiKt4TruthJets.size()):
            jet = t.AntiKt4TruthJets.at(j)
            if jet.pt()/GeV > 20 and abs(jet.eta())<4.5: 
                h["jpt" ].Fill(jet.pt()/GeV,weight)
                h["jeta"].Fill(jet.eta(),weight)
                jetcandidate = candidate(jet.pt()/GeV,jet.eta(),jet.phi(),jet.m()/GeV)
                jets.append(jetcandidate)                 
        h["njets"].Fill(len(jets),weight)  
        # gamma:
        gammas = []
        for y in xrange( t.TruthPhotons.size()):
            gamma = t.TruthPhotons.at(y)
            if gamma.pt()/GeV > 20 and abs(gamma.eta())<2.5:
                h["ypt" ].Fill(gamma.pt()/GeV,weight)
                h["yeta"].Fill(gamma.eta(),weight)
                gammacandidate = candidate(gamma.pt()/GeV,gamma.eta(),gamma.phi(),0)                
                gammacandidate.charge=1
                gammas.append(gammacandidate)
        h["ngams"].Fill(len(gammas),weight) 
        #
        # event selections:
        #
        dR=10
        for i in xrange(0,len(gammas)-1):
            for j in xrange(i+1,len(gammas)):
                if deltaR(gammas,i,j)<dR: 
                    dR=deltaR(gammas,i,j)
                    y_1=gammas[i] 
                    y_2=gammas[j]
        h["yyR"].Fill(dR)
        dR=10
        for i in xrange(0,len(gammas)-1):
            for j in xrange(i+1,len(gammas)):
                if y_1 != gammas[i] and y_1 != gammas[j] and y_2 != gammas[i]  and y_2 != gammas[j] and deltaR(gammas,i,j)<dR:
                    dR=deltaR(gammas,i,j)
                    y_3=gammas[i] 
                    y_4=gammas[j]
        if dR>=10: continue
        h["yyR"].Fill(dR)
        yy_1 = y_1.P + y_2.P  
        yy_2 = y_3.P + y_4.P  
        h["m2y" ].Fill(yy_1.M() ,weight)
        h["m2y" ].Fill(yy_2.M() ,weight)
        h["pt2y"].Fill(yy_1.Pt(),weight)
        h["pt2y"].Fill(yy_2.Pt(),weight)
        if(len(gammas)>=4):
            angles = getelicities(y_1.P,y_2.P,y_3.P,y_4.P) 
            h["Hcosthetas"].Fill(angles["cthstr"],weight)
            h["Hcostheta1"].Fill(angles["cth1"  ],weight)
            h["Hcostheta2"].Fill(angles["cth2"  ],weight)
            h["Hphi1"     ].Fill(angles["phi1"  ],weight)
            h["Hphi"      ].Fill(angles["phi"   ],weight)
            yyyy = yy_1 + yy_2
            h["m4y" ].Fill(yyyy.M() ,weight)
            h["pt4y"].Fill(yyyy.Pt(),weight)            
        #
        # Truth particles
        #        
        truths = []
        for i in xrange( t.TruthParticles.size()):
            truth = t.TruthParticles.at(i)
            if abs(truth.pdgId())==35: 
                h["mH" ].Fill(truth.m()/GeV,weight)
                h["ptH"].Fill(truth.pt()/GeV,weight)                
                if truth.m()/GeV<500: h["yyRoffshel"].Fill(dR)
            if abs(truth.pdgId())==36: 
                h["ma" ].Fill(truth.m()/GeV,weight)
                h["pta"].Fill(truth.pt()/GeV,weight)
            if abs(truth.pdgId())==22 and abs(truth.parent().pdgId())==36:  
                truthcandidate = candidate(truth.pt()/GeV,truth.eta(),truth.phi(),truth.m()/GeV)
                truths.append(truthcandidate)          
        dR=10
        for i in xrange(0,len(truths)-1):
            for j in xrange(i+1,len(truths)):
                if deltaR(truths,i,j)<dR: 
                    dR=deltaR(truths,i,j)
                    y_1=truths[i] 
                    y_2=truths[j]
        h["yyRmatched"].Fill(dR)
        dR=10
        for i in xrange(0,len(truths)-1):
            for j in xrange(i+1,len(truths)):
                if y_1 != truths[i] and y_1 != truths[j] and y_2 != truths[i]  and y_2 != truths[j] and deltaR(truths,i,j)<dR:
                    dR=deltaR(truths,i,j)
                    y_3=truths[i] 
                    y_4=truths[j]
        if dR>=10: continue
        h["yyRmatched"].Fill(dR)                
        yy_1 = y_1.P + y_2.P  
        yy_2 = y_3.P + y_4.P  
        h["m2ymatched" ].Fill(yy_1.M() ,weight)
        h["m2ymatched" ].Fill(yy_2.M() ,weight)
        h["pt2ymatched"].Fill(yy_1.Pt(),weight)
        h["pt2ymatched"].Fill(yy_2.Pt(),weight)
        if(len(truths)==4):
            yyyy = yy_1 + yy_2
            h["m4ymatched" ].Fill(yyyy.M() ,weight)
            h["pt4ymatched"].Fill(yyyy.Pt(),weight)            
        else: 
            print "More than 4 matched photons found... remove duplicated"
    # end of event loop
    H.append(h) 

#end of sample loop    

#
# pltting and printing
#
SetAtlasStyle()
for distro in H[0]: PlotHistograms(H,distro,samples)
'''
#t.Draw("@AntiKt4TruthJets.size()>>h","EventInfo.mcEventWeight()") 
'''
