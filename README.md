# Autonomous Obstacle Avoidance Mobile Robot (ROS 2 Humble)

A professional ROS 2 Humble and C++ implementation of an Autonomous Mobile Robot (AMR) designed for reactive obstacle avoidance, hardware-software co-design, and real-time environment visualization via RViz2.

## 📌 Project Overview
This project focuses on the development of a differential-drive mobile robot capable of navigating dynamic environments safely. By utilizing an **Arduino Mega 2560** connected to an array of **HC-SR04 Ultrasonic Sensors**, the robot scans its surroundings, streams distance metrics over high-speed serial communication, and utilizes a robust **ROS 2 C++ architecture** to compute velocity commands (`geometry_msgs/msg/Twist`) for reactive obstacle avoidance.

## 🛠️ Tech Stack & System Architecture
- **Robotics Framework:** ROS 2 Humble Elite
- **Operating System:** Ubuntu 22.04 LTS (Tested on Lenovo IdeaPad Gaming 3)
- **Languages:** Modern C++ (C++17), Arduino Sketch (C/C++)
- **Simulation & Visualization:** RViz2 (Robot Visualizer)
- **Main Hardware:** Arduino Mega 2560, HC-SR04 Ultrasonic Sensors, Chassis Motors & Drivers

## 📂 Repository Structure
```text
├── robot_ws/                    # Main ROS 2 Workspace
│   └── src/
│       ├── obstacle_avoidance/  # Custom ROS 2 C++ Package
│       │   ├── src/
│       │   │   ├── robot_controller.cpp  # Logic for obstacle avoidance & Twist commands
│       │   │   └── sensor_subscriber.cpp # Reads distance topics
│       │   ├── CMakeLists.txt
│       │   └── package.xml
├── arduino_firmware/            # Embedded firmware for Arduino Mega 2560
│   └── Multi_Sensor_Read.ino    # Trigger/Echo timing logic & Serial broadcasting
├── media/                       # Rviz2 simulation videos, screenshots, and robot photos
└── docs/
    ├── Final_Project_Report.pdf # Full Academic Graduation Technical Report (Group 6)
    └── Project_Presentation.pdf # Technical presentation slides

