FROM debian:stable-slim

# joshuajerred/signaleasel
# To run the image: docker run -it --rm --name signaleasel joshuajerred/signaleasel

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    libpulse-dev \
    libgtest-dev \
    doxygen \
    graphviz

# Build and install gtest
# RUN cd /usr/src/gtest && cmake CMakeLists.txt && make && cp lib/*.a /usr/lib && ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a && ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a