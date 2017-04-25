#!/bin/bash

for mx in `cat /data01/montoya/workarea/HGamma/HighMass/HGamAnalysisFramework/Run/listofsamples_all.txt | grep 4y | sed 's|ggH||g' | awk -F'_' '{print $2}'| sort -n | uniq`
do
    for ma in `cat /data01/montoya/workarea/HGamma/HighMass/HGamAnalysisFramework/Run/listofsamples_all.txt | grep 4y | sed 's|2a||g' | awk -F'_' '{print $4}' | grep -v '03'| sed 's|0|0.|g' | sort -n | uniq `
    do 
	maR=`echo $ma | sed 's|\.||g'`
	eff=`cat /data01/montoya/workarea/HGamma/HighMass/HGamAnalysisFramework/Run/cutflows/cutflow_aMCnloPy8_ggH$mx\_NWA_2a$maR\_4y.txt | grep CutFAIL | awk '{print $10}' | sed 's|\\\%||g'`
	let mxR=mx/100
	effR=`echo $eff/100| bc -l`
        echo $mxR $ma $effR
       #echo $mx $ma $effR 
	done
done > failraw.dat


exit
gnuplot
A(x,y) = a*(1/(1+b*exp(-c*x))) * e*(1/(1+e*exp(f*y)))


A(x,y) = a*(1/(1+b*exp(-c*x))) * (1/(1+d*exp(e*y))) 
a=0.6;b=35;c=0.7;d=10;e=0.1;
#fit A(x,y) 'pass.dat' via a,b,c,d,e
a=2.71063e+00
b=9.16852e+00
c=2.90043e-03
d=1.72105e-01
e=3.03896e-01
splot 'pass.dat', A(x,y)





F(x) = a/(1+b*exp(-c*x)) 
a=1;b=1;c=1;
fit F(x) 'pass07.dat' using 1:3 via a,b,c
plot 'pass07.dat' using 1:3, F(x)



G(x) = a*exp(-d*x) + e
d=1;e=1;
fit G(x) 'passtmp2.dat' using 2:3 via a,d,e
plot 'passtmp2.dat' using 2:3, G(x)






