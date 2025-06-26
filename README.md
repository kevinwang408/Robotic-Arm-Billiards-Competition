# Robotic Arm Billiard Game

This project is a Robotic Arm Billiard Game, designed to simulate and automate billiard playing using a robotic arm. The system is composed of three main modules: image processing, robotic arm path planning, and robot control.

---

## Table of Contents

- [Overview](#overview)
- [Python Module: Image Processing](#python-module-image-processing)
- [C++ Modules: Path Planning & Robot Control](#c-modules-path-planning--robot-control)
    - [Path Planning](#path-planning)
    - [Robot Control](#robot-control)
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

**Input image example**:

<p align="center">
  <img src="https://github.com/user-attachments/assets/0d9578e9-452b-4030-8918-fcd2dcb5171c" alt="多球員圖" width="400"/>
</p>





2. **Image Preprocessing**
    - Adjusts the image size, brightness, and contrast.
    - These adjustments improve the accuracy of subsequent circle detection.

3. **Circle Detection**
    - Converts the image to grayscale.
    - Applies Gaussian blur to reduce noise.
    - Uses the Hough Circle Transform to detect circles representing billiard balls.

**The result of circle detection**

<p align="center">
  <img src="https://github.com/user-attachments/assets/ca9772a4-4363-4533-a588-3bfc0e2234a9" alt="多球員圖" width="400"/>
</p>




4. **Coordinate Transformation**
    - Transforms the detected image coordinates of the billiard balls into robot coordinates using the intrinsic and extrinsic matrices.
    - Stores the transformed coordinates for use in path planning and robot control.

**Tech Stack:**  
- Language: Python  
- Libraries: (To be added, e.g. OpenCV, NumPy, etc.)

---

## C++ Modules: Path Planning & Robot Control

### Path Planning

The path planning module determines the optimal way for the robotic arm to play the shot, considering the positions of all balls and holes. Its main logic includes:

1. **Primary Conditions Considered**
    - **Direct Shot:** The cue ball can collide with the target billiard ball, and the target billiard ball can be directly pocketed.
    - **Reflection Shot:** If the direct shot is blocked, the cue ball may need to reflect off a wall to hit the target billiard ball, and then the target billiard ball can be directly pocketed.

2. **Obstacle Checking**
    - For both cue ball-to-billiard ball and billiard ball-to-hole paths, the algorithm checks for obstacles by calculating the shortest distance from the path to each other billiard ball.
    - If this distance is less than twice the radius (2R, where R is the radius of a billiard ball), the path is considered blocked and invalid.

3. **Angle Consideration**
    - When evaluating a path from the cue ball to the billiard ball, the algorithm also considers the angle between two vectors:
      - The vector from the cue ball to the target billiard ball.
      - The vector from the target billiard ball to the target hole.
    - This angle must be large enough to ensure that the cue ball can correctly strike the target billiard ball in a way that allows it to be pocketed. If the angle is too small, the shot is not feasible and the path is considered invalid.

4. **Path Selection**
    - If there is a valid direct path (meeting both obstacle and angle criteria), the shot is planned accordingly.
    - If all direct paths are blocked or do not meet the angle requirement, the module proceeds to evaluate the possibility of a valid reflection shot.

**direct path**

<p align="center">
  <img src="https://github.com/user-attachments/assets/70eed315-2bc7-4194-9a31-7dac8a8b59f0" alt="trajectory of refliction" width="400"/>
</p>



5. **Reflection Logic**
    - For reflection shots, the system projects the target billiard ball's coordinates across each of the four sides of the billiard table ("mirror" points).
    - For each projected (reflected) point, it checks if the path from the cue ball to the projected billiard ball is clear, and considers angle requirements as above.
    - If such a path exists (i.e., not blocked by other balls and meets the angle requirement), the reflection path is considered valid and can be used for shot planning.

**path to mirror point**

<p align="center">
  <img src="https://github.com/user-attachments/assets/cd39f32f-d8a5-4f39-9aa5-68e61dc22c90" alt="mirror point path" width="400"/>
</p>

**all the possible path**

<p align="center">
  <img src="https://github.com/user-attachments/assets/ae092947-6d6f-417f-909f-4a3d72d85394" alt="trajectory of refliction" width="400"/>
</p>






6. **Path Evaluation and Final Selection**
    - After finding all available valid paths (both direct and reflection), the module calculates the total distance for each path.
    - The total distance is defined as the sum of the distance between the cue ball and the target billiard ball, plus the distance from the target billiard ball to the target hole.
    - The path with the shortest total distance is selected as the final execution path for the robotic arm.

*More details on advanced reflection handling will be added soon.*

---

### Robot Control

The robot control module is responsible for executing the selected shot. Its main workflow is:

1. **Hit Point Calculation**
    - Uses the vector from the cue ball to the collision point on the target billiard ball to determine the precise hit point for the robotic arm.

2. **Arm Manipulation and Execution**
    - Moves the robotic arm to the assigned position based on the calculated hit point and path planning results.
    - Executes the shot by striking the cue ball at the correct location and direction.

**Tech Stack:**  
- Language: C++  
- Libraries/Dependencies: (To be added, e.g. ROS, Eigen, hardware SDKs, etc.)

---

## License

This project is licensed under the MIT License.


---

*This README provides a high-level overview. Please check the respective directories (`python/`, `cpp/`) for module-specific documentation and setup instructions as they become available.*
