#export ARMDIR="/opt/jerry/tool"
#eport PATH=$PATH:${ARMDIR}/bin

# arm
#./configure --prefix=`pwd`/build --host=arm-linux CC=arm-linux-androideabi-gcc CXX=arm-linux-androideabi-g++

# host
./configure --prefix=`pwd`/build  --without-ssl  --without-ca-path --disable-ldap

#./configure --prefix=`pwd`/build

#make
