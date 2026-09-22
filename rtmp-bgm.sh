#!/bin/bash

if [ -z "$RTMP_BGM" ]; then
    echo "\$RTMP_BGM is not set or is empty."
    exit
else
    echo "Streaming to: $RTMP_BGM"
fi

bgm_list=$(mktemp /tmp/bgmlist.XXXXXXXX)

while true; do
    echo "Loop starting..."; echo
	find -type f | grep -E '\.(mp3|m4a)$' | sed 's/^\.\///' | shuf > $bgm_list
	cnt=$(wc -l < $bgm_list)

	for ((i=1; i<=$cnt; i++)); do
		filepath=$(sed -n "$i{p;q}" $bgm_list)
		filename="${filepath##*/}"

        echo "===== Playing: $filename ====="
        if [ ! -e "$filepath" ]; then
            echo "$filepath does not exist, continuing to the next file."; echo
            continue
        fi

        songname=${filename%.*}
        echo "🎵 $songname" > $XDG_RUNTIME_DIR/current-bgm.txt

        ffmpeg -re -i "$filepath" -c:v copy -c:a aac -ar 48000 -f flv $RTMP_BGM

        echo; echo "Waiting for next song... (Press 'q' to quit)"
        read -t 5 -n 1 input

        if [[ $input == 'q' ]]; then
            echo "Exiting..."
			rm -f $bgm_list
            exit
        fi

        echo
    done
done
