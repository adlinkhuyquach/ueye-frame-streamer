#!/bin/bash
set -ex

export IMAGE_NAME=$1 # The full docker image name (without tag)
export BRANCH_NAME=$2 # The git branch name (also the resulting docker image tag)
export REGISTRY=$3 # The Docker Registry to use
export REGISTRY_USERNAME=$4 # The username for the Docker Registry
export REGISTRY_PASSWORD=$5 # The password for the Docker Registry
export DOCKERFILE=$6 # The dockerfile to be used

echo ${REGISTRY_PASSWORD} | docker login --username ${REGISTRY_USERNAME} --password-stdin ${REGISTRY}
docker build -f ${DOCKERFILE} --force-rm --compress --pull --build-arg BUILD_TYPE=Release --build-arg BRANCH_NAME=${BRANCH_NAME} --build-arg ARTIFACTORY_USERNAME=${REGISTRY_USERNAME} --build-arg ARTIFACTORY_PASSWORD=${REGISTRY_PASSWORD} -t ${REGISTRY}/${IMAGE_NAME}:${BRANCH_NAME} .
docker push ${REGISTRY}/${IMAGE_NAME}:${BRANCH_NAME}
