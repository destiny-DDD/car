#include "rclcpp/rclcpp.hpp"
#include "pub_node.hpp"

int main(int argc,char** argv)
{
    rclcpp::init(argc,argv);
    auto pub_node=std::make_shared<car::CarPublisher>("pub_node");
    rclcpp::spin(pub_node);
    rclcpp::shutdown();
    return 0;
}