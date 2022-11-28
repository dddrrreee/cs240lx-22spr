#!/bin/sh

make -B

if [ "$1" = "basic_vs_matthew" ] ; then
    in_file=../test_inputs/p7.dimacs
    cat $in_file | (./basic xcheck 2> xcheck_basic.output)
    diff xcheck_basic.output ../matthew_output/xcheck_p7.output
fi

if [ "$1" = "basic_vs_fast" ] ; then
    in_file=../test_inputs/p9.dimacs
    cat $in_file | (./basic xcheck 2> xcheck_basic.output)
    cat $in_file | (./fast xcheck 2> xcheck_fast.output)
    diff xcheck_basic.output xcheck_fast.output
fi
