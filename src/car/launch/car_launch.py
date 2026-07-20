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

    action_car=launch_ros.actions.Node(
        package='car',
        executable='car',
        parameters=[config],
        output='screen',
    )

    return launch.LaunchDescription([
        action_car,
    ])