#include "pub_node.hpp"
#include "sub_node.hpp"
#include <ament_index_cpp/get_package_share_directory.hpp>

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  std::string config_file_path =
      ament_index_cpp::get_package_share_directory("car") +
      "/config/params.yaml";
  rclcpp::NodeOptions options;
  options.arguments({"--ros-args", "--params-file", config_file_path});
  auto pub_node = std::make_shared<car_pub::CarPublisher>("pub_node", options);
  // auto sub_node =
  //     std::make_shared<car_sub::CarSubscription>("sub_node", options);
  auto executor = rclcpp::executors::MultiThreadedExecutor();
  executor.add_node(pub_node);
  // executor.add_node(sub_node);
  executor.spin();
  rclcpp::shutdown();
  return 0;
}