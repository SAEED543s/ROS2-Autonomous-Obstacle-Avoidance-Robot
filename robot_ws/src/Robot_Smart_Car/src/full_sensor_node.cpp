#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include <sstream>
#include <vector>
#include <cmath>
#include <string>

// Constants
#define WHEEL_DIAMETER   0.065  // 6.5 cm
#define WHEEL_BASE       0.153  // المسافة بين العجلتين (15.3 cm)
#define TICKS_PER_REV    12.0

rclcpp::Node::SharedPtr g_node;
rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_pub;
rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr       ultrasonic_pub;
rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr          ir_pub;
rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr      odom_pub;
std::shared_ptr<tf2_ros::TransformBroadcaster>             tf_broadcaster;

// Odometry state
long   prev_left  = 0;
long   prev_right = 0;
double x     = 0.0;
double y     = 0.0;
double theta = 0.0;

std::string trim(std::string s)
{
    s.erase(0, s.find_first_not_of(" \r\n\t"));
    if (!s.empty())
        s.erase(s.find_last_not_of(" \r\n\t") + 1);
    return s;
}

void sensorCallback(const std_msgs::msg::String::SharedPtr msg)
{
    std::stringstream ss(msg->data);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, ','))
        tokens.push_back(trim(item));

    if (tokens.size() < 7) return;

    float dist       = 0;
    int   irState    = 0;
    long  leftCount  = 0;
    long  rightCount = 0;

    try {
        if (tokens[0].empty() || tokens[1].empty() ||
            tokens[2].empty() || tokens[3].empty()) return;
        dist       = std::stof(tokens[0]);
        irState    = std::stoi(tokens[1]);
        leftCount  = std::stol(tokens[2]);
        rightCount = std::stol(tokens[3]);
    } catch (...) { return; }

    // ===== 1) Joint States =====
    double leftAngle  = (leftCount  / TICKS_PER_REV) * 2.0 * M_PI;
    double rightAngle = (rightCount / TICKS_PER_REV) * 2.0 * M_PI;

    auto joint_msg = sensor_msgs::msg::JointState();
    joint_msg.header.stamp = g_node->now();
    joint_msg.name     = {"left_wheel_joint", "right_wheel_joint",
                           "casterjoint", "caster_wheel_joint"};
    joint_msg.position = {leftAngle, rightAngle, 0.0, 0.0};
    joint_pub->publish(joint_msg);

    // ===== 2) Odometry =====
    long delta_left  = leftCount  - prev_left;
    long delta_right = rightCount - prev_right;
    prev_left  = leftCount;
    prev_right = rightCount;

    double dist_left  = (delta_left  / TICKS_PER_REV) * M_PI * WHEEL_DIAMETER;
    double dist_right = (delta_right / TICKS_PER_REV) * M_PI * WHEEL_DIAMETER;
    double dist_center = (dist_left + dist_right) / 2.0;
    double delta_theta = (dist_right - dist_left) / WHEEL_BASE;

    x     += dist_center * cos(theta);
    y     += dist_center * sin(theta);
    theta += delta_theta;

    // TF: odom → base_link
    geometry_msgs::msg::TransformStamped tf;
    tf.header.stamp    = g_node->now();
    tf.header.frame_id = "odom";
    tf.child_frame_id  = "base_link";
    tf.transform.translation.x = x;
    tf.transform.translation.y = y;
    tf.transform.translation.z = 0.0;
    tf.transform.rotation.z = sin(theta / 2.0);
    tf.transform.rotation.w = cos(theta / 2.0);
    tf_broadcaster->sendTransform(tf);

    // Odometry message
    auto odom_msg = nav_msgs::msg::Odometry();
    odom_msg.header.stamp    = g_node->now();
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id  = "base_link";
    odom_msg.pose.pose.position.x = x;
    odom_msg.pose.pose.position.y = y;
    odom_msg.pose.pose.orientation.z = sin(theta / 2.0);
    odom_msg.pose.pose.orientation.w = cos(theta / 2.0);
    odom_pub->publish(odom_msg);

    // ===== 3) Ultrasonic =====
    auto ultra_msg = std_msgs::msg::Float32();
    ultra_msg.data = dist;
    ultrasonic_pub->publish(ultra_msg);

    // ===== 4) IR =====
    auto ir_msg = std_msgs::msg::Bool();
    ir_msg.data = (irState == 0);
    ir_pub->publish(ir_msg);

    printf("X: %.2f Y: %.2f Theta: %.2f | Dist: %.1f cm | IR: %s\n",
        x, y, theta, dist,
        irState == 0 ? "OBSTACLE" : "CLEAR");
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    g_node = rclcpp::Node::make_shared("full_sensor_node");

    joint_pub      = g_node->create_publisher<sensor_msgs::msg::JointState>
                        ("/joint_states", 10);
    ultrasonic_pub = g_node->create_publisher<std_msgs::msg::Float32>
                        ("/ultrasonic_distance", 10);
    ir_pub         = g_node->create_publisher<std_msgs::msg::Bool>
                        ("/ir_obstacle", 10);
    odom_pub       = g_node->create_publisher<nav_msgs::msg::Odometry>
                        ("/odom", 10);
    tf_broadcaster = std::make_shared<tf2_ros::TransformBroadcaster>(g_node);

    auto sensor_sub = g_node->create_subscription<std_msgs::msg::String>(
        "/sensor_data", 10, sensorCallback);

    printf("Full Sensor Node Started!\n");
    rclcpp::spin(g_node);
    rclcpp::shutdown();
    return 0;
}