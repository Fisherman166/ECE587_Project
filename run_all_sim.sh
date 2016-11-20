#!/bin/tcsh

# Generic
set sim_dir=/u/santiagl/ECE587_Project
set skip_instr=100000000
set run_instr=20000000
set file=results

# Cache specific
set cache_type=dl1
set benchmark_list=( gcc li )
set line_list=( 512 256 128 64 )
set line_sz_list= ( 32 64 )
set ways_list=( 2 4 8 16 )
# Clear Results file 
echo "Benchmark Logs: " > ${file}.log

foreach b ($benchmark_list) # iterate over benchmarks list
set benchmark=$b
echo "%%%%%%%%%%%%%   Benchmark: " $benchmark  "%%%%%%%%%%%%%%">> ${file}.log  # add benchmark name to result file 
    foreach l ($line_list)
        foreach lw ($line_sz_list)
            foreach w ($ways_list)
            set cache_conf=${cache_type}:${l}:${lw}:${w}
            ./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr -cache:dl1 ${cache_conf}:l" >& results/lru.out
            #./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr -cache:dl1 dl1:128:64:4:s" >& results/score.out
            #./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder -args "-fastfwd $skip_instr -max:inst $run_instr -cache:dl1 dl1:128:64:4:p:3:1" >& results/pLRU.out
            echo "*****************    ${cache_conf}     **********************************" >> ${file}.log
grep IPC results/*.out | column -t >> ${file}.log


end # ways 

end # line size 

end # number of line list  

end # end benchmark 

rm results/*.out
 echo "Done" 

