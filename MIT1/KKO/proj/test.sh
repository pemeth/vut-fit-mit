#!/usr/bin/bash

runthis(){
    ## print the command to the logfile
    echo "$@" >> "$stats"
    ## run the command and redirect it's error output
    ## to the logfile
    eval "$@" 2>> "$stats"
}

raw_dir="data/"
test_dir="test/"
stats="test/stats"

mkdir -p "$test_dir"
touch "$stats"
printf "" > "$stats"

declare -a options=( "" "-m" "-a" "-m -a" )

for opt in "${options[@]}"
do
    printf "========================================\n" >> "$stats"
    printf "\tWith options: %s \n" "$opt" >> "$stats"
    printf "========================================\n" >> "$stats"
    for raw_file in "$raw_dir"*.raw
    do
        # Filenames with paths for encoded and decoded images
        enc_file=$(basename "$raw_file")
        enc_file="$test_dir${enc_file%.*}"$opt".enc"
        dec_file=$(basename "$raw_file")
        dec_file="$test_dir${dec_file%.*}"$opt".dec"

        # Hacked for option combination `-m -a`, because it didn't want to work
        if [[ "$opt" == "-m -a" ]]; then
            enc_file=$(basename "$raw_file")
            enc_file="$test_dir${enc_file%.*}-m-a.enc"
            dec_file=$(basename "$raw_file")
            dec_file="$test_dir${dec_file%.*}-m-a.dec"
        fi

        # Print and run compression and then decompression
        printf "\nCOMPRESSING '%s' into '%s'\n\n" "$raw_file" "$enc_file" >> "$stats"
        runthis "{ time ./huff_codec -c -w 512 -i "$raw_file" -o "$enc_file" "$opt" ; }"

        printf "\nDECOMPRESSING '%s' into '%s'\n\n" "$enc_file" "$dec_file" >> "$stats"
        runthis "{ time ./huff_codec -d -w 512 -i "$enc_file" -o "$dec_file" "$opt" ; }"

        # RAW image vs compressed image sizes + compression factor
        orig_size=$(stat -c%s "$raw_file")
        enc_size=$(stat -c%s "$enc_file")
        factor=`echo "scale=4; $orig_size / $enc_size" | bc`
        #factor=$((orig_size / enc_size))
        printf "\n" >> "$stats"
        printf "Size RAW:\t\t%s bytes\n" "$orig_size" >> "$stats"
        printf "Size encoded:\t%s bytes\n" "$enc_size" >> "$stats"
        printf "Compression factor: %f\n" "$factor" >> "$stats"

        # Are the original image and decompressed image the same?
        printf "\n" >> "$stats"
        diff -s "$raw_file" "$dec_file" >> "$stats"

        printf '\n//////////////////////////////////////////////////////////////////////\n' >> "$stats"
    done
done