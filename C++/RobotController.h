// RobotController.h
// ===========================================================================
// Defines control functions to interface with the HRSDK robot arm for
// billiards striking operations. Includes:
// - Move to specified pose (PTP + LIN)
// - Trigger a strike using digital output
// - Return to home pose for reset
// ===========================================================================

#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include <vector>
#include "HRSDK.h"

// ---------------------------------------------------------------------------
// Moves the robot arm to the given Cartesian pose (x, y, z, Rx, Ry, Rz).
// This includes a point-to-point (PTP) movement and a final linear (LIN)
// movement to position the cue tip properly above the ball.
// Waits for the motion to complete before returning.
// ---------------------------------------------------------------------------
void moveToPose(HROBOT device_id, double hit_position[6] ,double distance);

// ---------------------------------------------------------------------------
// Triggers a striking action using a digital output signal.
// Sequence:
// - Digital output ON (false -> true -> false)
// - Waits between toggles to allow mechanical response
// - Waits for movement status confirmation after strike
// ---------------------------------------------------------------------------
void executeStrike(HROBOT device_id ,double distance);

// ---------------------------------------------------------------------------
// Returns the robot arm to its preconfigured home pose using axis angles.
// Waits for the home motion to complete before returning.
// ---------------------------------------------------------------------------
void returnToHome(HROBOT device_id, double home_pose[6]);

#endif // ROBOT_CONTROLLER_H
