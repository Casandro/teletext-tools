#!/bin/bash

IN="$1"
OUT="$2"


ffprobe $IN 2> /tmp/temp.ffprobe || exit
sid=`grep "0x" /tmp/temp.ffprobe | grep "Stream" | grep -v "Video" | grep -v "Audio" | grep -v "dvb_subtitle" | cut -d "[" -f 2 | cut -d "]" -f 1`
cat $IN |  $TS_TO_PES $sid | $ES_TO_PACKETS > $OUT


