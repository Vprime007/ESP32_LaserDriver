# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/vprime/esp/v5.4.1/esp-idf/components/bootloader/subproject"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/tmp"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/src/bootloader-stamp"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/src"
  "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/vprime/Documents/github/ESP32_LaserDriver/Source/Laser_Driver/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
