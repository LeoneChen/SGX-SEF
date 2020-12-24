if [ -n "$1" ]; then
  FLAG=$1
fi

make sdk_install_pkg ${FLAG} -s
if [ $? -ne 0 ]; then
  echo "error: make sdk_install_pkg"
  exit
fi
echo "success: make sdk_install_pkg"

echo "c" | sudo -S ./linux/installer/bin/sgx_linux_x64_sdk_*.bin <<EOF
no
/opt/intel/
EOF
if [ $? -ne 0 ]; then
  echo "error: install sgx sdk"
  exit
fi
echo "success: install sgx sdk"

make deb_local_repo ${FLAG} -s
if [ $? -ne 0 ]; then
  echo "error: make deb_local_repo"
  exit
fi
echo "success: make deb_local_repo"

echo "c" | sudo -S apt update
echo "c" | sudo -S apt-get install libsgx-launch libsgx-urts libsgx-epid libsgx-quote-ex libsgx-dcap-ql -y
echo "c" | sudo -S apt-get install libsgx-launch-dbgsym libsgx-urts-dbgsym libsgx-epid-dbgsym libsgx-quote-ex-dbgsym libsgx-dcap-ql-dbgsym -y
echo "c" | sudo -S apt-get install libsgx-launch-dev libsgx-urts-dev libsgx-epid-dev libsgx-quote-ex-dev libsgx-dcap-ql-dev -y
