#include "sub_node.hpp"

namespace car_sub {
CarSubscription::CarSubscription(const std::string &name,
                                 const rclcpp::NodeOptions &options)
    : Node(name, options) {
  // 串口初始化
  InitParameter();
  LibXR::PlatformInit();
  peripherals_ = std::make_unique<LibXR::HardwareContainer>();
  ramfs_ = std::make_unique<LibXR::RamFS>();
  uart_client_ = std::make_unique<LibXR::LinuxUART>(
      sub_vid_, sub_pid_, 115200, LibXR::LinuxUART::Parity::NO_PARITY, 8, 1);
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

  // odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/odom", 10);
  tf_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

  //   注册接收回调
  cb0 = LibXR::Topic::Callback::Create(
      [](bool, CarSubscription *self, const WheelMsg &msg) {
        self->data.speed_x = msg.speed_x;
        self->data.speed_y = msg.speed_y;
        self->data.ang_z = msg.ang_z;
        std::cout << "sub " << self->data.speed_x << " " << self->data.speed_y << " "
            << self->data.ang_z << std::endl;
      },
      this);
  wheel.RegisterCallback(cb0);

  last_time_ = this->now();
  // 创建时间回调 20ms一次 50hz
  timer_ =
      this->create_wall_timer(std::chrono::milliseconds(20),
                              std::bind(&CarSubscription::TimeCallback, this));
}

void CarSubscription::TimeCallback() {
  // std::cout << "sub " << data.speed_x << " " << data.speed_y << " "
  //           << data.ang_z << std::endl;
  now_time_ = this->now();
  dt = (now_time_ - last_time_).seconds();
  if (dt <= 0.0 || dt > 0.5) // 25倍时间间隔
    dt = 0.02;
  last_time_ = now_time_;
  change.yaw_ += data.ang_z * dt;
  change.x_ += (data.speed_x * std::cos(change.yaw_) -
                data.speed_y * std::sin(change.yaw_)) *
               dt;
  change.y_ += (data.speed_x * std::sin(change.yaw_) +
                data.speed_y * std::cos(change.yaw_)) *
               dt;

  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, change.yaw_);

  // auto odom_msg = nav_msgs::msg::Odometry();
  // odom_msg.header.stamp = now_time_;
  // odom_msg.header.frame_id = odom_frame_;
  // odom_msg.child_frame_id = child_frame_;
  // odom_msg.pose.pose.position.x = change.x_;
  // odom_msg.pose.pose.position.y = change.y_;
  // odom_msg.pose.pose.position.z = 0.0;
  // odom_msg.pose.pose.orientation.x = q.x();
  // odom_msg.pose.pose.orientation.y = q.y();
  // odom_msg.pose.pose.orientation.z = q.z();
  // odom_msg.pose.pose.orientation.w = q.w();

  auto transform = geometry_msgs::msg::TransformStamped();
  transform.header.stamp = now_time_;
  transform.header.frame_id = odom_frame_;
  transform.child_frame_id = child_frame_;
  transform.transform.translation.x = change.x_;
  transform.transform.translation.y = change.y_;
  transform.transform.translation.z = 0.0;
  transform.transform.rotation.x = q.x();
  transform.transform.rotation.y = q.y();
  transform.transform.rotation.z = q.z();
  transform.transform.rotation.w = q.w();

  tf_->sendTransform(transform);
}
// 参数
void CarSubscription::InitParameter() {
  sub_vid_ = this->declare_parameter<std::string>("vid", "1a86");
  sub_pid_ = this->declare_parameter<std::string>("pid", "7523");
  odom_frame_ = this->declare_parameter("odom_frame", "odom");
  child_frame_ = this->declare_parameter("child_frame", "base_footprint");

  std::cout << "Get parameter: " << "sub_vid: " << sub_vid_
            << " sub_pid: " << sub_pid_ << " odom_frame: " << odom_frame_
            << " child_frame: " << child_frame_ << '\n';
}

} // namespace car_sub