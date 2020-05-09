make clean
make all
sudo rmmod first.ko
sudo insmod first.ko
modinfo first.ko
make clean
clear
clear
echo "=====================>\n"
tail -2 /var/log/kern.log