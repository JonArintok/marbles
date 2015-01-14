#!/bin/bash

if [ -n "$1" ]; then
	filename=$1
else
	filename="perfTestOut/$USER@$HOSTNAME"
fi

runAndPrint() {
	>&2 cat /proc/version
	>&2 lscpu
	
	echo "cgol.mrbl"
	>&2 echo "\n\n\n\ncgol.mrbl"
	>&2 perf stat ./marblerun test/cgol.mrbl
	
	echo "bouncingColors.mrbl"
	>&2 echo "\n\n\n\nbouncingColors.mrbl"
	>&2 perf stat ./marblerun test/bouncingColors.mrbl
	
	echo "fibonacciX16.mrbl"
	>&2 echo "\n\n\n\nfibonacciX16.mrbl"
	>&2 perf stat ./marblerun test/fibonacciX16.mrbl
}

printf "writing to $filename...\n"
runAndPrint 2> $filename

exit 0
