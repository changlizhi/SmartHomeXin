echo "startup setting..."

cd /

if [ ! -d data ]; then
    mkdir data
fi



srcfile_path=/opt/work
runfile_path=/tmp
testflag=false


if [ ! -d param ]; then
    mkdir param
fi


if [ ! -d bakup ]; then
    mkdir bakup
fi

cd /opt/data/

if [ ! -d cache ]; then
    mkdir cache
fi

cd /opt/work

app_file=smarthome.out

if [ -f $app_file ]; then
    cp $app_file ${runfile_path}/
    cd $runfile_path
    chmod +x $app_file
    ./$app_file
    cd $srcfile_path
else
    echo "no " $app_file
    testflag=true
fi
cd /

#echo "end setting"
