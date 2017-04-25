#!/bin/bash

filename=$1
shift 1

filename=`echo $filename | sed 's|.py||g' | sed 's|MC15JobOptions/||g'`

dsid=`echo $filename | awk -F"." '{print $2}'`

filenameEVNT=$filename.EVNT.e3698/
filenameTRUTH=$filename.DAOD_TRUTH.e3698/
#
filenameHITSF=$filename.HITS.e3698_a766/
filenameAODF=$filename.AOD.e3698_a766_a822/
filenameHITS=$filename.HITS.e3698_s2608/
filenameAOD=$filename.AOD.e3698_s2608_r7773/

setupATLAS --quiet

generate(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:GENERATING +++++++++++++++++++++++++++++++++++++++++++++++++'
    mkdir -p MCProd/$filenameEVNT
    cp MC15JobOptions/  MCProd/$filenameEVNT/ -r
    pushd MCProd/$filenameEVNT/
    namejoboption=MC15JobOptions/$filename.py
    unset CMTPATH
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.5/AtlasProduction/19.2.5.6/AtlasProductionRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.5/cmtsite/asetup.sh 19.2.5.6,notest --cmtconfig x86_64-slc6-gcc47-opt 
    Generate_tf.py --ecmEnergy=13000 --firstEvent=1 --jobConfig=$namejoboption --maxEvents=5000 --outputEVNTFile=EVNT.0$dsid._000001.pool.root.1 --randomSeed=1 --runNumber=$dsid --skipEvents=0
    popd

    #latest:      19.2.5.5
    #tested with: 19.2.5.6
}

truth(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:TRUTHNTUP ++++++++++++++++++++++++++++++++++++++++++++++++++'
    mkdir -p  MCProd/$filenameTRUTH 
    cp MCProd/$filenameEVNT/EVNT.0$dsid._000001.pool.root.1 MCProd/$filenameTRUTH/
    pushd MCProd/$filenameTRUTH/
    unset CMTPATH
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.7.3/AtlasDerivation/20.7.3.1/AtlasDerivationRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.7.3/cmtsite/asetup.sh 20.7.3.1,AtlasDerivation,gcc48,here 
    Reco_tf.py --inputEVNTFile EVNT.0$dsid._000001.pool.root.1 --outputDAODFile NTUP.0$dsid._000001.root --reductionConf TRUTH1
    popd  
}

hitsfast(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:SIMULATION FASTSIM +++++++++++++++++++++++++++++++++++++++++'
    mkdir -p MCProd/$filenameHITSF
    cp MCProd/$filenameEVNT/EVNT.0$dsid._000001.pool.root.1 MCProd/$filenameHITSF/
    pushd MCProd/$filenameHITSF/
    unset CMTPATH;
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.3/AtlasProduction/19.2.3.8/AtlasProductionRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.3/cmtsite/asetup.sh 19.2.3,notest --cmtconfig x86_64-slc6-gcc47-opt 
    Sim_tf.py --inputEVNTFile=EVNT.0$dsid._000001.pool.root.1 --DBRelease="default:current" --DataRunNumber=222525 --conditionsTag "default:OFLCOND-RUN12-SDR-19" --firstEvent=1 --geometryVersion="default:ATLAS-R2-2015-03-01-00_VALIDATION" --maxEvents=3000 --outputHITSFile=HITS.0$dsid._000001.pool.root.1 --physicsList=FTFP_BERT --postInclude "default:PyJobTransforms/UseFrontier.py" --preInclude "EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py" --randomSeed=1 --runNumber=$dsid --simulator=ATLFASTII --skipEvents=0
    popd
}

