FROM ubuntu:24.04
LABEL authors="james"


RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-venv \
    lcov \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Create a venv for Conan
RUN python3 -m venv /opt/venv
# Activate venv
ENV PATH="/opt/venv/bin:$PATH"

# Install Conan inside the venv
RUN pip install --upgrade pip \
  && pip install conan

# Configure Conan default profile
RUN conan profile detect --force

# Set working dir and copy project files
WORKDIR /app
COPY --exclude=build/ . .

# Build with Conan
RUN conan build . --build=missing -s build_type=Release


CMD ["/bin/bash"]