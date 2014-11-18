#
# Install Boost development libraries, curl development libraries, python 2.7 and g++
#
sudo apt-get install -y --force-yes libboost-dev libcurl4-openssl-dev python2.7 g++

#
# Install cURLpp
#
wget http://curlpp.googlecode.com/files/curlpp-0.7.3.tar.gz
tar -xzf curlpp-0.7.3.tar.gz
rm curlpp-0.7.3.tar.gz
mv curlpp-0.7.3 curlpp
cd curlpp
./configure
make
make install
cd ..
rm -rf curlpp/

#
# Install easy_install and use that to install Mechanize
#
wget http://pypi.python.org/packages/2.7/s/setuptools/setuptools-0.6c11-py2.7.egg
sudo chmod +x setuptools-0.6c11-py2.7.egg
./setuptools-0.6c11-py2.7.egg
sudo easy_install mechanize
rm setuptools-0.6c11-py2.7.egg
#
# Download JsonCpp. Download and install Scons. Use Scons to build JsonCpp. Copy JsonCpp .a and .so files to proper lib directory.
#
wget http://iweb.dl.sourceforge.net/project/jsoncpp/jsoncpp/0.5.0/jsoncpp-src-0.5.0.tar.gz
tar -xzf jsoncpp-src-0.5.0.tar.gz
rm jsoncpp-src-0.5.0.tar.gz
mv jsoncpp-src-0.5.0 jsoncpp
wget -O scons.tar.gz http://downloads.sourceforge.net/project/scons/scons/2.3.4/scons-2.3.4.tar.gz?r=&ts=1416275814&use_mirror=iweb -O scons-2.3.4.tar.gz
tar -xzf scons.tar.gz
rm scons.tar.gz
cd scons
sudo python setup.py install
cp script/scons ../jsoncpp/scons.py
cd ../jsoncpp
sudo python scons.py platform=linux-gcc
sudo cp -r include/json/ /usr/include/json/
cd libs/linux-gcc-*
sudo rm /usr/local/lib/libjson_gcc-*.a /usr/local/lib/libjson_gcc-*.so
sudo cp *.a *.so /usr/local/lib/
sudo ln -sf /usr/local/lib/libjson_linux-gcc-*_libmt.a /usr/local/lib/libjson.a
sudo ln -sf /usr/local/lib/libjson_linux-gcc-*_libmt.so /usr/local/lib/libjson.so
cd ../../..
sudo rm -rf jsoncpp/ scons/

#
# Install Ncurses
#
wget ftp://ftp.gnu.org/pub/gnu/ncurses/ncurses-5.9.tar.gz
tar -xzf ncurses*.tar.gz
rm ncurses*.tar.gz
mv ncurses* ncurses
cd ncurses
./configure
make
sudo make install
cd ..
rm -rf ncurses

#
# Install GNU Readline
#
wget ftp://ftp.cwru.edu/pub/bash/readline-6.2.tar.gz
tar -xzf readline-6.2.tar.gz
rm -f readline-6.2.tar.gz
mv readline-6.2 readline
cd readline
./configure
make
sudo make install
cd ..
rm -rf readline/

#
# Run ldconfig so that all new libraries are recognized.
#
sudo bash -c "echo /usr/local/lib/ > /etc/ld.so.conf.d/local.conf"
sudo /sbin/ldconfig
