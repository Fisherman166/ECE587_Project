#!/bin/tcsh

set skip_instr=100000000
set run_instr=20000000

./Run.pl -db bench.db -dir results/gcc1 -benchmark gcc -sim /mnt/Storage/Windows/Sean/Documents/Documents/School/Fall_2016/ECE587/project/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr" >& results/gcc1.out

./Run.pl -db bench.db -dir results/gcc1 -benchmark gcc -sim /mnt/Storage/Windows/Sean/Documents/Documents/School/Fall_2016/ECE587/project/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr -cache:dl1 dl1:128:64:4:s" >& results/score.out
./Run.pl -db bench.db -dir results/gcc1 -benchmark gcc -sim /mnt/Storage/Windows/Sean/Documents/Documents/School/Fall_2016/ECE587/project/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr -cache:dl1 dl1:128:64:4:p:3:2" >& results/pLRU.out
