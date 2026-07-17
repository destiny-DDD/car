#ifndef __SUB_NODE_HPP_
#define __SUB_NODE_HPP_

#include "SharedTopic.hpp"
#include "SharedTopicClient.hpp"
#include "linux_uart.hpp"
#include <rclcpp/rclcpp.hpp>

namespace car_sub {

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

class CarSubscription : public rclcpp::Node {
private:
  // LibXR
  std::unique_ptr<LibXR::HardwareContainer> peripherals_;
  std::unique_ptr<LibXR::RamFS> ramfs_;
  std::unique_ptr<LibXR::LinuxUART> uart_client_;
  std::unique_ptr<LibXR::Terminal<1024, 64, 16, 128>> terminal_;
  std::unique_ptr<LibXR::Thread> term_thread_;

public:
  explicit CarSubscription(const std::string &name);
  void receive();
};

} // namespace car_sub

#endif