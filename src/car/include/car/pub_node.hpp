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

  // static SharedTopic shared_topic(
  //     hw, appmgr, "uart_client", 256,
  //     {{"topic1", "libxr_def_domain"}});

  static SharedTopicClient shared_topic_client(
      hw, appmgr, "uart_client", 256,
      {{"chassis_data"}});
}

class CarPublisher : public rclcpp::Node {
private:
  // ros2
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;

  // send
  struct WheelMsg {
    float speed_x;
    float speed_y;
    float ang_z;
  };

  WheelMsg data={0.0f,0.0f,0.0f};

  // LibXR
  std::unique_ptr<LibXR::HardwareContainer> peripherals_;
  std::unique_ptr<LibXR::RamFS> ramfs_;
  std::unique_ptr<LibXR::LinuxUART> uart_client_;
  std::unique_ptr<LibXR::Terminal<1024, 64, 16, 128>> terminal_;
  std::unique_ptr<LibXR::Thread> term_thread_;

  LibXR::Topic wheel;
  // LibXR::Topic::Callback cb0;

public:
  explicit CarPublisher(const std::string &name);
  void send_data_callback(const geometry_msgs::msg::Twist::SharedPtr msg_data);
  void ser(WheelMsg &speed);
};

} // namespace car_pub

#endif