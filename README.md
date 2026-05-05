# Smart Room Monitor — ROS 2 from Scratch

A ROS 2 (Jazzy) project built to deeply understand robotics middleware 
by implementing a simulated smart room system from the ground up with
custom messages, URDF, TF tree, sensor nodes, and a full launch pipeline.

## Current State
Early development. Core infrastructure is complete.

## What exists right now
- Custom ROS 2 interface package with a message, service, and action definition
- Robot URDF with 6 links: body, temperature sensor, humidity sensor, camera, speaker, fan
- Full TF tree broadcasting via robot_state_publisher
- RViz2 configuration with RobotModel and TF displays
- Single launch file that brings up the entire system in one command
- Dockerized development environment with NVIDIA GPU acceleration

## In progress
- Python sensor publisher nodes (temperature, humidity, camera)
- C++ controller nodes (fan, speaker)
- ROS 2 service server for fan speed control
- ROS 2 action server for speaker interaction

## Planned
- Nav2 autonomous navigation
- micro-ROS integration on Teensy 4.1
- Deployment to Raspberry Pi 5

## Stack
- ROS 2 Jazzy, Ubuntu 24.04, Docker
- Python and C++
