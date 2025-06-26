// RobotController.cpp
// ===========================================================================
// Implements robot movement and cue strike control using HRSDK API.
//
// The robot supports point-to-point (PTP), linear (LIN) motion, and digital
// I/O control for strike execution.
// ===========================================================================

#include "RobotController.h"
#include <windows.h>
#include <iostream>
#include "HRSDK.h"

// ---------------------------------------------------------------------------
// Polls the motion state of the robot arm until it completes current command.
// Required to ensure sequence safety.
// ---------------------------------------------------------------------------
void wait(HROBOT device_id) {
    while (true) {
        if (get_motion_state(device_id) == 1) break; // 1 = Motion complete
    }
}

void moveToPose(HROBOT device_id, double hit_position[6] ,double distance) {
    double pos_cueball[6] = { 0 };

    *pos_cueball = hit_position[0]; // X coordinate
    *(pos_cueball + 1) = hit_position[1]; // Y coordinate
    *(pos_cueball + 2) = hit_position[2]; // Z coordinate
    *(pos_cueball + 3) = 0; // Roll angle
    *(pos_cueball + 4) = 0; // Pitch angle
    *(pos_cueball + 5) = hit_position[5]; // Yaw           

    // Move robot using point-to-point motion (typically top-down)
    ptp_pos(device_id, 0, pos_cueball);
    wait(device_id);
    // Lower robot to final strike position using linear motion
    lin_pos(device_id, 0, 0, pos_cueball);
    wait(device_id);
}

void executeStrike(HROBOT device_id ,double distance) {
    //hit power control
    set_digital_output(device_id, 15, true);
    set_digital_output(device_id, 14, true);
    set_digital_output(device_id, 13, true);
    set_digital_output(device_id, 12, true);
    set_digital_output(device_id, 11, true);
    set_digital_output(device_id, 10, true);
    set_digital_output(device_id, 9, true);
    //categorize distance and set outputs accordingly
    std::cout << "Distance: " << distance << std::endl;
    // Set outputs based on distance ranges
    if (distance <= 100) {
        std::cout << "really close" << std::endl;
        set_digital_output(device_id, 15, true);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, false);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }else if (distance >= 100 and distance < 150) {
        std::cout << "very close" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, true);
        set_digital_output(device_id, 13, false);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }else if (distance >= 150 and distance < 175) {
        std::cout << "close" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, true);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }
    if (distance >= 175 and distance <200) {
        std::cout << "a little bit close" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, true);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }else if (distance >= 200 && distance < 250) {
        std::cout << "middle" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, true);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }else if (distance >= 250 && distance < 350) {
        std::cout << "a little bit far" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, false);
        set_digital_output(device_id, 12, true);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, false);
        set_digital_output(device_id, 9, false);
    }
    else if (distance >= 350 && distance < 450) {
        std::cout << "far" << std::endl;
        set_digital_output(device_id, 15, false);
        set_digital_output(device_id, 14, false);
        set_digital_output(device_id, 13, false);
        set_digital_output(device_id, 12, false);
        set_digital_output(device_id, 11, false);
        set_digital_output(device_id, 10, true);
        set_digital_output(device_id, 9, false);
    }
    else {
        std::cout << "really far" << std::endl;
    }

    // Use digital output 16 to activate solenoid/striker
    set_digital_output(device_id, 16, false); // Trigger ON
    Sleep(500);                               // Wait 0.5 sec
    set_digital_output(device_id, 16, true);  // Reset
    Sleep(500);                               // Wait
    set_digital_output(device_id, 16, false); // Final off
    wait(device_id);                          // Wait for any motion
}

void returnToHome(HROBOT device_id, double home_pose[6]) {

    ptp_axis(device_id, 0, home_pose);
    wait(device_id);
}
