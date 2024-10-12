FROM debian:stable-slim
# ^ debian bookworm at the moment

# joshuajerred/signaleasel
# To run the image: 
# docker run -it --rm --name signaleasel joshuajerred/signaleasel

# Update and install basic dependencies
RUN apt-get update
RUN apt-get install -y \
    git \
    build-essential \
    cmake \
    clang-tidy

# Install libraries
RUN apt-get install -y \
    libpulse-dev \
    libgtest-dev

# Install documentation tools
RUN apt-get install -y \
    doxygen \
    graphviz

# Install gcovr and lcov
RUN apt-get install -y \
    python3-full \
    python3-pip \
    lcov
RUN pip install gcovr --break-system-packages

# Build and install gtest
# RUN cd /usr/src/gtest && cmake CMakeLists.txt && make && cp lib/*.a /usr/lib && ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a && ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a