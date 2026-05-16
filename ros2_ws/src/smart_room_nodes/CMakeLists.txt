cmake_minimum_required(VERSION 3.8)
project(smart_room_nodes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── C++ dependencies ──────────────────────────────────────────
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(smart_room_msgs REQUIRED)

# ── Python support ────────────────────────────────────────────
# This extension teaches ament_cmake how to handle Python
find_package(ament_cmake_python REQUIRED)
find_package(rclpy REQUIRED)

# Install the Python package (smart_room_nodes/ folder)
# This makes: from smart_room_nodes.sensor_nodes.temp_sensor_node import main
# work correctly inside the container
ament_python_install_package(${PROJECT_NAME})

# Install Python executable scripts
install(PROGRAMS
  scripts/temp_sensor
  scripts/humidity_sensor
  scripts/fan_node
  DESTINATION lib/${PROJECT_NAME}
)

# ── C++ executable ────────────────────────────────────────────
add_executable(fan_controller_node src/fan_controller_node.cpp)

ament_target_dependencies(fan_controller_node
  rclcpp
  std_msgs
  sensor_msgs
  smart_room_msgs
)

install(TARGETS
  fan_controller_node
  DESTINATION lib/${PROJECT_NAME}
)

# ── Finalize ──────────────────────────────────────────────────
ament_package()
