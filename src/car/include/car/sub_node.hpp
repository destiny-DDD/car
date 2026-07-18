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
      {{"chassis_data"}});

  // static SharedTopicClient shared_topic_client(
  //     hw, appmgr, "uart_client", 256,
  //     {{"topic1", "libxr_def_domain"}});
}

class CarSubscription : public rclcpp::Node {
private:

  // send
  struct WheelMsg {
    float speed_x;
    float speed_y;
    float ang_z;
  };

  // LibXR
  std::unique_ptr<LibXR::HardwareContainer> peripherals_;
  std::unique_ptr<LibXR::RamFS> ramfs_;
  std::unique_ptr<LibXR::LinuxUART> uart_client_;
  std::unique_ptr<LibXR::Terminal<1024, 64, 16, 128>> terminal_;
  std::unique_ptr<LibXR::Thread> term_thread_;

  LibXR::Topic wheel;
  LibXR::Topic::Callback cb0;

public:
  explicit CarSubscription(const std::string &name);
  // void receive();
};

} // namespace car_sub

#endif