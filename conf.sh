wget http://superb-dca3.dl.sourceforge.net/project/boost/boost/1.51.0/boost_1_51_0.tar.gz
tar -vxzf boost_1_51_0.tar.gz
rm boost_1_51_0.tar.gz
mv boost_1_51_0/ boost/
cd boost
./bootstrap.sh
./b2
cp -r boost/* /usr/local/include/
cd ..
rm -rf boost/
