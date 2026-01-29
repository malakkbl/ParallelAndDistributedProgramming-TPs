#!/bin/bash

HPL_DIR="../testing"
OUTFILE="../results/hpl_results.csv"

Ns=(1000 5000 10000 20000)
NBs=(1 2 4 8 16 32 64 128 256)

echo "N,NB,time_s,gflops,status" > $OUTFILE

cd $HPL_DIR || exit 1

for N in "${Ns[@]}"; do
  for NB in "${NBs[@]}"; do

    echo "Running HPL with N=$N NB=$NB"

    # Generate clean HPL.dat
    cat > HPL.dat << EOF
HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
HPL.out
6
1
$N
1
$NB
0
1
1
1
16.0
1
2
1
4
1
2
1
1
1
0
1
0
2
64
0
0
1
8
EOF

    # Run HPL
    ./xhpl > hpl_tmp.out

    # Extract result line
    LINE=$(grep '^WR' hpl_tmp.out | tail -n 1)

    if [ -z "$LINE" ]; then
      echo "$N,$NB,NA,NA,FAILED" >> $OUTFILE
      continue
    fi

    TIME=$(echo "$LINE" | awk '{print $6}')
    GFLOPS=$(echo "$LINE" | awk '{print $7}')

    if grep -q PASSED hpl_tmp.out; then
      STATUS="PASSED"
    else
      STATUS="FAILED"
    fi

    echo "$N,$NB,$TIME,$GFLOPS,$STATUS" >> $OUTFILE

    rm -f hpl_tmp.out

  done
done

cd - > /dev/null
