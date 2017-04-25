#    April, 2016
#--------------------------------------------------------------
# on-the-fly generation of heavy H -> yy MG5 events, montoya@cern.ch
#--------------------------------------------------------------

def build_madfks_inc(madfks_card_old=None,madfks_card_new='madfks_mcatnlo.inc.new'):
    """Build a new madfks_card.dat from an existing one.
    Params should be a dictionary of dictionaries. The first key is the block name, and the second in the param name.
    Eventually madfkss will replace the other arguments, but we need to keep them for backward compatibility for now."""
    # Grab the old madfks card and move it into place
    madfkscard = subprocess.Popen(['get_files','-data',madfks_card_old])
    madfkscard.wait()
    if not os.access(madfks_card_old,os.R_OK):
        mglog.info('Could not get madfks card '+madfks_card_old)
    if os.access(madfks_card_new,os.R_OK):
        mglog.error('Old madfks card at'+str(madfks_card_new)+' in the current directory. Dont want to clobber it. Please move it first.')
        return -1

    oldcard = open(madfks_card_old,'r')
    newcard = open(madfks_card_new,'w')
    lowfrac=0.025
    highfrac=0.25
    for line in oldcard:
        print "line ",line
        print "T/F ",line.find("frac_low=")
        if line.find("frac_low=")!= -1 :
            newcard.write('      parameter (frac_low=%1.3fd0) \n' % (lowfrac))
        elif line.find("frac_upp=")!= -1 :
            newcard.write('      parameter (frac_upp=%fd0) \n' % (highfrac))
        else:
            newcard.write(line)
	
    oldcard.close()
    newcard.close()
    return newcard		

from MadGraphControl.MadGraphUtils import *
#
nevents=10000
mode=0
do_newtheory=0

# DSID lists (extensions can include systematics samples)
decay_2y=[341000] # dummy
for x in xrange(341524,341541): decay_2y.append(x)


fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in decay_2y or runArgs.runNumber in decay_2b:
    fcard.write("""
    set group_subprocesses Auto
    set ignore_six_quark_processes False   
    set loop_optimized_output True
    set complex_mass_scheme False
    import model sm
    define p = g u c d s t b u~ c~ d~ s~ t~ b~
    define j = g u c d s t b u~ c~ d~ s~ t~ b~
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    import model nmssm --modelname
    generate p p > h01, h01 > a a
    add process p p > a01
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

#   import model NMSSM_UFO  

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#--------------------------------------------------------------
# Higgs masses and widths in GeV
#--------------------------------------------------------------

import math
mh1=1.250e+02                 
mh2=7.500e+02 
ma1=1.000e+01 
mhc=mh2
mh3=mh2
ma2=mh2
wh1=4.088e-03 	
wh2=4.088e-03 	  # NWA
wa1=2.495587e-04  # default in param_card
whc=3.250011e+00  # default in param_card 
wh3=1.118083e+00  # default in param_card
wa2=3.509479e+00  # default in param_card

###
# Read Mass and Width from joboption name
###
int(s) 

mh2_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_ggH750_* -> H02=750GeV
mh2=0
for s in mh2_str.split("_"):
    ss=s.replace("ggH","")           # in GeV
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

#Fetch default LO run_card.dat and set parameters. using nn23lo instead of cteq6l1

extras = { 'lhe_version':'3.0',                        # 
           'cut_decays':'F',                           # lhe version fixed by arrange_output function   
           'pdlabel':"'nn23lo'",
           'parton_shower':'PYTHIA8',                   
           'fixed_ren_scale':'F',                      # Dynamic scale
           'fixed_fac_scale':'F'                       # Dynamic scale
           }

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)


#Build param_card.dat from UFO, and set Higgs masses
#write_param_card is part of the nmssm model

import os
import sys
sys.path.append( os.environ['MADPATH']+'/models/nmssm/' )
from write_param_card import ParamCardWriter
ParamCardWriter('param_card.TMP.dat')
masses = {'25':str(mh1)+' # MH01',
          '35':str(mh2)+' # MH02',
          '36':str(ma1)+' # MxA01',
          '37':str(mhc)+' # MH',
          '45':str(mh3)+' # MH03',
          '46':str(ma2)+' # MxA02'
          }
decays = {'25':'DECAY  25 '+str(wh1),
          '35':'DECAY  35 '+str(wh2)
          }

build_param_card(param_card_old='param_card.TMP.dat',param_card_new='param_card.dat',masses=masses,decays=decays)

print_cards()

runName='run_01'     

if do_newtheory:
    #copy file to running dir 
    if os.access(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc',os.R_OK):
        shutil.copy(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc','madfks_mcatnow.SM.inc')
    else:
        raise RuntimeError('Cannot find default NLO madfks_mcatnow.SM.inc!')
    #modify file

    build_madfks_inc(madfks_card_old='madfks_mcatnow.SM.inc', madfks_card_new='madfks_mcatnlo.inc.new')

    #copy it back
    if os.access(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc',os.R_OK):
        shutil.copy('madfks_mcatnlo.inc.new',os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc')
    else:
        raise RuntimeError('Cannot find default NLO madfks_mcatnow.inc.new!')

    os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_gg_hpbtx/fastjet')
    os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_uux_hpbtx/fastjet')
    os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_uxu_hpbtx/fastjet')
    os.system("/bin/ln -s " + os.environ['FASTJETPATH'] + "/lib/* "+os.environ['PWD']+'/'+process_dir+'/lib')


generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,lhe_version=3,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

# Shower

include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_aMcAtNlo.py")

evgenConfig.description = 'aMcAtNlo NMSSM High mass Higgs to a01 to 2gammas'
evgenConfig.keywords+=['Higgs','MSSM','BSMHiggs']
evgenConfig.contact = ['G. Carrillo-Montoya']

evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

do2y=False

for s in mh2_str.split("_"):
    ss=s.replace(".py","")  
    print ss
    if ss=='2y' and runArgs.runNumber in decay_2y:
        do2y=True

if do2y==False:
    raise RuntimeError("No decay mode was identified, check jobOption name %s, and/or runNumber %i."%(runArgs.jobConfig[0],runArgs.runNumber))

if do2y:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "36:onMode = off",                   # turn off all ma1 decays
                                "36:onIfMatch = 22 22"]              # switch on a01 to 2y
#   evgenConfig.keywords+=['','']
    

