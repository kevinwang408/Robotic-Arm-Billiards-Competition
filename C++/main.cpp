// main.cpp
// ===========================================================================
// Main entry point for robotic billiards system.
// This program loads position data, evaluates shot opportunities, and commands
// a robotic arm to execute a cue strike using either direct or flip shots.
//
// Flow Summary:
// 1. Read CSV inputs (ball positions, wall positions, hole positions)
// 2. Determine valid direct child ball-to-hole shots (using ShotPlanner)
// 3. If none are available, use wall bounce logic (FlipPlanner)
// 4. Select best shot by shortest distance
// 5. Command robot to strike
// ===========================================================================

#include <iostream>
#include "FileIOUtils.h"
#include "ShotPlanner.h"
#include "FlipPlanner.h"
#include "RobotController.h"
#include "GeometryUtils.h"
#include "HRSDK.h"
#include "limits"
void __stdcall callBack(uint16_t, uint16_t, uint16_t*, int) {};

int main() {
    // Connect to robot controller (assumes HRSDK environment setup)
    
    HROBOT device_id = open_connection("169.254.148.16", 1, callBack);
    if (device_id < 0) {
        std::cerr << "Failed to connect to robot controller." << std::endl;
        return -1;
    }

    // Load all required input data from CSV
    std::vector<std::vector<double>> cueball = loadCSV2D("csv/cueball.csv", 2);   // cueball[0] = mother ball
    std::vector<std::vector<double>> childballs = loadCSV2D("csv/childball.csv", 2);
    std::vector<std::vector<double>> holes = loadCSV2D("csv/holes.csv", 2);
    std::vector<std::vector<double>> walls = loadCSV2D("csv/walls.csv", 2);
    int ball_count = loadSingleInt("csv/ballcount.csv");

    // Generate all possible direct shots
    auto valid_shots = selectClearShots(cueball, holes, childballs, 15);

    std::vector<double> target_ball;
    std::vector<double> target_hole;
    double total_distance = 0;
    // Select best direct shot (by shortest distance)
    if (!valid_shots.empty()) {
        double min_dist = std::numeric_limits<double>::max();
        for (const auto& shot : valid_shots) {
            double dx = shot.first[0] - shot.second[0];
            double dy = shot.first[1] - shot.second[1];
            double cue_dx = cueball[0][0] - shot.first[0];
            double cue_dy = cueball[0][1] - shot.first[1];
            double dist = mag(dx, dy)+ mag(cue_dx, cue_dy);
            if (dist < min_dist) {
                min_dist = dist;
                target_ball = shot.first;
                target_hole = shot.second;
                total_distance = dist;
            }
        }
        std::cout << "Selected direct shot.";
    } else {
        // If no direct shot is valid, try flip shots (bank shots)
        auto flip_shots = evaluateFlipShots(cueball[0], childballs, childballs, walls, 15);

        if (!flip_shots.empty()) {
            FlipShot best = flip_shots[0];
            for (const auto& fs : flip_shots) {
                if (fs.total_distance < best.total_distance) {
                    best = fs;
                    total_distance= fs.total_distance;
                }
            }
            target_ball = best.target_coords;
            target_hole = best.hole_coords;
            std::cout << "Selected flip shot via wall.";
        } else {
            std::cerr << "No available shots (direct or flip).";
            return -1;
        }
    }
    // Prepare robot for strike
    double origin_point[6] = { 90,0,0,0,-90,0 };
    double hit_position[6] = {0};
    // Calculate hit position based on target ball and hole coordinates
    double rel_x = target_hole[0] - target_ball[0];
    double rel_y = target_hole[1] - target_ball[1];
    double rel_dis = sqrt(pow(rel_x, 2) + pow(rel_y, 2));
    double vector_x = rel_x / rel_dis; // Unit vector x-component
    double vector_y = rel_y / rel_dis; // Unit vector y-component   
    double hit_x=cueball[0][0] + vector_x * (15 + 3); // Add some offset for the cue ball
    double hit_y=cueball[0][1] + vector_y * (15 + 3); // Add some offset for the cue ball
    double z = 0; // Assuming flat surface, z-coordinate is 0
    hit_position[0] = hit_x;
    hit_position[1] = hit_y;
    hit_position[2] = z;
    hit_position[3] = 0; // Roll angle
    hit_position[4] = 0; // Pitch angle
    // Calculate the angle of the hit direction
    double angle[3] = { vector_x, vector_y, 0 };
    double vector_TCP[3] = { 0,-1,0 };
    double inner_product = 0;
    double sum = 0;
    for (int i = 0; i < 3; i++)
    {
        sum = angle[i] * vector_TCP[i];
        inner_product += sum;
    }
    // Calculate the angle in degrees
    double theta = abs(acos(inner_product))* 180 / M_PI;;
    if (vector_x>0){
        hit_position[5] = -90+theta; // Yaw angle (facing downwards
    } else {
        hit_position[5] = -90-theta; // Facing left
    } 
    // Define a target robot pose manually or via mapping (hardcoded here)
    moveToPose(device_id, hit_position,total_distance);      // Move to position
    executeStrike(device_id,total_distance);         // Strike the ball
    returnToHome(device_id, origin_point);  // Reset to home pose

    disconnect(device_id); // Disconnect from robot
    return 0;
}
