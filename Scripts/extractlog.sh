#!/bin/bash
ma=$1
shift 1

echo "void inputnumbers(){"> inputnumbers.C 
echo " Int_t i=0; ">> inputnumbers.C 
 
for i in `seq 200 10 1500`; 
do 
    echo -n " FMSA[i]"=$i
    echo -n "; upper_limit_data[i]="`tail outputs/log_$i\_$ma.log -n8 | grep observ  | awk '{print $3}'`
    echo -n "; upper_limit[i]="`tail outputs/log_$i\_$ma.log -n8 | grep median  | awk '{print $4}'`
    echo -n "; m1_sigma_down[i]="`tail outputs/log_$i\_$ma.log -n8 | grep '\-1 s' | awk '{print $5}'`
    echo -n "; m1_sigma_up[i]="`tail outputs/log_$i\_$ma.log -n8 | grep '+1 s'  | awk '{print $5}'`
    echo -n "; m2_sigma_down[i]="`tail outputs/log_$i\_$ma.log -n8 | grep '\-1 s' | awk '{print $5}'`
    echo -n "; m2_sigma_up[i]="`tail outputs/log_$i\_$ma.log -n8 | grep '+2 s'  | awk '{print $5}'`
    echo "; i++;"
done | grep -v '\[i\]=;' | grep -v '\[i\]=0;' >> inputnumbers.C 

echo " tot=i;" >> inputnumbers.C
echo "}">> inputnumbers.C 

root -l -q -b "plotlimit.C(\"$ma\")"

display outputs/limitplot_ma`echo $ma| sed 's|\.|_|g'`GeV.png &

