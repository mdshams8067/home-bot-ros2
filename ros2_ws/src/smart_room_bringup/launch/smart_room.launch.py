import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    # Paths
    urdf_file = os.path.join(
        get_package_share_directory('smart_room_description'),
        'urdf',
        'smart_room_robot.urdf'
    )

    rviz_config = os.path.join(
        get_package_share_directory('smart_room_bringup'),
        'rviz2',
        'smart_room.rviz'
    )

    params_file = os.path.join(
        get_package_share_directory('smart_room_bringup'),
        'config',
        'smart_room_params.yaml'
    )

    with open(urdf_file, 'r') as f:
        robot_description = f.read()

    return LaunchDescription([

        # ── Robot description and TF ──────────────────────────
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[{'robot_description': robot_description}],
            output='screen'
        ),

        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher',
            output='screen'
        ),

        # ── Sensor nodes (Python publishers) ─────────────────
        Node(
            package='smart_room_nodes',
            executable='temp_sensor',
            name='temp_sensor_node',
            parameters=[params_file],
            output='screen'
        ),

        Node(
            package='smart_room_nodes',
            executable='humidity_sensor',
            name='humidity_sensor_node',
            parameters=[params_file],
            output='screen'
        ),

        # ── Actuator nodes (Python service/action servers) ────
        Node(
            package='smart_room_nodes',
            executable='fan_node',
            name='fan_node',
            output='screen'
        ),

        Node(
            package='smart_room_nodes',
            executable='speaker_node',
            name='speaker_node',
            output='screen'
        ),

        # ── Controller nodes (C++ subscribers + clients) ──────
        Node(
            package='smart_room_nodes',
            executable='fan_controller_node',
            name='fan_controller_node',
            parameters=[params_file],
            output='screen'
        ),

        Node(
            package='smart_room_nodes',
            executable='greeting_controller_node',
            name='greeting_controller_node',
            parameters=[params_file],
            output='screen'
        ),

        # ── Visualisation ─────────────────────────────────────
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config],
            output='screen'
        ),

    ])
