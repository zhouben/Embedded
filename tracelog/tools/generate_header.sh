#!/bin/bash

echo "#ifndef _C_FILES_TBBLE_H_"
echo "#define _C_FILES_TBBLE_H_"
echo ""
i=0
echo "CFILES_DESC cfiles_table[]="
echo "{"
while read name
do
    echo "    {\"$name\", $i},"
    i=$(($i+1))
done
echo "};"

echo ""
echo "#endif"
