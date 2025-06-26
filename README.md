# Robotic Arm Billiard Game

This project is a Robotic Arm Billiard Game, designed to simulate and automate billiard playing using a robotic arm. The system is composed of three main modules: image processing, robotic arm path planning, and robot control.

---

## Table of Contents

- [Overview](#overview)
- [Python Module: Image Processing](#python-module-image-processing)
- [C++ Modules: Path Planning & Robot Control](#c-modules-path-planning--robot-control)
- [Getting Started](#getting-started)
- [License](#license)

---

## Overview

The Robotic Arm Billiard Game project consists of:

1. **Image Processing (Python):** Detects the billiard table, balls, and cue using computer vision techniques.
2. **Path Planning (C++):** Calculates optimal shot trajectories and robotic arm movements.
3. **Robot Control (C++):** Sends commands to the robotic arm to execute the planned shots.

---

## Python Module: Image Processing

The image processing module is responsible for detecting and locating the billiard balls and table, and converting their positions into the robot's coordinate system. The workflow consists of the following steps:

1. **Data Loading**
   - Loads the intrinsic matrix of the camera.
   - Loads the extrinsic matrix for transforming coordinates from the camera frame to the robot frame.
   - Loads the input image of the billiard table and billiard balls.

2. **Image Preprocessing**
   - Adjusts the image size, brightness, and contrast.
   - These adjustments improve the accuracy of subsequent circle detection.

3. **Circle Detection**
   - Converts the image to grayscale.
   - Applies Gaussian blur to reduce noise.
   - Uses the Hough Circle Transform to detect circles representing billiard balls.

4. **Coordinate Transformation**
   - Transforms the detected image coordinates of the billiard balls into robot coordinates using the intrinsic and extrinsic matrices.
   - Stores the transformed coordinates for use in path planning and robot control.

**Tech Stack:**  
- Language: Python  
- Libraries: (To be added, e.g. OpenCV, NumPy, etc.)  

---

## C++ Modules: Path Planning & Robot Control

These modules handle:

- **Path Planning:**  
  - Calculating collision-free paths for the robotic arm.
  - Determining the best angles and forces for successful shots.

- **Robot Control:**  
  - Interfacing with the robotic arm hardware.
  - Executing planned paths in real-time.

**Tech Stack:**  
- Language: C++  
- Libraries/Dependencies: (To be added, e.g. ROS, Eigen, hardware SDKs, etc.)  

*More details coming soon.*

---

## Getting Started

### Prerequisites

- Python (version to be specified)
- C++ compiler (version to be specified)
- Additional dependencies will be listed in respective subdirectories.

### Installation

Instructions for installation and setup for both Python and C++ components will be added soon.

---

## License

[Specify your project license here.]

---

*This README provides a high-level overview. Please check the respective directories (`python/`, `cpp/`) for module-specific documentation and setup instructions as they become available.*
