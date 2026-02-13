# Parallel and Distributed Programming - TPs

**Done by:** Kably Malak

This repository contains all my lab assignments (TPs) for the **Parallel and Distributed Programming** course. Each TP is organized in its own directory with exercises, code implementations, analysis, results, and its report.

## Overall Structure

```
ParallelAndDistributedProgramming-TPs/
├── README.md
├── TP1/
├── TP2/
├── TP3/
└── ...
```

---

## TP1 - Optimizing Memory Access

### Structure
```
TP1/
├── TP1.pdf                     # TP assignment instructions
├── TP1Report.pdf               # Final report
├── Ex1/                        # Cache performance analysis
│   ├── ex1.c
│   ├── analysis.ipynb
│   ├── results_O0.txt
│   ├── results_O2.txt
│   └── output.png
├── Ex2/                        # Matrix multiplication algorithms (ijk vs ikj)
│   ├── mxm_ijk.c
│   ├── mxm_ikj.c
│   ├── result_ijk_O0.txt
│   ├── result_ijk_O2.txt
│   ├── result_ikj_O0.txt
│   └── result_ikj_O2.txt
├── Ex3/                        # Block matrix multiplication
│   ├── mxm_bloc.c
│   ├── result_block_8.txt
│   ├── result_block_16.txt
│   ├── result_block_32.txt
│   ├── result_block_64.txt
│   ├── result_block_128.txt
│   └── CMDs.png
├── Ex4/                        # Memory debugging with Valgrind
│   ├── memory_debug.c
│   ├── output_normal.txt
│   ├── valgrind_before_fix.txt
│   ├── valgrind_after_fix.txt
│   ├── BeforeFix.png
│   ├── AfterFix.png
│   └── Screen1.png
└── Ex5/                        # HPL benchmark analysis
    ├── run_hpl_all.sh
    ├── hpl_results.csv
    ├── vis.ipynb
    ├── hpl_gflops_vs_nb.png
    ├── hpl_time_vs_nb.png
    └── hpl_efficiency_vs_nb.png
```

---

## TP2 - Foundations of Parallel Computing

### Structure
```
TP2/
├── TP2.pdf                     # TP assignment instructions
├── TP2Report.pdf               # Final report
├── plots.ipynb                 # Jupyter notebook for data visualization
├── Ex1/                        # Loop optimizations and unrolling
│   ├── double_unroll_param.c
│   ├── float_unroll_param.c
│   ├── int_unroll_param.c
│   ├── short_unroll_param.c
│   ├── results_O0.txt
│   ├── results_O2.txt
│   ├── results_float_O0.txt
│   ├── results_float_O2.txt
│   ├── results_int_O0.txt
│   ├── results_int_O2.txt
│   ├── results_short_O0.txt
│   ├── results_short_O2.txt
│   ├── cmds.txt
│   ├── NoUnrollingCMDs.png
│   ├── MyBandwidth.png
│   ├── Plot1.png
│   └── Plot2.png
├── Ex2/                        # Instruction scheduling
│   ├── exercise2.c
│   ├── exercise2_manual.c
│   ├── O0.s
│   ├── O2.s
│   ├── loop_O0.txt
│   ├── loop_O2.txt
│   ├── labels_O0.txt
│   ├── time_O0.txt
│   ├── time_O2.txt
│   └── time_manual_O0.txt
└── Ex3/                        # Profiling and scalability analysis
    ├── exercise3.c
    ├── callgrind_report_N5e6.txt
    ├── callgrind_report_N1e7.txt
    ├── callgrind_report_N1e8.txt
    ├── amdahl_results.txt
    ├── gustafson_results.txt
    ├── amdahl_scaling.png
    ├── gustafson_scaling.png
    └── plot_ex3.py
```

---

## TP3 - OpenMP Parallel Programming

### Structure
```
TP3/
├── TP3.pdf                     # TP assignment instructions
├── TP3Report.pdf               # Final report
├── ex1/                        
│   ├── ex1.c
│   ├── outputs/
│   │   ├── output_1thread.txt
│   │   ├── output_2threads.txt
│   │   ├── output_4threads.txt
│   │   └── output_8threads.txt
│   └── terminal_screenshot.png
├── ex2/                        
│   ├── ex2_sequential.c
│   ├── ex2_parallel.c
│   ├── output_sequential.txt
│   ├── output_parallel.txt
│   └── terminal_screenshot.png
├── ex3/                        
│   ├── serial.c
│   ├── parallel1line.c
│   ├── serial.txt
│   ├── parallel1line.txt
│   └── terminal_screenshot.png
├── ex4/                        
│   ├── serial.c
│   ├── parallel.c
│   ├── matrix_threads.c
│   ├── matrix_scheduling.c
│   ├── serial.txt
│   ├── parallel.txt
│   ├── thread_results.txt
│   ├── scheduling_results.txt
│   ├── analysis.ipynb
│   ├── speedup_plot.png
│   ├── efficiency_plot.png
│   ├── scheduling_comparison.png
│   ├── combined_analysis.png
│   ├── thread_execution.png
│   └── scheduling_execution.png
└── ex5/                        
    ├── jacobi_parallel.c
    ├── results.csv
    ├── analysis.ipynb
    ├── jacobi_time.png
    ├── jacobi_speedup.png
    ├── jacobi_efficiency.png
    └── jacobi_combined.png
```

## Future TPs

Additional TPs will be added throughout the semester as they are assigned.