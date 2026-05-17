#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int bluetooth_port = -1;
char last_sent_cmd = '\0'; // لحفظ آخر حرف تم إرساله لمنع التكرار المفرط

void commandCallback(const std_msgs::msg::String::SharedPtr msg) {
    if (bluetooth_port < 0) return;

    std::string cmd_str = msg->data;
    char char_cmd = '\0'; 

    if (cmd_str == "FORWARD") char_cmd = 'F';
    else if (cmd_str == "STOP_AND_TURN") char_cmd = 'V'; // حرف التفادي الذكي
    else if (cmd_str == "BACKWARD") char_cmd = 'B';
    else if (cmd_str == "LEFT") char_cmd = 'L';
    else if (cmd_str == "RIGHT") char_cmd = 'R';

    // لا ترسل الحرف إلا لو كان جديداً أو كان أمر تفادي (لتأكيد إرساله)
    if (char_cmd != '\0' && (char_cmd != last_sent_cmd || char_cmd == 'V')) {
        write(bluetooth_port, &char_cmd, 1);
        tcdrain(bluetooth_port); // إجبار الهاردوير على إرسال الحرف فوراً وعدم تأجيله
        last_sent_cmd = char_cmd;
    }
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("sensor_node");
    
    auto sensor_pub = node->create_publisher<std_msgs::msg::String>("/sensor_data", 10);
    auto cmd_sub = node->create_subscription<std_msgs::msg::String>("/cmd_move", 10, commandCallback);

    // فتح المنفذ بالإعدادات القياسية المتزامنة لضمان وصول الأوامر بدون سقوط
    bluetooth_port = open("/dev/rfcomm0", O_RDWR | O_NOCTTY);
    if (bluetooth_port < 0) {
        RCLCPP_ERROR(node->get_logger(), "Error: Couldn't open Bluetooth Port (/dev/rfcomm0)!");
        return 1;
    }

    struct termios tty;
    if(tcgetattr(bluetooth_port, &tty) != 0) return 1;
    cfsetispeed(&tty, B9600); cfsetospeed(&tty, B9600);
    tty.c_cflag &= ~PARENB;   tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;
    
    tty.c_cc[VMIN] = 1;  // انتظر حرف واحد على الأقل قبل القراءة لمنع اللوب اللانهائية بطيئة البيانات
    tty.c_cc[VTIME] = 5; 
    tcsetattr(bluetooth_port, TCSANOW, &tty);

    RCLCPP_INFO(node->get_logger(), "Bluetooth Connected successfully on /dev/rfcomm0!");

    char read_buf[1];
    std::string line = "";

    while (rclcpp::ok()) {
        int num_bytes = read(bluetooth_port, &read_buf, 1);
        
        if (num_bytes > 0) {
            if (read_buf[0] == '\n') {
                if(!line.empty()) {
                    auto message = std_msgs::msg::String();
                    message.data = line;
                    sensor_pub->publish(message);
                    line = "";
                }
            } else if (read_buf[0] != '\r') {
                line += read_buf[0];
            }
        }
        rclcpp::spin_some(node);
    }

    close(bluetooth_port);
    rclcpp::shutdown();
    return 0;
}