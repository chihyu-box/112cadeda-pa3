#!/bin/bash

rm result.log
rm -rf result
mkdir -p result

for file in two_level_blif/*.blif; do
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    
    echo $filename >> result.log
    ./bin/M11207432 "two_level_blif/${filename}.blif" >> result.log

    mv out.blif "result/${filename}.blif"
done
