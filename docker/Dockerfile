FROM osrf/ros:jazzy-desktop-full

ENV NVIDIA_VISIBLE_DEVICES=all
ENV NVIDIA_DRIVER_CAPABILITIES=all
ENV __NV_PRIME_RENDER_OFFLOAD=1
ENV __GLX_VENDOR_LIBRARY_NAME=nvidia
ENV DEBIAN_FRONTEND=noninteractive
ENV TURTLEBOT3_MODEL=burger

RUN apt-get update && apt-get install -y \
    libgl1 \
    libgl1-mesa-dri \
    libglu1-mesa \
    mesa-utils \
    libvulkan1 \
    vulkan-tools \
    ros-jazzy-ros-gz-sim \
    ros-jazzy-ros-gz-bridge \
    ros-jazzy-ros-gz-interfaces \
    ros-jazzy-ros-gz-image \
    ros-jazzy-turtlebot3 \
    ros-jazzy-turtlebot3-gazebo \
    ros-jazzy-turtlebot3-simulations \
    ros-jazzy-turtlebot3-navigation2 \
    ros-jazzy-turtlebot3-teleop \
    ros-jazzy-turtlebot3-description \
    ros-jazzy-navigation2 \
    ros-jazzy-nav2-bringup \
    ros-jazzy-slam-toolbox \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-argcomplete \
    git \
    nano \
    wget \
  && rm -rf /var/lib/apt/lists/*

RUN rosdep update

RUN echo "source /opt/ros/jazzy/setup.bash" >> /root/.bashrc
RUN echo "export TURTLEBOT3_MODEL=burger" >> /root/.bashrc

RUN mkdir -p /ros2_ws/src
WORKDIR /ros2_ws

CMD ["/bin/bash"]
