#!/bin/bash

rm book
rm *.o

if [ -n "$1" ] && [ -n "$2" ] && [ -n "$3" ]; then
    cd 'test'
    ./test_gen.rb categories=$1 users=$2 orders=$3
    cd ..
    make
    ./book test/rand_test/db.txt test/rand_test/orders.txt test/rand_test/cat.txt
    make clean
else
    echo 'No parameters given, running test provided by professor.'
    make
    ./book test/official_test/db.txt test/official_test/orders.txt test/official_test/cat.txt
    make clean
fi
