# -*- mode: makefile -*-
#
# Makefile containing platform dependencies different projects.
MAKEFLAGS = --no-print-directory -r -s -j2


ARCH_LOC_1 := $(wildcard $(shell root-config --prefix)/test/Makefile.arch)
ARCH_LOC_2 := $(wildcard $(shell root-config --prefix)/etc/Makefile.arch)
ARCH_LOC_3 := $(wildcard $(shell root-config --prefix)/share/doc/root/test/Makefile.arch)
ifneq ($(strip $(ARCH_LOC_1)),)
  $(info Using $(ARCH_LOC_1))
  include $(ARCH_LOC_1)
else
  ifneq ($(strip $(ARCH_LOC_2)),)
    $(info Using $(ARCH_LOC_2))
    include $(ARCH_LOC_2)
  else
    ifneq ($(strip $(ARCH_LOC_3)),)
      $(info Using $(ARCH_LOC_3))
      include $(ARCH_LOC_3)
    else
      $(error Could not find Makefile.arch!)
    endif
  endif
endif


CXXFLAGS += -Wall -Wno-overloaded-virtual -Wno-unused

INCLUDES += -I./inc -I/users/yanght/include 

VPATH	= ./src ./inc

OBJECTS_Asymptotic	= obj/Asymptotic.o obj/CommonFunc.o obj/CommonFuncDict.o
OBJECTS_MakeAsimovData	= obj/MakeAsimovData.o obj/CommonFunc.o obj/CommonFuncDict.o
OBJECTS_Submit		= obj/submit.o obj/CommonFunc.o obj/CommonFuncDict.o
OBJECTS_Frame		= obj/frame.o obj/CommonFunc.o obj/CommonFuncDict.o obj/MyParticle.o obj/MyParticleDict.o
OBJECTS_Workspace	= obj/workspace.o obj/CommonFunc.o obj/CommonFuncDict.o obj/genXML.o obj/genXMLDict.o
OBJECTS_Threshold	= obj/threshold.o obj/Limit.o obj/LimitDict.o obj/CommonFunc.o obj/CommonFuncDict.o
OBJECTS_Limit		= obj/limit.o obj/Limit.o obj/LimitDict.o obj/CommonFunc.o obj/CommonFuncDict.o
OBJECTS_analysis 	= obj/analysis.o obj/Hgg_tree.o obj/MyParticle.o obj/MyParticleDict.o

GLIBS	+= -lTMVA -lMLP
#GLIBS 	+= -L/users/yanght/lib  #-lCommonFunc #-lLimit
GLIBS	+= -lTreePlayer -lProof -lProofPlayer -lutil -lRooFit -lRooFitCore  -lRooStats -lFoam -lMinuit -lHistFactory -lXMLParser -lXMLIO -lCore -lGpad #-lMathCore 
.PHONY:
	$(OBJECTS)

MakeAsimovData: $(OBJECTS_MakeAsimovData)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) -O2 $^ $(ROOTLIBS) $(GLIBS) -o $@

Asymptotic: 	$(OBJECTS_Asymptotic)
		@echo "Linking " $@ "from " $(OBJECTS_Asymptotic)
		@$(LD) $(LDFLAGS) -O2 $^ $(ROOTLIBS) $(GLIBS) -o $@

Submit:		$(OBJECTS_Submit)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) -O2 $^ $(GLIBS) -o $@

Frame:		$(OBJECTS_Frame)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt) $@

Threshold: 	$(OBJECTS_Threshold)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) -O2 $^ $(ROOTLIBS) $(GLIBS) -o $@

Limit: 		$(OBJECTS_Limit)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) -O2 $^ $(ROOTLIBS) $(GLIBS) -o $@

Workspace:	$(OBJECTS_Workspace)
		@echo "Linking " $@
		@$(LD) $(LDFLAGS) -O2 $^ $(GLIBS) -o $@

analysis: 	$(OBJECTS_analysis)
		@echo "Linking " $@
#		echo $(LD) $(LDFLAGS) $^ $(GLIBS) -o $@
		@$(LD) $(LDFLAGS) $^ $(GLIBS) -o $@

obj/%.o : %.cxx
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.h %.cxx
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.cc
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.h %.cc
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.hh %.cc
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.C
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%.o : %.h %.C
	@echo "Compiling $@"
	@$(CXX) $(CXXFLAGS) -O2 -c $< -o $@ $(INCLUDES)

obj/%Dict.o:
		@echo "Compiling $@"
#		echo rootcint -f $*Dict.cxx -c $*.h $*LinkDef.h
		rootcint -f $*Dict.cxx -c inc/$*.h inc/$*LinkDef.h
#		echo $(CXX) $(INCLUDES) $(CXXFLAGS) -c $*Dict.cxx -o $*Dict.o
		$(CXX) $(INCLUDES) $(CXXFLAGS) -c $*Dict.cxx -o obj/$*Dict.o
		rm -f $*Dict.cxx $*Dict.h 

clean:
	@echo "Cleaning $<..."
	rm -fr *~ obj/*.o */*~ *_Dict.* *.a 
	@echo "Done"