#!/bin/bash

DEVDIR=dev

FULLDEVDIR=$HOME/$DEVDIR

# Put everything into dev
cd $HOME
mkdir $DEVDIR

sudo rm /etc/rc.local
set +H
sudo sh -c "echo '#!/bin/sh\n/home/ubuntu/jetson_clocks.sh\nnvpmodel -m 0\nexit 0\n' >> /etc/rc.local"
set -H
sudo chmod a+x /etc/rc.local

# Prerequisites
# Base
sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get -y install build-essential cmake cmake-curses-gui git libssl-dev libboost-all-dev libgflags-dev libgoogle-glog-dev uuid-dev libboost-filesystem-dev libboost-system-dev libboost-thread-dev ncurses-dev pkg-config

# BLAS
sudo apt-get -y install libatlas-base-dev libopenblas-base libopenblas-dev liblapack-dev liblapack3 gsl-bin libgsl0-dev

# Codecs
sudo apt-get -y install libdc1394-22 libdc1394-22-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libtheora-dev libxvidcore-dev x264 v4l-utils gstreamer1.0-tools gstreamer1.0-plugins-ugly libvorbis-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev

# IO
sudo apt-get -y install libusb-1.0-0-dev libudev-dev

# GUI
sudo apt-get -y install libgtk2.0-dev libglew-dev libgtk-3-dev libglfw3-dev

# Eigen
cd $FULLDEVDIR
wget --no-check-certificate http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz
tar xzvf 3.3.4.tar.gz
sudo rm -r 3.3.4.tar.gz
cd eigen-eigen-5a0156e40feb
mkdir build
cd build
cmake ../
sudo make install

# OpenCV
cd $FULLDEVDIR
git clone --branch 3.4.0 https://github.com/opencv/opencv.git
git clone --branch 3.4.0 https://github.com/opencv/opencv_contrib.git
cd opencv
mkdir build
cd build
cmake -DTINYDNN_USE_SSE=OFF -DTINYDNN_USE_AVX=OFF -DCPU_BASELINE_DISABLE="" -DENABLE_NEON=ON -DENABLE_VFPV3=ON -DBUILD_CUDA_STUBS=OFF -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_JASPER=OFF -DBUILD_JAVA=OFF -DBUILD_JPEG=OFF -DBUILD_OPENEXR=OFF -DBUILD_PACKAGE=ON -DBUILD_PERF_TESTS=OFF -DBUILD_PNG=OFF -DBUILD_PROTOBUF=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_TBB=OFF -DBUILD_TESTS=OFF -DBUILD_TIFF=OFF -DBUILD_WITH_DEBUG_INFO=OFF -DBUILD_WITH_DYNAMIC_IPP=OFF -DBUILD_ZLIB=OFF -DBUILD_opencv_apps=ON -DBUILD_opencv_aruco=ON -DBUILD_opencv_bgsegm=ON -DBUILD_opencv_bioinspired=ON -DBUILD_opencv_calib3d=ON -DBUILD_opencv_ccalib=ON -DBUILD_opencv_core=ON -DBUILD_opencv_datasets=ON -DBUILD_opencv_dnn=OFF -DBUILD_opencv_dpm=ON -DBUILD_opencv_face=ON -DBUILD_opencv_features2d=ON -DBUILD_opencv_flann=ON -DBUILD_opencv_freetype=OFF -DBUILD_opencv_fuzzy=OFF -DBUILD_opencv_highgui=ON -DBUILD_opencv_img_hash=ON -DBUILD_opencv_imgcodecs=ON -DBUILD_opencv_imgproc=ON -DBUILD_opencv_js=OFF -DBUILD_opencv_line_descriptor=ON -DBUILD_opencv_ml=ON -DBUILD_opencv_objdetect=ON -DBUILD_opencv_optflow=ON -DBUILD_opencv_phase_unwrapping=ON -DBUILD_opencv_photo=ON -DBUILD_opencv_plot=ON -DBUILD_opencv_python_bindings_g=ON -DBUILD_opencv_reg=ON -DBUILD_opencv_rgbd=ON -DBUILD_opencv_saliency=OFF -DBUILD_opencv_sfm=ON -DBUILD_opencv_shape=ON -DBUILD_opencv_stereo=ON -DBUILD_opencv_stitching=OFF -DBUILD_opencv_structured_light=ON -DBUILD_opencv_superres=ON -DBUILD_opencv_surface_matching=ON -DBUILD_opencv_text=OFF -DBUILD_opencv_tracking=ON -DBUILD_opencv_ts=ON -DBUILD_opencv_video=ON -DBUILD_opencv_videoio=ON -DBUILD_opencv_videostab=ON -DBUILD_opencv_world=OFF -DBUILD_opencv_xfeatures2d=ON -DBUILD_opencv_ximgproc=ON -DBUILD_opencv_xobjdetect=ON -DBUILD_opencv_xphoto=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_INSTALL_PREFIX=/usr -DENABLE_PRECOMPILED_HEADERS=OFF -DOPENCV_ENABLE_NONFREE=OFF -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules -DWITH_1394=ON -DWITH_ARAVIS=OFF -DWITH_CAROTENE=ON -DWITH_CLP=OFF -DWITH_CUBLAS=OFF -DWITH_CUDA=OFF -DWITH_CUFFT=OFF -DWITH_EIGEN=ON -DWITH_FFMPEG=ON -DWITH_GDAL=OFF -DWITH_GDCM=OFF -DWITH_GIGEAPI=OFF -DWITH_GPHOTO2=ON -DWITH_GSTREAMER=ON -DWITH_GSTREAMER_0_10=OFF -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_HALIDE=OFF -DWITH_ITT=ON -DWITH_JASPER=ON -DWITH_JPEG=ON -DWITH_LAPACK=ON -DWITH_LIBV4L=ON -DWITH_MATLAB=ON -DWITH_MFX=OFF -DWITH_NVCUVID=OFF -DWITH_OPENCL=ON -DWITH_OPENCLAMDBLAS=ON -DWITH_OPENCLAMDFFT=ON -DWITH_OPENCL_SVM=OFF -DWITH_OPENEXR=ON -DWITH_OPENGL=OFF -DWITH_OPENMP=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_OPENVX=OFF -DWITH_PNG=ON -DWITH_PTHREADS_PF=ON -DWITH_PVAPI=OFF -DWITH_QT=OFF -DWITH_TBB=ON -DWITH_TIFF=ON -DWITH_UNICAP=OFF -DWITH_V4L=ON -DWITH_VA=OFF -DWITH_VA_INTEL=OFF -DWITH_VTK=ON -DWITH_WEBP=ON -DWITH_XIMEA=OFF -DWITH_XINE=OFF ../
make all -j4
sudo make install -j4

