A ROS 2 (Jazzy) project built to deeply understand robotics middleware
by implementing a simulated smart room system from the ground up.

## Current State
Active development. Core communication pipeline complete.

## What exists right now
- Custom ROS 2 interface package with msg, srv, and action definitions
- Robot URDF with 6 links — body, temperature sensor, humidity sensor,
  camera, speaker, fan — with full TF tree
- RViz2 configuration with RobotModel and TF displays
- Single launch file that brings up the entire system in one command
- Dockerized development environment with NVIDIA GPU acceleration
- Temperature sensor publisher node (Python) with runtime parameters
- Humidity sensor publisher node (Python) with runtime parameters
- Fan controller node (C++) — subscribes to temperature, calls fan service
- Fan service server node (Python) — handles SetFanSpeed requests
- Complete pub/sub + service loop working across Python and C++ nodes

## In progress
- Speaker action server (Python)
- Greeting controller (C++)
- Unified launch file for all nodes
- Parameter YAML config file

## Planned
- Nav2 autonomous navigation
- micro-ROS integration on Teensy 4.1
- Deployment to Raspberry Pi 5

## ROS 2 concepts covered so far
- Custom msg / srv / action definitions with rosidl
- URDF and TF transforms
- Python publishers with runtime parameter callbacks
- C++ subscriber with service client
- Python service server
- Mixed Python/C++ package with ament_cmake

## Stack
- ROS 2 Jazzy, Ubuntu 24.04, Docker + NVIDIA GPU
- Python and C++