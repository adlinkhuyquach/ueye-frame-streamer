Preface
=======
The AEA uEye Frame Streamer app grabs frames from uEye Camera and publishs to Data River.


Build Docker Images
===================
docker build --pull --build-arg ARTIFACTORY_USERNAME=usr --build-arg ARTIFACTORY_PASSWORD=pass -t aea-ueye-frame-streamer .

Run Docker Container
====================
sudo docker run --rm -ti --name ueye_test --network=host --privileged -v /dev/=/dev/ -v $(pwd)/build:/vortexedge/config/ -e RLM_LICENSE=5053@licsrv-adlink.prismtech.com -t artifactory.prismtech.com/edge-docker-dev-ghead/adlinktech/aea-ueye-frame-streamer:master
