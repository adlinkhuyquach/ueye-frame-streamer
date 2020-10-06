################################################################################
# Build the service in a dev image
################################################################################
FROM artifactory.prismtech.com/edge-docker-dev-ghead/adlinktech/vortex-edge-cpp-dev-internal:5.1.0 as builder

###############################################################################
# Install dependencies
################################################################################

RUN apt-get update && apt-get install -y git libboost-dev curl

################################################################################
# Install ueye api
################################################################################
ENV ARTIFACTORY_USERNAME=huyq
ENV ARTIFACTORY_PASSWORD=huyq
RUN mkdir -p /ids_installation/
RUN curl -u "${ARTIFACTORY_USERNAME}:${ARTIFACTORY_PASSWORD}" http://artifactory.prismtech.com/artifactory/edge-generic-dev-ghead/ueye/amd/ids-software-suite-linux-4.93.0-64.tgz -o ids-software-suite-linux-4.93.0-64.tgz \
    && tar -xzvf ids-software-suite-linux-4.93.0-64.tgz -C /ids_installation/ \
    && rm -r ids-software-suite-linux-4.93.0-64.tgz \
    && echo "y" | /ids_installation/ueye_4.93.0.989_amd64.run \
    && rm -r /ids_installation

################################################################################
# Build and install OpenCV
################################################################################

RUN apt-get update && apt-get -y install build-essential cmake git pkg-config libgtk-3-dev \
                      libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
                      libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
                      gfortran openexr libatlas-base-dev python3-dev python3-numpy \
                      libtbb2 libtbb-dev libdc1394-22-dev

RUN mkdir ~/opencv_build && cd ~/opencv_build \
   && git clone https://github.com/opencv/opencv.git \
   && git clone https://github.com/opencv/opencv_contrib.git \
   && cd opencv_contrib \
   && git checkout 4.2.0 \
   && cd ../opencv \
   && git checkout 4.2.0 \
   && mkdir build && cd build \
   && cmake -D CMAKE_BUILD_TYPE=RELEASE \
         -D CMAKE_INSTALL_PREFIX=/usr/local \
         -D OPENCV_GENERATE_PKGCONFIG=ON \
         -D OPENCV_EXTRA_MODULES_PATH=~/opencv_build/opencv_contrib/modules .. \
   && make -j$(nproc) \
   && make install

################################################################################
# Copy the source files
################################################################################
COPY CMakeLists.txt /builder/
COPY cmake /builder/cmake
COPY src /builder/src
COPY definitions /builder/definitions
COPY vortexedge /vortexedge

ENV EDGE_APP_SDK_HOME=/vortexedge

################################################################################
# Build
################################################################################

RUN mkdir /builder/build \
    && cd /builder/build \
    && cmake ../. \
    && cmake --build .

################################################################################
# Create the run-time image
################################################################################
FROM artifactory.prismtech.com/edge-docker-dev-ghead/adlinktech/vortex-edge-base-run:5.1.0

################################################################################
# Install/Copy Dependencies
################################################################################

RUN apt-get update && apt-get install -y libgtk-3-dev \
                      libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
                      libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
                      gfortran openexr libatlas-base-dev python3-dev python3-numpy \
                      libtbb2 libtbb-dev libdc1394-22-dev

COPY --from=builder /builder/build/xerces-c/src/*.so /usr/local/lib/
COPY --from=builder /usr/lib/*ueye* /usr/lib/
COPY --from=builder /usr/local/lib/libopencv* /usr/local/lib/
COPY --from=builder /builder/build/aea-ueye-frame-streamer .
COPY --from=builder /builder/definitions/ definitions/

############################################
# Copy the licensing information to metadata
############################################
COPY vortexedge /vortexedge
RUN mkdir -p /vortexedge/metadata/config/default/ \
	 && cp ${EDGE_SDK_HOME}/etc/config/default_datariver_config_v1.2.xml /vortexedge/metadata/config/default/DataRiverConfig.xml

# Copy the license files etc.
RUN mkdir -p /vortexedge/docs/adlinktech/aea-ueye-frame-streamer/releasenotes/html \
	&& cp ${OSPL_HOME}/LICENSE "/vortexedge/docs/adlinktech/aea-ueye-frame-streamer/releasenotes/html/ADLINK-License.txt" \
	&& cp ${EDGE_SDK_HOME}/LICENSE "/vortexedge/docs/adlinktech/aea-ueye-frame-streamer/releasenotes/html/EDGESDK-License.txt" \
	&& cp /vortexedge/metadata/adlinktech/vortex-edge-base-run/licensing/packages/libarchive13/licenses/BSD/content.txt /vortexedge/docs/adlinktech/aea-ueye-frame-streamer/releasenotes/html/libarchive-license.txt

ENTRYPOINT ["entrypoint", "/aea-ueye-frame-streamer", "/vortexedge/metadata/config/UEyeFrameStreamer.xsd", "/vortexedge/config/UEyeFrameStreamer.xml"]
