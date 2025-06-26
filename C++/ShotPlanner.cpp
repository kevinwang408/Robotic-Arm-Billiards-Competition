// ShotPlanner.cpp
// ===========================================================================
// Implements core shot filtering logic based on geometric interference
// checking with other balls on the table.
// ===========================================================================

#include "ShotPlanner.h"
#include "GeometryUtils.h"
#include <cmath>
#include <limits>

bool isPathObstructed(
    double x1, double y1, double x2, double y2,
    const std::vector<std::vector<double>>& obstacles,
    double bound_radius
) {
    for (const auto& obs : obstacles) {
        double obs_x = obs[0];
        double obs_y = obs[1];       
        // Calculate perpendicular distance to line (x1,y1)->(x2,y2)
        double d = dis(x2 - x1, y2 - y1, x1, y1, obs_x, obs_y);
        if ((obs_x==x2 && obs_y==y2) || (obs_x==x1 && obs_y==y1)) {
            continue;   
        }
        // If close enough to line AND within the segment bounds, it's an obstruction
        if (std::abs(d) < bound_radius) {
            double mag_target = mag(x2 - x1, y2 - y1);
            double mag_obs = mag(obs_x - x1, obs_y - y1);
            if (mag_obs < mag_target) return true;
        }
    }
    return false;
}

std::vector<std::pair<std::vector<double>, std::vector<double>>> selectClearShots(
    const std::vector<std::vector<double>>& cueballs,
    const std::vector<std::vector<double>>& holes,
    const std::vector<std::vector<double>>& childballs,
    double bound_radius
) {
    //check if there is an obstacle between childball and holes
    std::vector<std::pair<std::vector<double>, std::vector<double>>> child_hole_result;
    //check if there is an obstacle between cueball and childball
    std::vector<std::pair<std::vector<double>, std::vector<double>>> cue_child_result;
    std::vector<std::pair<std::vector<double>, std::vector<double>>> result;
    // For every childball, check all hole paths
    for (const auto& child : childballs) {
        for (const auto& hole : holes) {
            if (!isPathObstructed(child[0], child[1], hole[0], hole[1], childballs, bound_radius)) {
                child_hole_result.emplace_back(child, hole);  // Add valid shot
            }
        }
    }
     for (const auto& child : childballs) {
        for (const auto& hole : holes) {
            if (!isPathObstructed(child[0], child[1], cueballs[0][0], cueballs[0][1], childballs, bound_radius)) {
                //angle is big enough to make collision
                double angle2 = std::abs(acos(COS_VAL(child[0]-cueballs[0][0],child[1]-cueballs[0][1],hole[0]-child[0],hole[1]-child[1])) * 180 / 3.1415926);
                if (angle2 < 110 ){
                        cue_child_result.emplace_back(cueballs, child);  // Add valid shot
                    }
            }
        }
    }

    for (const auto& child_hole : child_hole_result) {
        const std::vector<double>& child_ball = child_hole.first;   
        const std::vector<double>& hole_coord = child_hole.second;  

        for (const auto& cue_child : cue_child_result) {
            const std::vector<double>& cue_ball = cue_child.first;  

            // if the child ball coordinates match the cue ball coordinates
            // (considering floating-point precision)
            // and the size of child_ball matches the size of cue_ball
            // then we can assume they are the same ball
            // This is a simplification, in practice you might want to check more conditions
            // such as distance or specific properties of the balls.
            // Here we assume child_ball is a child ball and cue_ball is the cue ball.
            // If the child ball coordinates match the cue ball coordinates
            // we can consider it a valid shot. 
            if (child_ball.size() == cue_ball.size()) {
                bool is_same = true;
                for (int i = 0; i < child_ball.size(); ++i) {
                    if (std::abs(child_ball[i] - cue_ball[i]) > 1e-9) {  // 考慮浮點數精度
                        is_same = false;
                        break;
                    }
                }

                // if the child ball matches the child ball coordinates
                // then we can consider it a valid shot
                // and add it to the result
                if (is_same) {
                    result.emplace_back(child_ball, hole_coord);
                    break;  
                }
            }
        }
    }

    return result;
}
