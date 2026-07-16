#include "pub_node.hpp"

namespace car
{
    CarPublisher::CarPublisher(const std::string& name):Node(name)
    {
        RCLCPP_INFO(this->get_logger(),"%s初始化成功",name.c_str());
        test();
    }

    void CarPublisher::test()
    {
        RCLCPP_INFO(this->get_logger(),"测试开启");
    }
}