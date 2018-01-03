#!/bin/ash

echo "start install ........"
rm -rf /tmp/ftup*
system_config=/etc/config/system
cu_config=/opt/work/update.txt
http_addr=118.123.15.189
http_port=8055
scp_addr=xmpp.fantem.com
scp_port=9332

dst_packet_fn=puPacket.zip
depacket_dir=depack
new_ver_fn=version.txt
install_sh=install.sh


## Assamble the url for download the version file form remote server
ver_downurl="http://${http_addr}:${http_port}/upload/support/$new_ver_fn"


## Get the publish ID number which system had install 
if [ -f "$cu_config" ]; then 
	cu_pubid=`awk -F '=' '/pubid/{print $2}'  $cu_config`
	cu_pubid=`echo $cu_pubid|awk '{sub(/^[[:blank:]]*/,"",$0); print $0;}'`
else
	cu_pubid=0
fi 

##  Make temp dir 
up_tmp_dir='/tmp/ftup'${cu_pubid}'/'
echo "up_tmp_dir=$up_tmp_dir"

if [ ! -d $up_tmp_dir"/"$depacket_dir ]; then 
	mkdir -p $up_tmp_dir"/"$depacket_dir 
fi 

## Check the version.txt exist or not
if [ -f "${up_tmp_dir}${new_ver_fn}" ]; then 
	echo "find file ${up_tmp_dir}${new_ver_fn}  "
	cat ${up_tmp_dir}${new_ver_fn}
else
	echo "wget  $ver_downurl and save it to ${up_tmp_dir} "
	wget -T 5 -t 5 $ver_downurl -O ${up_tmp_dir}${new_ver_fn}
	if [ -f "${up_tmp_dir}${new_ver_fn}" ]; then 
		echo "${new_ver_fn} download to ${up_tmp_dir}"
		cat ${up_tmp_dir}${new_ver_fn}
	else
		echo "Can not download ${new_ver_fn}, update canneled... "
		echo "update_fail"
		exit 1
	fi
fi 


## Extract the New Publish ID and md5
new_pubid=`awk -F '= *' '/pubid/{print $2}'  ${up_tmp_dir}${new_ver_fn}`
new_pubid=`echo $new_pubid|awk '{sub(/^[[:blank:]]*/,"",$0); print $0;}'`
new_md5=`awk -F '=' '/md5/{print $2}'  ${up_tmp_dir}${new_ver_fn}`
new_md5=`echo $new_md5|awk '{sub(/^[[:blank:]]*/,"",$0); print $0;}'`

pub_fn=pub${new_pubid}.zip
pub_remote_path=download/pub${new_pubid}.zip

echo "cu_pubid="$cu_pubid
echo "new_pubid="$new_pubid
echo "new_md5="$new_md5 
echo "pub_remote_path="$pub_remote_path
echo "pub_name="$pub_fn


if [ "$new_pubid" -le "$cu_pubid" ]
then
	echo "/******   no new ver info  *******/"
	echo "update_fail"
	exit 1 
fi


aplay /tmp/mounts/SD-P1/voice/updatesoftstart.wav
## Assamble the url for download the version file form remote server
zip_downurl="http://${http_addr}:${http_port}/upload/support/download/$pub_fn"

wget -T 5 -t 5 $zip_downurl -O ${up_tmp_dir}${pub_fn}

# Count real_md5 
md5sum ${up_tmp_dir}${pub_fn} > ${up_tmp_dir}/md5.txt
rel_md5=`cat ${up_tmp_dir}/md5.txt`
echo "ttt: $rel_md5"
rel_md5=`echo $rel_md5|cut -b 1-32`
echo "ttt: $rel_md5"

# md5 compare
if [ "$rel_md5"x != "$new_md5"x ]; then
	echo "find file md5 error ....  "
	echo "new md5=$new_md5"
	echo "rel md5=$rel_md5"
	echo "update_fail"
	exit 1
fi
echo echo "new md5=${new_md5} is right!"

#for unzip the dst file
unzip -o ${up_tmp_dir}${pub_fn} -d ${up_tmp_dir}$depacket_dir

echo "finding ${up_tmp_dir}${depacket_dir}/${install_sh}"

if [ -f "${up_tmp_dir}${depacket_dir}/${install_sh}" ]; then 
	echo "Install script was found ... run it now.. "
	chmod 722 ${up_tmp_dir}${depacket_dir}/${install_sh}
	retstr='test'
	retstr=`exec ${up_tmp_dir}${depacket_dir}/${install_sh} ${up_tmp_dir}${depacket_dir}/`
	if [ "$?" -ne 0 ]; then
		echo "instal fail .. "
		echo ${retstr}
		echo "rm ${up_tmp_dir} -rf"
		rm ${up_tmp_dir} -rf
		echo "update_fail"
		exit 1
	else
		echo "cp ${up_tmp_dir}${new_ver_fn} $cu_config "
		cp  ${up_tmp_dir}${new_ver_fn} $cu_config -f
		
		echo "rm ${up_tmp_dir} -rf"
		rm ${up_tmp_dir} -rf
		echo "update_ok"
		aplay /tmp/mounts/SD-P1/voice/updatesoftfinsh.wav
	fi
fi
exit 0
