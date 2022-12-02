#!/bin/sh
cd ../../../

# sudo apt-get -qq update

# sudo add-apt-repository -y ppa:obsproject/obs-studio
# sudo apt-get -qq update

sudo apt-get install -y \
	libc-dev-bin \
	libc6-dev git \
	build-essential \
	checkinstall \
	cmake \
	obs-studio \
	qt6-base-dev \
	libboost-all-dev \
	libasio-dev \
	libxss-dev \
	libxtst-dev	\
	libboost-all-dev \
	# libpipewire-0.3-dev

# # Dirty hack
# sudo wget -O /usr/include/obs/obs-frontend-api.h https://raw.githubusercontent.com/obsproject/obs-studio/25.0.0/UI/obs-frontend-api/obs-frontend-api.h

if [ ! -d "obs-studio" ]; then
	git clone --recursive https://github.com/obsproject/obs-studio.git
fi

if [ ! -d "obsBuildDir" ]; then
	mkdir obsBuildDir
fi
cd obs-studio/CI/
chmod +x build-linux.sh
./build-linux.sh --build-dir ../obsBuildDir
cd ../../obsBuildDir
sudo cmake -P cmake_install.cmake
# cd ../UI/obs-frontend-api/ &&
# sudo cmake -P cmake_install.cmake &&
# pwd &&
# cd ../../../easystream_main_plugin/
# sudo ldconfig
