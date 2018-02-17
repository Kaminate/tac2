#!/bin/bash
# git bash doesn't work btw

clear

if [ $# -eq 0 ]
then
  echo "You need to supply your name"
  echo "Example command line: ./codeLineCount.sh \"MyFirstName\""
  exit 1;
fi

myname=$1 # first argument
lineTotal=0


for f in ../src/* # <-- todo: recursive
do
  # -w whole word
  # -i ignore case
  # -q quite
  if grep -w -i -q $myname $f; 
  then
    # code if found
    echo "[x] $f" 
    fileLineCount="$(wc "$f" | awk {'print $1'})"
    #echo "this line " $fileLineCount
    lineTotal=$(($lineTotal + $fileLineCount))
    #echo "total " $lineTotal
  else
    # code if not found
    echo "[ ] $f"
  fi
done

echo "Total lines of code: $lineTotal"

cd 
#ls
