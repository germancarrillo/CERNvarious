import os, sys

#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_ggH_quark_mass_effects_Common.py')

###
# Read Mass from joboption name
###
int(s) 

mh2_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H750_* -> H02=750GeV
mh2=0
for s in mh2_str.split("_"):
    ss=s.replace("ggH","")              # in GeV
    if ss.isdigit():
        mh2 = int(ss)        
if mh2==0:
   raise RuntimeError("Heavy Higgs mass not set, mh2=0, check joOption name %s"%mh2_str)
###

ma1_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_2a2_* -> a01=2GeV
ma1=0
for s in ma1_str.split("_"):
    if s.replace("2a","").isdigit():  
        if "2a0" in s: 
            ss=s.replace("2a0","0.")     # decimal in GeV
        else:
            ss=s.replace("2a","")        # int in GeV  
        ma1 = float(ss)        
if ma1==0:
   raise RuntimeError("light Higgs-pseudoscalar mass not set, ma1=0, check joOption name %s"%ma1_str)
###

wh2_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE WIDTH (in %) WE WANT TO SIMULATE IN FORMAT LIKE: *_W10_* -> wh2=mh2*0.1 or _NWA_ -> wh2=4.08MeV
wh2=0
for s in wh2_str.split("_"):
    if "NWA" in s:
        wh2=4.088e-03
        break
    ss=s.replace("p","")
    if ss.replace("W","").isdigit():  
        if "W0" in ss: 
            ss=ss.replace("W0","0.")  # decimal in GeV
        else:
            ss=ss.replace("W","")     # int in GeV  
        wh2 = mh2*float(ss)/100.       
if wh2==0:
   raise RuntimeError("Heavy Higgs width not set, wh2=0, check joOption name %s"%wh2_str)

###

# Set Powheg variables, overriding defaults
# Note: width_H will be overwritten in case of CPS.
PowhegConfig.mass_H  = mh2
PowhegConfig.width_H = wh2

# Turn on the heavy quark effect
PowhegConfig.use_massive_b = True
PowhegConfig.use_massive_c = True

# Complex pole scheme or not (1 for NWA and 3(CPS) for SM)
PowhegConfig.bwshape = 1

# Dynamical scale (sqrt(pT(H)^2+mH^2) real emission contributions)
# Note: r2330 does not support this option. please use newer versions.
PowhegConfig.runningscale = 2

# EW correction
if PowhegConfig.mass_H <= 1000.:
  PowhegConfig.ew = 1
else:
  PowhegConfig.ew = 0

# Set scaling and masswindow parameters
hfact_scale    = 1.2
masswindow_max = 30.

# Calculate an appropriate masswindow and hfact
masswindow = masswindow_max
if PowhegConfig.mass_H <= 700.:
  masswindow = min( (1799.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
else:
  masswindow = min( (1999.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
PowhegConfig.masswindow = masswindow
PowhegConfig.hfact = PowhegConfig.mass_H / hfact_scale

# Increase number of events requested to compensate for filter efficiency
PowhegConfig.nEvents *= 2.

PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()


# Change PDG ID of SM Higgs to BSM Higgs-like scalar, for Pythia8

infile = 'PowhegOTF._1.events'

f1 = open( infile )
lines = f1.readlines()
f1.close()

newfile = infile+'.temp'
os.system('rm %s ' %newfile)
f2 = open(newfile,'w')

for num,line in enumerate(lines):
    if line.startswith('      25     1'):
        f2.write(line.replace('      25     1','      35     1'))
    else:
        f2.write(line)

f2.close()

os.system('rm %s ' %infile)
os.system('mv %s %s ' %(newfile,infile) )


#--------------------------------------------------------------
# Pythia8 showering
# note: Main31 is set in Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1' ]

#--------------------------------------------------------------
# Higgs Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += [ "35:onMode = off",             # decay of our Higgs
                             "35:m0 = "+str(mh2),           # Higgs mass (just in case)
                             "35:mWidth = "+str(wh2),       # Higgs width (just in case)
                             "35:addChannel 1 1 101 36 36",
                             "35:onIfMatch = 36 36",        # decay H --> AA    
                             "36:onMode = off",
                             "36:addChannel 1 1 101 22 22",
                             "36:m0 = "+str(ma1),           # set A mass
                             "36:mMin = 0.005",
                             "36:mWidth = 0.01",            # Narrow width approximation
                             "36:onIfMatch = 22 22"         # decay A --> 2photon
                             ]


#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.process     = "gg->H->aa->4photons"
evgenConfig.description = "POWHEG+PYTHIA8+EVTGEN, ggH H->aa->4photons"
evgenConfig.keywords    = [ "SM", "Higgs", "BSMHiggs", "4photon" ]
evgenConfig.contact     = [ 'j.beacham@cern.ch' ]


# for Hmass in `echo 750`; do for width in `echo NWA 1 5 7 10`; do for amass in `echo 001 01 02 03 05 07 1 2 3 5 10`; do cp org_POW.py `echo mc15_13TeV.341000.PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_ggH$Hmass\_W$width\p_2a$amass\_4y.py | sed 's|WNWAp|NWA|g'`; done ; done ; done; for Hmass in `echo 700 800`; do for width in `echo NWA`; do for amass in `echo 01 05 1`; do cp org_POW.py `echo mc15_13TeV.341000.PowhegPhtyia8EvtGen_CTEQ6L1_NMSSM_ggH$Hmass\_W$width\p_2a$amass\_4y.py | sed 's|WNWAp|NWA|g'`; done ; done ; done


