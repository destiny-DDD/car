#include "sub_node.hpp"

namespace car_sub {
CarSubscription::CarSubscription(const std::string &name) : Node(name) {
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
  static LibXR::HardwareContainer peripherals{
      LibXR::Entry<LibXR::RamFS>({*ramfs_, {"ramfs"}}),
      LibXR::Entry<LibXR::UART>({*uart_client_, {"uart_client"}}), 
    };

  // LibXR::Topic::Domain domain("libxr_def_domain");
  wheel = LibXR::Topic::CreateTopic<WheelMsg>("chassis_data");

  XRobotMain(peripherals);

//   注册接收回调
  cb0 = LibXR::Topic::Callback::Create(
      [](bool, CarSubscription *self, const WheelMsg &msg) {
        std::cout <<"sub "<< msg.speed_x << " " << msg.speed_y << " " << msg.ang_z
                  << std::endl;
      },
      this);
  wheel.RegisterCallback(cb0);
}
} // namespace car_sub