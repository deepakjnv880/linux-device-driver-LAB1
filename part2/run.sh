make clean
make all
sudo rmmod first.ko
sudo insmod first.ko string="babni"
modinfo first.ko
make clean
clear
clear
echo "=====================>\n"
#dmesg
tail -2 /var/log/kern.log