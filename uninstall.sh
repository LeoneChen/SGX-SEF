echo "c" | sudo -S /opt/intel/sgxsdk/uninstall.sh

echo "c" | sudo -S apt-get purge libsgx-launch libsgx-urts libsgx-epid libsgx-quote-ex libsgx-dcap-ql -y
echo "c" | sudo -S apt-get purge libsgx-launch-dbgsym libsgx-urts-dbgsym libsgx-epid-dbgsym libsgx-quote-ex-dbgsym libsgx-dcap-ql-dbgsym -y
echo "c" | sudo -S apt-get purge libsgx-launch-dev libsgx-urts-dev libsgx-epid-dev libsgx-quote-ex-dev libsgx-dcap-ql-dev -y
echo "c" | sudo -S apt-get purge libsgx-ae-epid libsgx-ae-le libsgx-ae-pce libsgx-ae-qe3 libsgx-ae-qve \
libsgx-aesm-launch-plugin libsgx-dcap-quote-verify libsgx-enclave-common sgx-aesm-service -y