recofast(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:RECONSTRUTING FASTSIM ++++++++++++++++++++++++++++++++++++++'
    mkdir -p MCProd/$filenameAODF
    cp MCProd/$filenameHITSF/HITS.0$dsid._000001.pool.root.1 MCProd/$filenameAODF/
    pushd MCProd/$filenameAODF/
    unset CMTPATH;
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/AtlasProduction/20.7.5.1/AtlasProductionRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/cmtsite/asetup.sh AtlasProd1,20.7.5.1.1 --cmtconfig x86_64-slc6-gcc49-opt
    Reco_tf.py --inputHITSFile=HITS.0$dsid._000001.pool.root.1 --maxEvents=3000 --postExec "all:CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"]" "ESDtoAOD:fixedAttrib=[s if \"CONTAINER_SPLITLEVEL = \'99\'\" not in s else \"\" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes]; svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib" --postInclude "default:RecJobTransforms/UseFrontier.py" --preExec "all:rec.Commissioning.set_Value_and_Lock(True); from AthenaCommon.BeamFlags import jobproperties; jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0); from LArROD.LArRODFlags import larRODFlags; larRODFlags.NumberOfCollisions.set_Value_and_Lock(20); larRODFlags.nSamples.set_Value_and_Lock(4); larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True); larRODFlags.firstSample.set_Value_and_Lock(0); larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)" "RAWtoESD:from CaloRec.CaloCellFlags import jobproperties; jobproperties.CaloCellFlags.doLArCellEmMisCalib=False" "ESDtoAOD:TriggerFlags.AODEDMSet=\"AODSLIM\"" --preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_v3.py" "RDOtoRDOTrigger:RecExPers/RecoOutputMetadataList_jobOptions.py" --skipEvents=0 --autoConfiguration=everything --conditionsTag "default:OFLCOND-MC15c-SDR-09" --geometryVersion="default:ATLAS-R2-2015-03-01-00" --runNumber=$dsid --digiSeedOffset1=2 --digiSeedOffset2=2 --digiSteeringConf='StandardSignalOnlyTruth' --AMITag=a822 --steering="doRDO_TRIG" --inputHighPtMinbiasHitsFile=/data05/mc15_13TeV/mc15_13TeV.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2195/HITS.05608152._[000001,000002,000003,000006].pool.root --inputLowPtMinbiasHitsFile=/data05/mc15_13TeV/mc15_13TeV.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2195/HITS.05608147._[000005,000006,000007,000014].pool.root --numberOfCavernBkg=0 --numberOfHighPtMinBias=0.12268057 --numberOfLowPtMinBias=39.8773194 --pileupFinalBunch=6 --outputAODFile=AOD.0$dsid._000001.pool.root.1 --jobNumber=1 --triggerConfig="RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2046,20,56" --ignorePatterns=Py:TrigConf2COOLLib.py.+ERROR.===================================+
    popd  
}

hitsfull(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:SIMULATING ++++++++++++++++++++++++++++++++++++++++++++++++++'
    mkdir -p MCProd/$filenameHITS 
    cp MCProd/$filenameEVNT/EVNT.0$dsid._000001.pool.root.1 MCProd/$filenameHITS/
    pushd MCProd/$filenameHITS/
    unset CMTPATH;
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.3/AtlasProduction/19.2.3.8/AtlasProductionRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.3/cmtsite/asetup.sh 19.2.3,notest --cmtconfig x86_64-slc6-gcc47-opt 
    Sim_tf.py --inputEVNTFile=EVNT.0$dsid._000001.pool.root.1 --AMITag=s2608 --DBRelease="default:current" --DataRunNumber=222525 --conditionsTag "default:OFLCOND-RUN12-SDR-19" --firstEvent=1 --geometryVersion="default:ATLAS-R2-2015-03-01-00_VALIDATION" --maxEvents=500 --outputHITSFile=HITS.0$dsid._000001.pool.root.1 --physicsList=FTFP_BERT --postInclude "default:PyJobTransforms/UseFrontier.py" --preInclude "EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py" --randomSeed=1 --runNumber=$dsid --simulator=MC12G4 --skipEvents=0 --truthStrategy=MC12
    popd
}

