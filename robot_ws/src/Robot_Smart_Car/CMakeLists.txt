cmake_minimum_required(VERSION 3.8)
project(Robot_Smart_Car)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)

add_executable(full_sensor_node src/full_sensor_node.cpp)

ament_target_dependencies(full_sensor_node 
  rclcpp 
  std_msgs 
  sensor_msgs
  nav_msgs
  geometry_msgs
  tf2_ros
  tf2
)

install(TARGETS full_sensor_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY urdf launch meshes
  DESTINATION share/${PROJECT_NAME}
)

ament_package()