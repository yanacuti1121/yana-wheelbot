#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

// Common interface for the two selectable wheelbot drive backends
// (continuous-rotation servo pair vs. L298N DC gear motors). Speed is a
// normalized percentage; sign carries direction.
class MotorDriver {
public:
    virtual ~MotorDriver() = default;

    // left_speed / right_speed: -100..100 (negative = reverse for that side).
    virtual void Drive(int left_speed, int right_speed) = 0;
    virtual void Stop() = 0;
};

#endif  // MOTOR_DRIVER_H
