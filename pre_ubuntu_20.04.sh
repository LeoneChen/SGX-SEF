sudo apt-get install build-essential ocaml ocamlbuild automake autoconf libtool wget python-is-python3 libssl-dev git cmake perl -y
sudo apt-get install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake reprepro unzip -y
sudo apt-get install build-essential python -y
sudo apt-get install libssl-dev libcurl4-openssl-dev libprotobuf-dev -y

make preparation
sudo cp external/toolset/ubuntu20.04/{as,ld,ld.gold,objdump} /usr/local/bin

if [ $(grep -Ec "^[^#].*sgx_debian_local_repo" /etc/apt/sources.list) -eq 0 ]; then
  echo "add apt source"
  sudo sh -c "echo \"deb [trusted=yes arch=amd64] file:/home/leone/文档/SGX-SEF/linux/installer/deb/sgx_debian_local_repo focal main\" >>/etc/apt/sources.list"
else
  echo "already added apt source"
fi
