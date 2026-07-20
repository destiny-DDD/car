import os
import launch
import launch_ros
from ament_index_python import get_package_share_directory


def generate_launch_description():
    pkg_dir_heart = get_package_share_directory("heart")
    pkg_dir_car = get_package_share_directory("car")
    pkg_dir_MID360=get_package_share_directory("livox_ros_driver2")
    pkg_dir_imu=get_package_share_directory("yesense_std_ros2")

    # 默认 xacro 文件路径
    xacro_file = os.path.join(pkg_dir_heart, "urdf", "car.urdf.xacro")
    default_rviz_config_path=os.path.join(pkg_dir_heart,"config","rviz.rviz")
    MID360_launch = os.path.join(
        pkg_dir_MID360,"launch_ROS2","msg_MID360_launch.py"
    )
    imu_launch = os.path.join(
        pkg_dir_imu,"launch","yesense_node.launch.py"
    )
    car_launch = os.path.join(
        pkg_dir_car,"launch","car_launch.py"
    )

    action_declare_arg_mode_path=launch.actions.DeclareLaunchArgument(
        name='model',
        default_value=xacro_file,
        description='加载的文件模型路径'
    )

    action_robot_state_publisher=launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{
            'robot_description':launch.substitutions.Command([
                'xacro ',launch.substitutions.LaunchConfiguration('model')
                ])}]
    )

    action_joint_state_publisher_gui=launch_ros.actions.Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
    )

    action_joint_state_publisher=launch_ros.actions.Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
    )

    action_rviz_node=launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d',default_rviz_config_path],
    )

    action_car_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            car_launch
        )
    )
    
    action_MID360_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            MID360_launch
        )
    )

    action_imu_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            imu_launch
        )
    )

    return launch.LaunchDescription([
        action_car_launch,
        action_declare_arg_mode_path,
        action_robot_state_publisher,
        # action_joint_state_publisher_gui,
        action_joint_state_publisher,
        action_rviz_node,
        action_MID360_launch,
        action_imu_launch,
    ])
