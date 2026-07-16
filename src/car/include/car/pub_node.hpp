#ifndef __PUB_NODE_HPP_
#define __PUB_NODE_HPP_

#include <rclcpp/rclcpp.hpp>

namespace car
{

class CarPublisher:public rclcpp::Node
{
public:
    explicit CarPublisher(const std::string& name);
    void test();
private:

};

}

#endif