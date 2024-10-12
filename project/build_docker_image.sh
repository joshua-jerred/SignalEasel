#!/bin/bash

# To run the image: docker run -it --rm --name signaleasel joshuajerred/signaleasel

# Must be envoked from the repository root
if [[ $(git rev-parse --show-toplevel) != $(pwd) ]]; then
  echo "This script must be envoked from the repository root"
  exit 1
fi

IMAGE_NAME="joshuajerred/signaleasel"
DOCKERFILE_PATH="project/Dockerfile"

# Build the docker image
docker build -t $IMAGE_NAME -f $DOCKERFILE_PATH .

# Push the docker image
docker push $IMAGE_NAME