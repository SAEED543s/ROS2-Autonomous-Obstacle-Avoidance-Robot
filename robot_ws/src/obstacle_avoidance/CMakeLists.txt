cmake_minimum_required(VERSION 3.8)
project(obstacle_avoidance)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -std=c++17)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)

# 1) Full Sensor Node
add_executable(full_sensor_node src/full_sensor_node.cpp)
ament_target_dependencies(full_sensor_node rclcpp std_msgs sensor_msgs nav_msgs tf2_ros)

# 2) Obstacle Node
add_executable(obstacle_node src/obstacle_node.cpp)
ament_target_dependencies(obstacle_node rclcpp std_msgs)

# 3) Motor Node
add_executable(motor_node src/motor_node.cpp)
ament_target_dependencies(motor_node rclcpp std_msgs)

# Install targets
install(TARGETS
  full_sensor_node
  obstacle_node
  motor_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
