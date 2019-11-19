cmake_minimum_required(VERSION 3.15.5)
project(GpuSwitcher)
set (GpuSwitcher_VERSION_MAJOR 1)
set (GpuSwitcher_VERSION_MINOR 0)

if(UNIX AND NOT APPLE)

add_executable(gpud gpud.c)
add_executable(gpum gpu.c)

install (TARGETS gpud DESTINATION bin)
install (TARGETS gpum DESTINATION bin)
install (FILES gpud.service DESTINATION /usr/lib/systemd/system)
install (DIRECTORY DESTINATION /etc/gpu.d)
install (FILES user.conf DESTINATION /etc/gpu.d)

endif()