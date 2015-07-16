#!/bin/sh
DIRP=`pwd`
ICSTMP=$2
URL=$1
cd "$ICSTMP"

if [ -z "$1" ]; then echo "putcal.sh: Keine URL angegeben"; exit; fi
if [ -z "$2" ]; then echo "putcal.sh: Kein tmpdir angegeben"; exit; fi

cadaver $URL << EOS
	`while read p; do
  	echo $p
	done < ./del`
	exit
EOS
cd $DIRP
exit
