#!/bin/bash

compare_outputs() {
    for flag in "pic" "tikz" "pst" "eps" "svg" "xml"; do
        if [ "$flag" = "tikz" ]; then
            ext="tkz"
        else
            ext="$flag"
        fi

        directory=${3%/*}
        filename=$(basename "$3")
        filename="${filename%.*}"

        FILE1="./1$filename.$ext"
        FILE2="./2$filename.$ext"

        "./$1" "$3" "-$flag" > /dev/null
        mv "$directory$filename.$ext" "$FILE1"

        "./$2" "$3" "-$flag" > /dev/null
        mv "$directory$filename.$ext" "$FILE2"

        difference=$(diff --suppress-common-lines "$FILE1" "$FILE2")

        if [ -n "$difference" ] ; then
            { echo "$3: $flag"; echo "$difference"; printf "\n"; } >> "./diff-log"
        fi

        rm "$FILE1" "$FILE2"
    done
}

if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters!"
    echo "Usage: ./compare_output.sh program1 program2 sample_directory"
    exit 1
fi

if ! command -v diff &> /dev/null
then
    echo "Program diff is not available"
    exit 1
fi

if ! [[ -x "$1" ]] ; then
    echo "File '$1' is not executable or found"
    exit 1
fi

if ! [[ -x "$2" ]] ; then
    echo "File '$2' is not executable or found"
    exit 1
fi

find "$3" -type f -name "*.gcl" | while read -r file; do
    compare_outputs "$1" "$2" "$file"
done