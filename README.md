# Parallel and Distributed Programming - TPs

**Done by:** Kably Malak

This repository contains all my lab assignments (TPs) for the **Parallel and Distributed Programming** course. Each TP is organized in its own directory with exercises, code implementations, analysis, results, and its report.

## Overall Structure

```
ParallelAndDistributedProgramming-TPs/
├── README.md
├── TP1/
└── TP2/ (to be added)
└── TP3/ (to be added)
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

## Future TPs

Additional TPs will be added throughout the semester as they are assigned.