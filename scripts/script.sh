#!/bin/bash
	
/opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gcc -dM -E - < /dev/null | sort | awk 'BEGIN{ fS=" "}{ for(i = 1; i <= NF; i++)    if(i == 1)  printf "%s ",$i; else if((i == 2) && (i == NF)) printf "%-40s\n",$i; else if(i == 2) printf "%-40s",$i; else if(i == NF) printf "%s\n",$i; else printf "%s ",$i}'