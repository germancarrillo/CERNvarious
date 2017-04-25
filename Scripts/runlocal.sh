#!/bin/bash

NTOYS=100

cat <<EOF > /data01/montoya/workarea/Xaa4y/Stats/tmp.sh
#!/bin/bash
mu=\$1
ma=\$2
NTOYS=\$3
shift 3
source /data01/montoya/workarea/configuration/rootenv64b.bash
pushd /data01/montoya/workarea/Xaa4y/Stats/
for i in \`seq 1 1 \$NTOYS\`; do root -l -q -b "PLL.C(\$mu,\$ma,\$RANDOM)"; done
popd
EOF
chmod 777 /data01/montoya/workarea/Xaa4y/Stats/tmp.sh

#source /data01/montoya/workarea/configuration/rootenv64b.bash
#echo '{gROOT->LoadMacro("PLL.C++");}' > /tmp/recompile.C
#root -l -q -b  /tmp/recompile.C

for mu in `seq 0.5 0.5 1.5`
do
    for ma in `seq 0.6 0.2 1.2`
    do
	echo "submitting mu=$mu and ma=$ma with NTOYS=100x$NTOYS"
       #nohup ssh pcwits11 "source /data01/montoya/workarea/Xaa4y/Stats/tmp.sh $ma $NTOYS" &> log_$ma.log & 
	nohup sh /data01/montoya/workarea/Xaa4y/Stats/tmp.sh $mu $ma $NTOYS &> log_$mu\_$ma.log & 
	echo "-----------------------------------"
    done
done
rm /data01/montoya/workarea/Xaa4y/Stats/tmp.sh #/tmp/recompile.C

#for mu in `seq 1 0.1 4`; do for i in `seq 1 1 20`; do root -l -q -b "PLL.C+($mu,1,$RANDOM)" > outputs/log_$mu\_$i.log; done ; done