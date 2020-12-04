#!/bin/bash
set -x
sudo yum -y group install "Development Tools"
sudo wget https://download.open-mpi.org/release/open-mpi/v3.1/openmpi-3.1.2.tar.gz
sudo tar xzf openmpi-3.1.2.tar.gz
cd openmpi-3.1.2
sudo ./configure --prefix=/software/openmpi/3.1.2 --enable-orterun-prefix-by-default
sudo make
sudo make all install

while IFS= read -r line; do
  echo 'export PATH=$PATH:/software/openmpi/3.1.2/bin' | sudo tee -a /users/$i/.bashrc
  echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/software/openmpi/3.1.2/lib/' | sudo tee -a /users/$i/.bashrc
done < <( ls -l /users | grep rwx | cut -d' ' -f3 )

sudo rm -Rf openmpi-3.1.2
sudo rm -Rf openmpi-3.1.2.tar.gz 
