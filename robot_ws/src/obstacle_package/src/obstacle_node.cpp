#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <sstream>
#include <vector>
#include <iostream>

rclcpp::Publisher<std_msgs::msg::String>::SharedPtr cmd_pub;

void sensorCallback(const std_msgs::msg::String::SharedPtr msg) {
    std::string data = msg->data;
    std::stringstream ss(data);
    std::string item;
    std::vector<std::string> tokens;

    while (std::getline(ss, item, ',')) {
        tokens.push_back(item);
    }

    if (tokens.size() >= 7) { 
        long dist = std::stol(tokens[0]);
        int irState = std::stoi(tokens[1]);
        std::string mode_val = tokens[6]; 

        auto cmd_msg = std_msgs::msg::String();

        // تم زيادة المسافة لـ 60 سم لضمان وقت كافٍ للاستجابة
        if (dist < 60 || irState == 0) {
            cmd_msg.data = "STOP_AND_TURN"; 
        } else {
            cmd_msg.data = "FORWARD";
        }

        if (mode_val == "A") { // لا نرسل أوامر إلا إذا كان في الوضع التلقائي
            cmd_pub->publish(cmd_msg);
        }

        // شاشة الـ Telemetry المحدثة
        std::cout << "\033[2J\033[1;1H"; 
        std::cout << "=========================================" << std::endl;
        std::cout << "       ROBOT TELEMETRY - SAFETY MODE     " << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Current Distance    : " << dist << " cm" << std::endl;
        std::cout << "Safety Threshold    : 60 cm" << std::endl;
        std::cout << "IR Sensor State     : " << (irState == 0 ? "OBSTACLE" : "CLEAR") << std::endl;
        std::cout << "Total Distance      : " << tokens[4] << " cm" << std::endl;
        std::cout << "Current Mode        : " << (mode_val == "A" ? "AUTO" : "MANUAL") << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        std::cout << "Command Sent        : [ " << cmd_msg.data << " ]" << std::endl;
    }
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("obstacle_node");
    
    auto sub = node->create_subscription<std_msgs::msg::String>("/sensor_data", 10, sensorCallback);
    cmd_pub = node->create_publisher<std_msgs::msg::String>("/cmd_move", 10);

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}