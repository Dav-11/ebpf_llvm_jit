#!/bin/bash

set -e

# Function to display help message
show_help() {
  echo "Usage: $0 [OPTION]"
  echo "Install main dependencies for your project."
  echo
  echo "Options:"
  echo "  -a         Install both main and QEMU dependencies."
  echo "  -q         Install only QEMU dependencies."
  echo "  -h         Display this help message."
}

# Function to check if the script is run on a Debian-based system
check_os() {
  if [[ -e /etc/debian_version ]]; then
    echo "System is Debian-based."
  else
    echo "Error: This script is only compatible with Debian-based systems."
    exit 1
  fi
}

# Function to check if the user has root privileges
check_root() {
  if [[ "$EUID" -ne 0 ]]; then
    echo "Error: This script must be run as root."
    exit 1
  fi
}

# install main deps
install_main() {
  apt install -y \
    sudo \
    git \
    build-essential \
    cmake \
    llvm-15 clang-15 \
    zlib1g-dev \
    libelf-dev \
    pkgconf \
    gcc-multilib
}

# install riscv64-qemu deps
install_qemu() {
  apt install -y \
      qemu-system-riscv64 \
      gcc-riscv64-unknown-elf
}

####
# MAIN
####

# Check if the system is Debian-based and if the user has root privileges
check_os
check_root

if [[ $# -eq 0 ]]; then
  show_help
  exit 0
fi

while getopts "aqhum" opt; do
  case "$opt" in
    a)
      # Install both main and QEMU dependencies
      install_main
      install_qemu
      ;;
    q)
      # Install only QEMU dependencies
      install_qemu
      ;;
    m)
      # install only main deps
      install_main
      ;;
    h | *)
      # Display help message if -h or --help is used, or for any unrecognized flag
      show_help
      exit 0
      ;;
  esac
done