# Darknet
cd $FULLDEVDIR
git clone https://github.com/yankailab/darknet.git
cd darknet
make -j4
cd data
wget --no-check-certificate https://pjreddie.com/media/files/yolov2.weights
wget --no-check-certificate https://pjreddie.com/media/files/yolov3.weights

# RealSense on PC
cd $FULLDEVDIR
git clone https://github.com/IntelRealSense/librealsense.git
cd librealsense/
sudo cp config/99-realsense-libusb.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && udevadm trigger
./scripts/patch-realsense-ubuntu-xenial.sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j4

# Pangolin
cd $FULLDEVDIR
git clone https://github.com/yankailab/Pangolin.git
cd Pangolin
mkdir build
cd build
cmake ../
cmake --build .
sudo make install

# ORB_SLAM2
cd $FULLDEVDIR
git clone https://github.com/yankailab/orb_slam2.git
cd orb_slam2
chmod +x build.sh
./build.sh

# gwsocket
git clone https://github.com/allinurl/gwsocket.git
cd gwsocket
autoreconf -fiv
./configure
make

# OpenKAI
cd $FULLDEVDIR
git clone https://github.com/yankailab/OpenKAI.git
cd OpenKAI
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DUSE_CUDA=ON -DCUDA_ARCH=62 -DUSE_OPENCV_CONTRIB=ON -DUSE_DARKNET=ON -DDarknet_root=/home/ubuntu/dev/darknet -DUSE_REALSENSE=OFF -Dlibrealsense_root=/home/ubuntu/dev/librealsense -DUSE_TENSORRT=ON -DTensorRT_build=/home/ubuntu/dev/jetson-inference-batch/build/aarch64 ../
make all -j4

