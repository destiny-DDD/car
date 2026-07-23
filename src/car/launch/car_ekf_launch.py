import os
import launch
import launch_ros
from ament_index_python import get_package_share_directory

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('car'),
        'config',
        'params.yaml',
    ),
    ekf_config = os.path.join(
        get_package_share_directory('car'),
        'config',
        'ekf.yaml',
    )
    box_config = os.path.join(
        get_package_share_directory('car'),
        'config',
        'filter_example.yaml',
    )

    action_car=launch_ros.actions.Node(
        package='car',
        executable='car',
        parameters=[config],
        output='screen',
    )

    action_ekf=launch_ros.actions.Node(
        package="robot_localization",
        executable="ekf_node",
        parameters=[ekf_config],
    )

    action_box=launch_ros.actions.Node(
        package="laser_filters",
        executable="scan_to_scan_filter_chain",
        parameters=[box_config],
    )

    return launch.LaunchDescription([
        action_car,
        action_ekf,
        action_box,
    ])