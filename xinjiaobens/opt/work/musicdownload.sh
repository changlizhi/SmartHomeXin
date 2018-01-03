#!/bin/bash
# downfilelist.sh
if [ ! -d /tmp/mounts/SD-P1/tmp ]; then
	mkdir /tmp/mounts/SD-P1/tmp
fi
if [ ! -d /tmp/mounts/SD-P1/play ]; then
	mkdir /tmp/mounts/SD-P1/play
fi
url=$1
cd /tmp/mounts/SD-P1/tmp
if [ -z "$url"];then
	echo "$url"
else	
	rm /tmp/mounts/SD-P1/tmp/url
	echo "$url">/tmp/mounts/SD-P1/tmp/url
	rm music.zip
fi

if [ -z "$url" ] && [ -f /tmp/mounts/SD-P1/tmp/music.zip ]; then
	url=`cat /tmp/mounts/SD-P1/tmp/url`
	echo "continue to download... $url....."
fi

if [ -z "$url"];then
	echo "the url is null "
	return
fi
echo "try to download... $url....."
state="0"
while [ "${state}" -eq 0 ]
do
	info=`wget -t 0 -T 10  -c -O music.zip $url 2>&1`
	echo $info
	case $info in
		*timed*)
			echo "download failed"
			state="0"
			;;
		*bad*)
			echo "bad address"
			state="0"
			rm /tmp/mounts/SD-P1/tmp/url
			break;
			;;
		*100%*)
			echo "download succc"
			mv music.zip /tmp/mounts/SD-P1/music.zip
			pwd=`uci -c/opt/ft get ftconfig.@ftconfig[0].musicpwd`
			echo "passwd:$pwd"

			unzip -P $pwd -o /tmp/mounts/SD-P1/music.zip -d /tmp/mounts/SD-P1/play
			state="1"
			rm /tmp/mounts/SD-P1/tmp/url
			break
			;;
	esac
done


echo "download.finshed......."
