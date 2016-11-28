#!/bin/tcsh

# Generic
set sim_dir=/u/santiagl/ECE587_Project
set skip_instr=100000000
set run_instr=20000000
# name for reslults and folder location 
# Note: Create folder first
set file=dl2_Baseline
set res_folder=dl2_Baseline
# Cache specific
# Change values here 
set cache_type=dl2
set mem_Lat=150
set mem_rest_Lat=10
set benchmark_list=( go gcc li ijpeg perl )
set set_list=( 4 )      
set line_sz_list= ( 64 )
set assoc_list=( 32 16 8 )  # for PLRU and Score not doing 2 it is pointless 
# No much to change beyond this point 

# Clear Results file 
echo "Benchmark Logs: " > ${file}.log
echo "Mem Latency:" $mem_Lat >> ${file}.log
echo "Mem Latency:" $mem_rest_Lat >> ${file}.log

#execution of benchmarks   
foreach b ($benchmark_list) # iterate over benchmarks list
set benchmark=$b
echo "%%%%%%%%%%%%%   Benchmark: " $benchmark  "%%%%%%%%%%%%%%">> ${file}.log  # add benchmark name to result file 
echo "Running" $benchmark
    foreach s ($set_list)
        foreach lw ($line_sz_list)
            foreach a ($assoc_list)
            set cache_conf=${cache_type}:${s}:${lw}:${a}
            set cache_name=${cache_type}_${s}_${lw}_${a}
            
             # Run LRU
           ./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder \
            -args "-fastfwd $skip_instr -max:inst $run_instr -cache:${cache_type} ${cache_conf}:l -mem:lat ${mem_Lat} ${mem_rest_Lat}" >& results/${res_folder}/${benchmark}_LRU_${cache_name}.out
            
             # Runs Score 
           ./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder \
            -args "-fastfwd $skip_instr -max:inst $run_instr -cache:${cache_type} ${cache_conf}:s -mem:lat ${mem_Lat} ${mem_rest_Lat}" >& results/${res_folder}/${benchmark}_SCORE_${cache_name}.out     
            
            # values for unique to PLRU 
            @ way2save=( ($a / 4) * 3 )
            @ max_score=( ($a / 2) + 1 )
            #RUNS PLRU 
           ./Run.pl -db bench.db -dir results/gcc1 -benchmark $benchmark -sim $sim_dir/src/sim-outorder \
            -args "-fastfwd $skip_instr -max:inst $run_instr -cache:${cache_type} ${cache_conf}:p:${max_score}:${way2save} -mem:lat ${mem_Lat} ${mem_rest_Lat}" >& results/${res_folder}/${benchmark}_PLRU_${cache_name}_p_${max_score}_${way2save}.out
          
            endif 
            echo "*****************    ${cache_conf}     **********************************" >> ${file}.log
          #  egrep 'IPC|dl2.misses' results/${res_folder}/${benchmark}_*_${cache_name}*.out | column -t >> ${file}.log


            end # ways 

        end # line size 

    end # number of set list  
    
echo "Finished" $benchmark
end # end benchmark 

 echo "Done" 
 

