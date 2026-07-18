#include "pub_node.hpp"
#include "geometry_msgs/msg/twist.hpp"
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
  ramfs_ = std::make_unique<LibXR::RamFS>(); // 1a86:7523
  //ttl
  // auto vid = this->declare_parameter<std::string>("vid", "1a86");
  // auto pid = this->declare_parameter<std::string>("pid", "7523");
  //linear
  auto vid = this->declare_parameter<std::string>("vid", "16d0");
  auto pid = this->declare_parameter<std::string>("pid", "1492");
  uart_client_ = std::make_unique<LibXR::LinuxUART>(
      vid, pid, 115200, LibXR::LinuxUART::Parity::NO_PARITY, 8, 1);
  terminal_ = std::make_unique<LibXR::Terminal<1024, 64, 16, 128>>(*ramfs_);
  term_thread_ = std::make_unique<LibXR::Thread>();
  term_thread_->Create(terminal_.get(),
                       LibXR::Terminal<1024, 64, 16, 128>::ThreadFun,
                       "terminal", 81900, LibXR::Thread::Priority::MEDIUM);

  static LibXR::HardwareContainer peripherals{
      LibXR::Entry<LibXR::RamFS>({*ramfs_, {"ramfs"}}),
      LibXR::Entry<LibXR::UART>({*uart_client_, {"uart_client"}}),
  };

  // 创建Topic
  LibXR::Topic::Domain domain("libxr_def_domain");
  wheel = LibXR::Topic::CreateTopic<WheelMsg>("topic1",&domain);

  // 注册接收回调
  // cb0 = LibXR::Topic::Callback::Create(
  //     [](bool, CarPublisher *self, const WheelMsg &msg) {
  //       std::cout << msg.speed_x << " " << msg.speed_y << " " << msg.ang_z
  //                 << std::endl;
  //     }, this);
  // wheel.RegisterCallback(cb0);
}

void CarPublisher::send_data_callback(
    const geometry_msgs::msg::Twist::SharedPtr msg_data) {
  data.speed_x = msg_data->linear.x;
  data.speed_y = msg_data->linear.y;
  data.ang_z = msg_data->angular.z;
  std::cout << "pub "<<data.speed_x << " " << data.speed_y << " " << data.ang_z
                  << std::endl;
  ser(data);
}

void CarPublisher::ser(WheelMsg &speed) { wheel.Publish(speed); }

} // namespace car_pub