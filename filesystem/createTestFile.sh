#!/bin/sh
mkdir TestFile
a=0;
size=1024*1024
while [ $a -lt 20 ]
do
	dd if=/dev/urandom of=./TestFile/$a".tmp" bs=$size count=1
    size=$(($size * 2))
	a=`expr $a + 1`
    echo $size
done



