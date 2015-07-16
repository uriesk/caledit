#!/bin/sh
DIRP=`pwd`
ICSTMP=$2
URL=$1
cd "$ICSTMP"

if [ -z "$1" ]; then echo "getcal.sh: Keine URL angegeben"; exit; fi
if [ -z "$2" ]; then echo "getcal.sh: Kein tmpdir angegeben"; exit; fi

mkdir ics
cadaver $URL << EOC | grep .ics | sed 's/.ics.*/.ics/' | sed 's/\ *//' > ./filelist
	ls
	exit
EOC

cd ics
cadaver $URL << EOA
	`while read p; do
  	echo "get $p"
	done < ../filelist`
	exit
EOA
cd $DIRP
exit