recofull(){
    echo '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GDC:RECONSTRUTING ++++++++++++++++++++++++++++++++++++++++++++++'
    mkdir -p MCProd/$filenameAOD
    cp MCProd/$filenameHITS/HITS.0$dsid._000001.pool.root.1 MCProd/$filenameAOD/
    pushd MCProd/$filenameAOD/
    unset CMTPATH;
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/AtlasProduction/20.7.5.1/AtlasProductionRunTime/cmt/setup.sh
    source /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/cmtsite/asetup.sh AtlasProd1,20.7.5.1.1 --cmtconfig x86_64-slc6-gcc49-opt
    Reco_tf.py --inputHITSFile=HITS.0$dsid._000001.pool.root.1 --maxEvents=500  --postExec "all:CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"]" "ESDtoAOD:fixedAttrib=[s if \"CONTAINER_SPLITLEVEL = \'99\'\" not in s else \"\" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes]; svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib" --postInclude "default:RecJobTransforms/UseFrontier.py" --preExec "all:rec.Commissioning.set_Value_and_Lock(True); from AthenaCommon.BeamFlags import jobproperties; jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0); from LArROD.LArRODFlags import larRODFlags; larRODFlags.NumberOfCollisions.set_Value_and_Lock(20); larRODFlags.nSamples.set_Value_and_Lock(4); larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True); larRODFlags.firstSample.set_Value_and_Lock(0); larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)" "RAWtoESD:from CaloRec.CaloCellFlags import jobproperties; jobproperties.CaloCellFlags.doLArCellEmMisCalib=False" "ESDtoAOD:TriggerFlags.AODEDMSet=\"AODSLIM\"" --preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_v3.py" "RDOtoRDOTrigger:RecExPers/RecoOutputMetadataList_jobOptions.py" --skipEvents=0 --autoConfiguration=everything --conditionsTag "default:OFLCOND-MC15c-SDR-09" --geometryVersion="default:ATLAS-R2-2015-03-01-00" --runNumber=$dsid --digiSeedOffset1=4 --digiSeedOffset2=4 --digiSteeringConf='StandardSignalOnlyTruth' --AMITag=r7773 --steering="doRDO_TRIG" --inputHighPtMinbiasHitsFile=/data05/mc15_13TeV/mc15_13TeV.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2195/HITS.05608152._[000001,000002,000003,000006].pool.root --inputLowPtMinbiasHitsFile=/data05/mc15_13TeV/mc15_13TeV.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2195/HITS.05608147._[000005,000006,000007,000014].pool.root --numberOfCavernBkg=0 --numberOfHighPtMinBias=0.12268057 --numberOfLowPtMinBias=39.8773194 --pileupFinalBunch=6 --outputAODFile=AOD.0$dsid._000001.pool.root.1 --jobNumber=1 --triggerConfig="RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2046,20,56" --ignorePatterns=Py:TrigConf2COOLLib.py.+ERROR.===================================+
    popd
}

echo $dsid 
generate
truth
#hitsfast
#recofast
#hitsfull
#recofull
  

#dsid=344252 ; 
#for mH in `cat /tmp/listmasses_mH.txt` ; do for ma in `cat /tmp/listmasses_ma.txt                          ` ; do cp template MC15.$dsid.aMcAtNloPythia8EvtGen_A14NNPDF23NLO_HC_ggH$mH\_NWA_2a$ma\_4y.py   ; let dsid=dsid+1; done; done ; 
#for mH in `cat /tmp/listmasses_mH.txt` ; do for ma in `cat /tmp/listmasses_ma.txt | grep -v 01 | grep -v 03` ; do cp template MC15.$dsid.aMcAtNloPythia8EvtGen_A14NNPDF23NLO_HC_ggH$mH\_NWA_2a$ma\_6pi0.py ; let dsid=dsid+1; done; done ;

