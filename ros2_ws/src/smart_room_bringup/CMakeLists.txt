cmake_minimum_required(VERSION 3.8)
project(smart_room_bringup)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY launch rviz2 config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
