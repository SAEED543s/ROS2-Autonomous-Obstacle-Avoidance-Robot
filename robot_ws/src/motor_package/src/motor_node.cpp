#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <iostream>

void moveCallback(const std_msgs::msg::String::SharedPtr msg) {
    std::cout << "[MOTOR MONITOR] Actuators Status: Executing -> " << msg->data << std::endl;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("motor_node");
    auto sub = node->create_subscription<std_msgs::msg::String>("/cmd_move", 10, moveCallback);

    RCLCPP_INFO(node->get_logger(), "Motor Monitor Node Started.");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}