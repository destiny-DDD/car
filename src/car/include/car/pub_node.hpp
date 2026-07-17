#ifndef __PUB_NODE_HPP_
#define __PUB_NODE_HPP_

#include "SharedTopic.hpp"
#include "SharedTopicClient.hpp"
#include "linux_uart.hpp"
#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>

namespace car_pub {

static void XRobotMain(LibXR::HardwareContainer &hw) {
  using namespace LibXR;
  static ApplicationManager appmgr;

  static SharedTopic shared_topic(
      hw, appmgr, "uart_client", 256,
      {{"ahrs_quaternion"}, {"lob_shot"}, {"reset"}});

  static SharedTopicClient shared_topic_client(
      hw, appmgr, "uart_client", 256,
      {{"target_euler"}, {"fire_notify", "tracker"}, {"target_num"}});
}

class CarPublisher : public rclcpp::Node {
public:
  explicit CarPublisher(const std::string &name);
  void send_data_callback(const geometry_msgs::msg::Twist::SharedPtr msg_data);
  void ser(const std::vector<uint8_t>& cmd);

private:
  // ros2
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;

  // send
  std::vector<uint8_t> cmd;

  // LibXR
  std::unique_ptr<LibXR::HardwareContainer> peripherals_;
  std::unique_ptr<LibXR::RamFS> ramfs_;
  std::unique_ptr<LibXR::LinuxUART> uart_client_;
  std::unique_ptr<LibXR::Terminal<1024, 64, 16, 128>> terminal_;
  std::unique_ptr<LibXR::Thread> term_thread_;

  LibXR::Topic wheel;
};

} // namespace car

#endif