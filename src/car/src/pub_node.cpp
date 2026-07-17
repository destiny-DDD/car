#include "pub_node.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <iterator>
#include <rclcpp/logger.hpp>
#include <unistd.h>

namespace car_pub {
CarPublisher::CarPublisher(const std::string &name) : Node(name) {
  sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel", 10, [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
        this->send_data_callback(msg);
      });
  // 串口初始化
    LibXR::PlatformInit();
    peripherals_ = std::make_unique<LibXR::HardwareContainer>();
    ramfs_ = std::make_unique<LibXR::RamFS>();
    auto vid = this->declare_parameter<std::string>("vid", "16d0");
    auto pid = this->declare_parameter<std::string>("pid", "1492");
    uart_client_ = std::make_unique<LibXR::LinuxUART>(
        vid, pid, 115200, LibXR::LinuxUART::Parity::NO_PARITY, 8, 1);
    terminal_ = std::make_unique<LibXR::Terminal<1024, 64, 16,
    128>>(*ramfs_); term_thread_ = std::make_unique<LibXR::Thread>();
    term_thread_->Create(terminal_.get(),
                         LibXR::Terminal<1024, 64, 16, 128>::ThreadFun,
                         "terminal", 81900, LibXR::Thread::Priority::MEDIUM);

    topic_ = LibXR::Topic::FindOrCreate<int>("topic");

    static LibXR::HardwareContainer peripherals{
        LibXR::Entry<LibXR::RamFS>({*ramfs_, {"ramfs"}}),
        LibXR::Entry<LibXR::UART>({*uart_client_, {"uart_client"}}),
    };
}

void CarPublisher::send_data_callback(
    const geometry_msgs::msg::Twist::SharedPtr msg_data) {
  RCLCPP_INFO(this->get_logger(),"测试开启");
  cmd = {0x0A, 0x0B};
  // x
  cmd.push_back(0x00);
  cmd.push_back(0x00);
  // y
  cmd.push_back(0x00);
  cmd.push_back(0x00);
  // 角速度
  cmd.push_back(0x00);
  cmd.push_back(0x00);

  cmd.push_back(0x0C);
}

void CarPublisher::ser() {
  // LibXR::Topic::Domain tracker_domain = LibXR::Topic::Domain("tracker");
  //   std::string msg = "hello\n";
  //   uart_client_->Write(msg.data(), msg.size());
  int hello = 1;
  topic_.Publish(hello);
  sleep(1);
}
} // namespace car