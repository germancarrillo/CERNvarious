#!/usr/bin/env python
import math
import ROOT
from ROOT import *
GeV      = 1000
##

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

def CreateHists(h,sample):
    "Creatining Histrograms"
    h["njets"  ] = ROOT.TH1F("a"+sample+"_njets" ,";n_{jets};  Normalized to 1"                 ,12  ,0     ,12  );
    h["ngams"  ] = ROOT.TH1F("a"+sample+"_ngamma",";n_{#gamma};Normalized to 1"                 ,10  ,0     ,10  );
    h["ma"     ] = ROOT.TH1F("a"+sample+"_ma"    ,";m_{#gamma#gamma} [GeV]; Normalized to 1"    ,100 ,0     ,20  );
    h["pta"    ] = ROOT.TH1F("a"+sample+"_pta"   ,";p_{T}^{#gamma#gamma} [GeV]; Normalized to 1",100 ,0     ,800 );
    h["mH"     ] = ROOT.TH1F("a"+sample+"_mH"    ,";m_{4#gamma} [GeV]; Normalized to 1"         ,100 ,400   ,1400);
    h["ptH"    ] = ROOT.TH1F("a"+sample+"_ptH"   ,";p_{T}^{4#gamma} [GeV]; Normalized to 1"     ,100 ,8     ,500 );
    h["Hcosths"] = ROOT.TH1F("a"+sample+"_cosths",";cos#theta*;  Normalized to 1"               ,100 ,-1    ,1   );
    h["jpt"    ] = ROOT.TH1F("a"+sample+"_jpt"   ,";p_{T}^{jet} [GeV]; Normalized to 1"         ,100 ,0     ,800 );
    h["jeta"   ] = ROOT.TH1F("a"+sample+"_jeta"  ,";#eta_{jet}; Normalized to 1"                ,20  ,-5    ,5   );
    h["yyR"    ] = ROOT.TH1F("a"+sample+"_yyR"   ,";#DeltaR(#gamma#gamma); Normalized to 1"     ,100 ,0     ,0.5 );
    h["ypt"    ] = ROOT.TH1F("a"+sample+"_ypt"   ,";p_{T}^{#gamma} [GeV]; Normalized to 1"      ,100 ,0     ,800 );
    h["yeta"   ] = ROOT.TH1F("a"+sample+"_yeta"  ,";#eta_{#gamma}; Normalized to 1"             ,20  ,-5    ,5   );
    h["yauthor"] = ROOT.TH1F("a"+sample+"_yauthor",";yauthor; Normalized to 1"                  ,100 ,0     ,20  ); 
    h["yf1"    ] = ROOT.TH1F("a"+sample+"_yf1"    ,";yf1    ; Normalized to 1"                  ,100 ,-0.2  ,1   ); 
    h["yf3"    ] = ROOT.TH1F("a"+sample+"_yf3"    ,";yf3    ; Normalized to 1"                  ,100 ,-0.2  ,1   ); 
    h["yf1core"] = ROOT.TH1F("a"+sample+"_yf1core",";yf1core; Normalized to 1"                  ,100 ,-0.2  ,1   ); 
    h["yf3core"] = ROOT.TH1F("a"+sample+"_yf3core",";yf3core; Normalized to 1"                  ,100 ,-0.2  ,1.5 ); 
    h["yweta2" ] = ROOT.TH1F("a"+sample+"_yweta2" ,";yweta2 ; Normalized to 1"                  ,100 ,-0.02 ,0.05); 
    h["yfracs1"] = ROOT.TH1F("a"+sample+"_yfracs1",";yfracs1; Normalized to 1"                  ,100 ,-5    ,5   );      
    h["ye277"  ] = ROOT.TH1F("a"+sample+"_ye277"  ,";ye277  ; Normalized to 1"                  ,100 ,0     ,1500);      
    h["yReta"  ] = ROOT.TH1F("a"+sample+"_yReta"  ,";yReta  ; Normalized to 1"                  ,100 ,-1    ,3   );      
    h["yRphi"  ] = ROOT.TH1F("a"+sample+"_yRphi"  ,";yRphi  ; Normalized to 1"                  ,100 ,0     ,3   );      
    h["yEratio"] = ROOT.TH1F("a"+sample+"_yEratio",";yEratio; Normalized to 1"                  ,100 ,0     ,1   );      
    h["yRhad"  ] = ROOT.TH1F("a"+sample+"_yRhad"  ,";yRhad  ; Normalized to 1"                  ,100 ,-2    ,20  );      
    h["yRhad1" ] = ROOT.TH1F("a"+sample+"_yRhad1" ,";yRhad1 ; Normalized to 1"                  ,100 ,-1    ,10  );      
    h["yDeltaE"] = ROOT.TH1F("a"+sample+"_yDeltaE",";yDeltaE; Normalized to 1"                  ,100 ,0    ,30000);      
    return
