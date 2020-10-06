#!/bin/bash
set -ex

# Assign arguments to variables
# The type of doc e.g. userguide
DOC_TYPE=$1
# Any other parameters to pass to the docs build
DOCS_PARM=$2

# Remove old docs
rm -rf vortexedge/docs/adlinktech/${APP_NAME}/${DOC_TYPE}

# Get git version
git gc --prune=now
git fetch -t
TAGGED=$(git describe --abbrev=0 --exact-match || echo "Not-Tagged")
GIT_HASH=$(git rev-parse --short HEAD)
GIT_TAG=$(git describe --abbrev=0 || echo "Not-Tagged")

# Checks it the version contains "-" which indicates it is not a tagged build.
if [[ "${TAGGED}" = "Not-Tagged" ]]
then
   VERSION=$GIT_HASH
else
   # Tagged build
   # Get the build number (all characters after the last '.')
   BUILD_NO=${GIT_TAG##*.}
   # Get the version number (all characters before the last '.')
   VERSION_NO=${GIT_TAG%.*}
   # Set the full version in the format "a.b.c (Build YY##)"
   # where a.b.c is the release number (without the RCx extension),
   # YY is the year and ## is the release candidate number (e.g. 03).
   VERSION="${VERSION_NO} (Build ${BUILD_NO})"
fi

# Build new docs
docker run --rm -u `id -u`:`id -g` --env VERSION="${VERSION}" -v $(pwd)/docs:/docs -v $(pwd)/vortexedge:/vortexedge ${DOCSBUILDER} docs/${DOC_TYPE}/source vortexedge/docs/adlinktech/${APP_NAME}/${DOC_TYPE} ${DOCS_PARM}
