#!/bin/bash

# save the command
app=$1
num=$2

case $num in
1)
echo " ---- test 1 ----"
$app aa bb cc dd
echo "----------------------------------------------------------"
;;
2)
echo " ---- test 2 ----"
$app aa
echo "----------------------------------------------------------"
;;
3)
echo " ---- test 3 ----"
timeout 5 $app nonexistence orders.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
4)
echo " ---- test 4 ----"
timeout 5 $app database.txt nonexistence categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
5)
echo " ---- test 5 ----"
timeout 5 $app database.txt orders.txt nonexistence
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
6)
echo " ---- test 6 ---- bad user ID"
timeout 5 $app database.txt orders6.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
7)
echo " ---- test 7 ---- 1 user, 1 order, in balance"
timeout 15 $app database8.txt orders8.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
8)
echo " ---- test 8 ---- 1 user, 1 order, out of balance"
timeout 15 $app database9.txt orders9.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
9)
echo " ---- test 9 ---- 1 user, many orders"
timeout 15 $app database8.txt orders10.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
10)
echo " ---- test 10 ---- 1 user, many orders, many cats"
timeout 15 $app database11.txt orders11.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
11)
echo " ---- test 11 ---- 1 user, few orders, 2 cats"
timeout 15 $app database11.txt orders12.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
12)
echo " ---- test 12 ---- 1 user, many orders, 1 cat"
timeout 15 $app database11.txt orders13.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
13)
echo " ---- test 13 ---- empty files except database"
timeout 5 $app database.txt orders14.txt blankcategories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
14)
echo " ---- test 14 ---- general test"
timeout 15 $app database15.txt orders15.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
15)
echo " ---- test 15 ---- large orders"
timeout 15 $app database16.txt orders16.txt categories.txt
if [ $? -eq 124 ]; then
    echo "timeout"
fi
echo "----------------------------------------------------------"
;;
*)
echo "wrong number"
;;
esac

echo "----------------------------------------------------------"
echo " --- Finished --- "

exit 0