##
def PlotHistograms(H,distro,samples):
    l = TLegend(0.65,0.50,0.93,0.92)
    l.SetFillColor(0)  
    l.SetFillStyle(0)
    l.SetBorderSize(0)           
    m = TPaveText(0.50,0.82,0.65,0.92,"brNDC") 
    m.SetFillColor(0)  
    m.SetFillStyle(0)
    m.SetBorderSize(0)           
    m.AddText("#bf{#it{ATLAS}} Internal")
    m.AddText("H#rightarrow#gamma#gamma")
    c = ROOT.TCanvas("c")             
    c.SetLogy()
    firsth = True
    for i,sample in enumerate(samples):
        if H[i][distro] is False: continue
        #
        H[i][distro].SetLineColor(i+2)    
        H[i][distro].SetLineWidth(2) 
        l.AddEntry(H[i][distro]," test sample","L") 
        H[i][distro].DrawNormalized("hist" if firsth is True else "hist,same") 
        firsth = False
    l.Draw()
    m.Draw()    
    c.SaveAs("plots_sim/"+distro+".png") 
    c.SaveAs("plots_sim/"+distro+".pdf") 
    c.SaveAs("plots_sim/"+distro+".root") 
    return
##

class candidate:
    "objects preselected: jets, electrons, muons, gammas or taus"    
    #  uncalibrated energy (sum of cells) in presampler in a 1x1 window in cells in eta X phi
    e011 = 0       
    #  uncalibrated energy (sum of cells) in presampler in a 3x3 window in cells in eta X phi
    e033 = 1      
    #  uncalibrated energy (sum of cells) in strips in a 3x2 window in cells in eta X phi
    e132 = 2      
    #  uncalibrated energy (sum of cells) in strips in a 15x2 window in cells in eta X phi
    e1152 = 3     
    #  transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1
    ethad1 = 4    
    #  ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3
    ethad = 5     
    #  E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)
    ehad1 = 6     
    #  E1/E = fraction of energy reconstructed in the first sampling, where E1 is energy in all strips belonging to the cluster and E is the total energy reconstructed in the electromagnetic calorimeter cluster
    f1 = 7        
    #  fraction of energy reconstructed in 3rd sampling
    f3 = 8       
    #  E1(3x1)/E = fraction of the energy reconstructed in the first longitudinal compartment of the electromagnetic calorimeter, where E1(3x1) the energy reconstructed in +/-3 strips in eta, centered around the maximum energy strip and E is the energy reconstructed in the electromagnetic calorimeter
    f1core = 9    
    #  E3(3x3)/E fraction of the energy reconstructed in the third compartment of the electromagnetic calorimeter, where E3(3x3), energy in the back sampling, is the sum of the energy contained in a 3x3 window around the maximum energy cell
    f3core = 10    
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x3 (in cell units eta X phi)
    e233 = 11      
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x5
    e235 = 12      
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 5x5
    e255 = 13      
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x7
    e237 = 14      
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7
    e277 = 15     
    #  uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x3
    e333 = 16      
    #  uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x5
    e335 = 17      
    #  uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x7
    e337 = 18      
    #  uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7
    e377 = 19      
    #  shower width using +/-3 strips around the one with the maximal energy deposit: 
    #	w3 strips = sqrt{sum(Ei)x(i-imax)^2/sum(Ei)}, where i is the number of the strip and imax the strip number of the most energetic one 
    weta1 = 20     
    #  the lateral width is calculated with a window of 3x5 cells using the energy weighted  sum over all cells, which depends on the particle impact point inside the cell: weta2 = 
    #	sqrt(sum Ei x eta^2)/(sum Ei) -((sum Ei x eta)/(sum Ei))^2, where Ei is the energy of the i-th cell 
    weta2 = 21     
    #  2nd max in strips calc by summing 3 strips
    e2ts1 = 22    
    #  energy of the cell corresponding to second energy maximum in the first sampling
    e2tsts1 = 23   
    #  shower shape in the shower core : [E(+/-3)-E(+/-1)]/E(+/-1), where E(+/-n) is the energy in +- n strips around the strip with highest energy
    fracs1 = 24    
    #  same as egammaParameters::weta1 but without corrections  on particle impact point inside the cell
    widths1 = 25   
    #  same as egammaParameters::weta2 but without corrections on particle impact point inside the cell
    widths2 = 26   
    #  relative position in eta within cell in 1st sampling 
    poscs1 = 27   
    #  relative position in eta within cell in 2nd sampling
    poscs2= 28  
    #  uncorr asymmetry in 3 strips in the 1st sampling 
    asy1 = 29    
    #  difference between shower cell and predicted track in +/- 1 cells
    pos = 30 
    #  Difference between the track and the shower positions: 
    #	sum_{i=i_m-7}^{i=i_m+7}E_i x (i-i_m) / sum_{i=i_m-7}^{i=i_m+7}E_i, 
    #	The difference between the track and the shower positions measured 
    #	in units of distance between the strips, where i_m is the impact cell 
    #	for the track reconstructed in the inner detector and E_i is the energy 
    #	reconstructed in the i-th cell in the eta direction for constant phi given by the track parameters
    pos7 = 31 
    #   barycentre in sampling 1 calculated in 3 strips
    barys1 =32   
    #  shower width is determined in a window detaxdphi = 0,0625 x 0,2, corresponding typically to 20 strips in 
    #eta : wtot1=sqrt{sum Ei x ( i-imax)^2 / sum Ei}, where i is the strip number and imax the strip number of the first local maximum
    wtots1 = 33   
    #  energy reconstructed in the strip with the minimal value between the first and second maximum
    emins1 = 34   
    #  energy of strip with maximal energy deposit
    emaxs1 = 35    
    #   1-ratio of energy in 3x3 over 3x7 cells; 
    #	E(3x3) = E0(1x1) + E1(3x1) + E2(3x3) + E3(3x3); E(3x7) = E0(3x3) + E1(15x3) + E2(3x7) + E3(3x7)
    r33over37allcalo = 36     
    # core energy in em calo  E(core) = E0(3x3) + E1(15x2) + E2(5x5) + E3(3x5)
    ecore = 37
    # e237/e277
    Reta = 38
    # e233/e237
    Rphi = 39
    # (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
    Eratio = 40
    # ethad/et
    Rhad = 41
    # ethad1/et
    Rhad1 = 42
    # e2tsts1-emins1
    DeltaE =43
    # number of enums 
    NumberOfShowerShapes = 44
    def __init__(self,pt,eta,phi,m):
        self.charge = 0
        self.P = ROOT.TLorentzVector()
        self.P.SetPtEtaPhiM(pt,eta,phi,m) 

