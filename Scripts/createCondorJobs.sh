#!/bin/bash

source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.04/x86_64-slc5-gcc43-dbg/root/bin/thisroot.sh

folder=$1
shift=1

pushd ../ 
tar -cvzf code.tgz code/ 
popd 
mv ../code.tgz .

mkdir -p /data6/montoya/statisticsdata/$folder/fig/
mkdir -p /data6/montoya/statisticsdata/$folder/root/
mkdir -p /data6/montoya/statisticsdata/$folder/workspaces/
mkdir -p /data6/montoya/statisticsdata/log/asymptotic_limits_$folder
mkdir -p /data6/montoya/statisticsdata/job/asymptotic_limits_$folder
cp listmasses code.tgz asymptotics.sh /data6/montoya/statisticsdata/job/asymptotic_limits_$folder
pushd /data6/montoya/statisticsdata/job/asymptotic_limits_$folder


for m in `cat listmasses | grep -v '#'` 
  do
  for f in `seq 0 1 1`
    do
#########################################################
#       Create script for submitting to Condor          #
#########################################################
cat > asymptotics_$m.GeV_$f.job <<EOF
Universe        = vanilla
Notification    = Never
Executable      = $PWD/asymptotics.sh  
Arguments       = $m $f $folder
GetEnv          = True
Output          = /data6/montoya/statisticsdata/log/asymptotic_limits_$folder/asymptotics_$m.GeV_$f.out 
Error           = /data6/montoya/statisticsdata/log/asymptotic_limits_$folder/asymptotics_$m.GeV_$f.err 
Log             = /data6/montoya/statisticsdata/log/asymptotic_limits_$folder/asymptotics_$m.GeV_$f.log 
Priority        = 600								    
notification    = Never								    
stream_output   = False 							    
stream_error    = False 							    
#+Group          = "ATLAS"							    
#+IsCommittedJob  = TRUE							    
Requirements    = ARCH == "X86_64"						    
+IsFastJob = True								    
#Requirements = TARGET.IsFastQueueSlot 						    
#Requirements = ARCH == "INTEL" || ARCH == "X86_64"				    
transfer_input_files = $PWD/code.tgz
WhenToTransferOutput = ON_EXIT_OR_EVICT 
OnExitRemove    = TRUE
Queue
EOF
########################################################
condor_submit asymptotics_$m.GeV_$f.job 
echo " mass $m GeV - flag $f"
  done
done
popd
