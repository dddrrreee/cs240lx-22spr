# icestorm/yosys/nextpnr prereqs
sudo apt-get install -y build-essential clang bison flex libreadline-dev \
                     gawk tcl-dev libffi-dev git mercurial graphviz   \
                     xdot pkg-config python python3 libftdi-dev \
                     python3-dev libboost-all-dev cmake libeigen3-dev


# verilator prereqs
sudo apt-get install -y git perl python3 make
sudo apt-get install -y g++  # Alternatively, clang
sudo apt-get install -y libgz  # Non-Ubuntu (ignore if gives error)
sudo apt-get install -y libfl2  # Ubuntu only (ignore if gives error)
sudo apt-get install -y libfl-dev  # Ubuntu only (ignore if gives error)
sudo apt-get install -y zlibc zlib1g zlib1g-dev  # Ubuntu only (ignore if gives error)
sudo apt-get install -y ccache  # If present at build, needed for run
sudo apt-get install -y libgoogle-perftools-dev numactl perl-doc
sudo apt-get install -y git autoconf flex bison
