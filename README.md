A ROS 2 (Jazzy) project built to deeply understand robotics middleware
by implementing a smart room system from scratch — custom messages, 
URDF, sensor nodes, controller nodes, and a full launch pipeline.

## Current State
Phase 1 (simulation) complete. Moving to real hardware.

## What exists right now

### Infrastructure
- Custom ROS 2 interface package — RoomStatus.msg, SetFanSpeed.srv, Greet.action
- Robot URDF with 6 links and full TF tree
- RViz2 configuration
- Unified launch file — entire system starts with one command
- YAML parameter file — all configuration in one place
- Dockerized development environment with NVIDIA GPU acceleration

### Nodes
| Node | Language | Pattern |
|---|---|---|
| temp_sensor_node | Python | Publisher |
| humidity_sensor_node | Python | Publisher |
| fan_node | Python | Service server |
| speaker_node | Python | Action server |
| fan_controller_node | C++ | Subscriber + service client |
| greeting_controller_node | C++ | Subscriber + action client |

### ROS 2 concepts covered
- Custom msg / srv / action with rosidl
- URDF and TF transforms
- Publishers and subscribers
- Service client and server
- Action client and server
- Runtime parameters with callbacks
- Mixed Python/C++ package
- Unified launch files with YAML params

## Phase 2 — Real hardware (in progress)
- Raspberry Pi 5 as ROS 2 brain
- Teensy 4.1 with micro-ROS for motor control
- RPLiDAR A1 for SLAM and navigation
- Nav2 autonomous navigation
- Real temperature and humidity sensors
- Camera with person detection

# The hardware part is still not finalised.

## Stack
- ROS 2 Jazzy, Ubuntu 24.04, Docker + NVIDIA GPU
- Python and C++