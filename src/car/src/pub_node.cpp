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
  ramfs_ = std::make_unique<LibXR::RamFS>(); // 1a86:7523
  auto vid = this->declare_parameter<std::string>("vid", "1a86");
  auto pid = this->declare_parameter<std::string>("pid", "7523");
  uart_client_ = std::make_unique<LibXR::LinuxUART>(
      vid, pid, 115200, LibXR::LinuxUART::Parity::NO_PARITY, 8, 1);
  terminal_ = std::make_unique<LibXR::Terminal<1024, 64, 16, 128>>(*ramfs_);
  term_thread_ = std::make_unique<LibXR::Thread>();
  term_thread_->Create(terminal_.get(),
                       LibXR::Terminal<1024, 64, 16, 128>::ThreadFun,
                       "terminal", 81900, LibXR::Thread::Priority::MEDIUM);

  // topic_ = LibXR::Topic::FindOrCreate<std::array<char, 5>>("topic");
  // r_topic_ = LibXR::Topic::FindOrCreate<std::array<char, 5>>("topic");
  // void (*r_cb_fun)(bool, CarPublisher *self, LibXR::RawData &data) =
  //     [](bool, CarPublisher *self, LibXR::RawData &data) {
  //       auto str = reinterpret_cast<std::array<char, 5> *>(data.addr_);
  //       // std::cout << (*str)[0] << (*str)[1] << (*str)[2] << (*str)[3]
  //       //           << (*str)[4] << '\n';
  //       self->ser();
  //     };
  // auto r_cb = LibXR::Topic::Callback::Create(r_cb_fun, this);
  // r_topic_.RegisterCallback(r_cb);

  static LibXR::HardwareContainer peripherals{
      LibXR::Entry<LibXR::RamFS>({*ramfs_, {"ramfs"}}),
      LibXR::Entry<LibXR::UART>({*uart_client_, {"uart_client"}}),
  };
  //创建Topic
  LibXR::Topic::Domain domain("first");
  wheel=LibXR::Topic::CreateTopic<std::vector<uint8_t>>("wheel",&domain);
}

void CarPublisher::send_data_callback(
    const geometry_msgs::msg::Twist::SharedPtr msg_data) {
  RCLCPP_INFO(this->get_logger(), "测试开启");
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

  ser(cmd);
}

void CarPublisher::ser(const std::vector<uint8_t>& cmd) {
  // // LibXR::Topic::Domain tracker_domain = LibXR::Topic::Domain("tracker");
  // //   std::string msg = "hello\n";
  // //   uart_client_->Write(msg.data(), msg.size());
  // int hello = 1;
  // std::array<char, 5> data{'h', 'e', 'l', 'l', 'o'};
  // sleep(1);
  // topic_.Publish(data);
  //发布Topic
  wheel.Publish(cmd);

}
} // namespace car_pub