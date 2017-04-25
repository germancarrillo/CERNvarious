#!/bin/bash
ma=$1
shift 1
source /data01/montoya/workarea/HGamma/Stats/xmlAnaWSBuilder/setup.sh
cat << EOF > compile.C
{
 gROOT->ProcessLine(".L limit.C+"); 
}
EOF

root -l -q -b compile.C

cat << EOF > execute.sh
#!/bin/bash
mx=\$1
ma=\$2
shift 2
source /data01/montoya/workarea/HGamma/Stats/xmlAnaWSBuilder/setup.sh
npoints=30
poimin=0
poimax=\`echo 226.239*e\(-0.00779834*\$mx\) +  2.44177 + 5 | bc -l\`  # 3.2fb-1
root -l -q -b "limit.C+(\"Comb_v01.root\", \$mx, \$ma, \$npoints, \$poimin, \$poimax )" &> outputs/log_\$mx\_\$ma.log 
EOF

seq 200 10 2000 | parallel source execute.sh {} $ma

rm compile.C execute.sh

#to execute: for i in `echo 0.1 0.2 0.3 0.5 0.7 1.0 1.5 2.0 3.0 5.0 7.5 10.0`; do source runscan.sh $i ; done
a               = 226.239          +/- 25.07        (11.08%)
b               = -0.00779834      +/- 0.000571     (7.323%)
c               = 2.44177          +/- 0.4661       (19.09%)