##
# Main routine
##
ROOT.gROOT.LoadMacro("AtlasStyle.C") 
ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')
if(not ROOT.xAOD.Init().isSuccess()): print "Failed to load xAOD.Init()"
#

samples  = [("1","1")]  # masses of pseudoscalar a in GeV
treeName = "CollectionTree" 
H        = [] 
#

for sample in samples:
    fileNameR = "/data01/mc15_13TeV/mc15_13TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.merge.AOD.e3791_s2608_s2183_r7773_tid10347579_01/AOD.07994236._000001.pool.root.1"
    print fileNameR
    f = ROOT.TFile.Open(fileNameR)
    print "Making TransientTree"
    t = ROOT.xAOD.MakeTransientTree(f,treeName)    
    print "TransientTree DONE" 
    h = {} #
    CreateHists(h,sample[0])
    for entry in xrange( 0,t.GetEntries() ): 
        t.GetEntry(entry)
        weight = t.EventInfo.mcEventWeight()            
        #
        # object selections:
        #
        # jets:
        jets = []
        for j in xrange(t.AntiKt4EMTopoJets.size()):   
            jet = t.AntiKt4EMTopoJets.at(j)
            if jet.pt()/GeV > 20 and abs(jet.eta())<4.5: 
                h["jpt" ].Fill(jet.pt()/GeV,weight)
                h["jeta"].Fill(jet.eta(),weight)
                jetcandidate = candidate(jet.pt()/GeV,jet.eta(),jet.phi(),jet.m()/GeV)
                jets.append(jetcandidate)                 
        h["njets"].Fill(len(jets),weight)  
        # gamma:
        gammas = []
        for y in xrange( t.Photons.size()):
            gamma = t.Photons.at(y)
            incrak = False 
            if abs(gamma.eta())>1.37 and abs(gamma.eta())>1.52: incrak = True 
            if gamma.pt()/GeV > 55 and abs(gamma.eta())<2.37 and not incrack:
                gammacandidate = candidate(gamma.pt()/GeV,gamma.eta(),gamma.phi(),0)                
                h["ypt"    ].Fill(gamma.pt()/GeV,weight)
                h["yeta"   ].Fill(gamma.eta()   ,weight)
                h["yauthor"].Fill(gamma.author(),weight)
                h["yf1"    ].Fill(gamma.showerShapeValue(gammacandidate.f1)    ,weight)
                h["yf3"    ].Fill(gamma.showerShapeValue(gammacandidate.f3)    ,weight)
                h["yf1core"].Fill(gamma.showerShapeValue(gammacandidate.f1core),weight)
                h["yf3core"].Fill(gamma.showerShapeValue(gammacandidate.f3core),weight)
                h["yweta2" ].Fill(gamma.showerShapeValue(gammacandidate.weta2) ,weight)
                h["yfracs1"].Fill(gamma.showerShapeValue(gammacandidate.fracs1),weight)
                h["ye277"  ].Fill(gamma.showerShapeValue(gammacandidate.e277)  ,weight)
                h["yReta"  ].Fill(gamma.showerShapeValue(gammacandidate.Reta)  ,weight)
                h["yRphi"  ].Fill(gamma.showerShapeValue(gammacandidate.Rphi)  ,weight)
                h["yEratio"].Fill(gamma.showerShapeValue(gammacandidate.Eratio),weight)
                h["yRhad"  ].Fill(gamma.showerShapeValue(gammacandidate.Rhad)  ,weight)
                h["yRhad1" ].Fill(gamma.showerShapeValue(gammacandidate.Rhad1) ,weight)
                h["yDeltaE"].Fill(gamma.showerShapeValue(gammacandidate.DeltaE),weight)
                gammacandidate.charge=1
                gammas.append(gammacandidate)
        h["ngams"].Fill(len(gammas),weight) 

        #
        # event selections:
        #
        if(len(gammas)>=2):
            Higgs  = gammas[0].P +  gammas[1].P
            h["Hcosths"].Fill(getcosthetastar(gammas[0].P,gammas[1].P),weight)
            h["mH" ].Fill(Higgs.M() ,weight)
            h["ptH"].Fill(Higgs.Pt(),weight)            
            h["yyR"].Fill(deltaR(gammas,0,1))
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
