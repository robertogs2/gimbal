autoreconf --verbose --force --install || {
 echo 'autogen.sh failed';
 exit 1;
}


rm -rf bin
mkdir bin
cd bin

echo
echo "Steps to compile:"
echo
echo "1. cd bin"
echo "2. run configure:"
echo -e "\t Run ../configure --prefix=\$PWD/usr  for pc."
echo "If crosscompilation requiered, run . /opt/poky/<version>/environment..."
echo -e "\t Run ../configure --prefix=\$PWD/usr --host=arm-poky-linux-gnueabi for cross compile."
echo "3. Then type 'make' to compile $package."
echo "4. make install"
echo
echo
echo "IF CROSS COMPILE: to install on raspberry:"
echo -e "\t Insert sd card."
echo -e "\t sudo cp -r usr /media/<usr>/<rasp file system>/usr."
echo
echo