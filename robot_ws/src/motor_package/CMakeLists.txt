cmake_minimum_required(VERSION 3.8)
project(motor_package)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# استدعاء الحزم المطلوبة من ROS 2
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# تعريف الـ Executable وربطه بملف السورس
add_executable(motor_node src/motor_node.cpp)

# ربط المكتبات والاعتمادات بالـ Executable
ament_target_dependencies(motor_node rclcpp std_msgs)

# تحديد مسار التثبيت للـ Node
install(TARGETS
  motor_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()