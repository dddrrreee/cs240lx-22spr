git clone https://github.com/YosysHQ/nextpnr nextpnr-ice40
cd nextpnr-ice40
cmake -DARCH=ice40 -DCMAKE_INSTALL_PREFIX=/usr/local .
make
sudo make install
