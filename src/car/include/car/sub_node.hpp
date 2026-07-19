#ifndef __SUB_NODE_HPP_
#define __SUB_NODE_HPP_

#include "SharedTopic.hpp"
// #include "SharedTopicClient.hpp"
#include "linux_uart.hpp"
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_ros/transform_broadcaster.hpp>

namespace car_sub {

static void XRobotMain(LibXR::HardwareContainer &hw) {
  using namespace LibXR;
  static ApplicationManager appmgr;

  static SharedTopic shared_topic(hw, appmgr, "uart_client", 256,
                                  {{"chassis_speed"}});

  // static SharedTopicClient shared_topic_client(
  //     hw, appmgr, "uart_client", 256,
  //     {{"topic1", "libxr_def_domain"}});
}

class CarSubscription : public rclcpp::Node {
private:
  // 参数
  std::string odom_frame_;
  std::string child_frame_;

  // 接收里程计数据
  struct WheelMsg {
    float speed_x;
    float speed_y;
    float ang_z;
  };
  struct TfData {
    float x_;
    float y_;
    float yaw_;
  };

  rclcpp::Time last_time_;
  rclcpp::Time now_time_;
  float dt;
  // 回调定时器创建
  rclcpp::TimerBase::SharedPtr timer_;
  // 初始化tf
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
  // 广播tf
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_;

  WheelMsg data = {0.0f, 0.0f, 0.0f};
  TfData change = {0.0f, 0.0f, 0.0f};

  // LibXR
  std::unique_ptr<LibXR::HardwareContainer> peripherals_;
  std::unique_ptr<LibXR::RamFS> ramfs_;
  std::unique_ptr<LibXR::LinuxUART> uart_client_;
  std::unique_ptr<LibXR::Terminal<1024, 64, 16, 128>> terminal_;
  std::unique_ptr<LibXR::Thread> term_thread_;

  std::string sub_vid_;
  std::string sub_pid_;

  LibXR::Topic wheel;
  LibXR::Topic::Callback cb0;

  void InitParameter();

public:
  explicit CarSubscription(const std::string &name,
                           const rclcpp::NodeOptions &options);
  void TimeCallback();
};

} // namespace car_sub

#endif