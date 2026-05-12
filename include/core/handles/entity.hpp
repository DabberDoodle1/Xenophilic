#pragma once

struct Entity {
    Entity(): pos_x(0.0), pos_y(0.0), vel_x(0.0), vel_y(0.0) {}
    double pos_x;
    double pos_y;
    double vel_x;
    double vel_y;
};
