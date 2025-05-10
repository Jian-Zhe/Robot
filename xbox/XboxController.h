#pragma once

#include <string>
#include <vector>

class XboxController {
public:
    enum Button {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LB = 4,
        RB = 5,
        BACK = 6,
        START = 7,
        XBOX = 8,
        LEFT_STICK = 9,
        RIGHT_STICK = 10
    };

    enum Axis {
        LEFT_X = 0,
        LEFT_Y = 1,
        LT = 2,
        RIGHT_X = 3,
        RIGHT_Y = 4,
        RT = 5,
        DPAD_X = 6,  // 用來處理 DPad 左右
        DPAD_Y = 7   // 用來處理 DPad 上下
    };

    enum DPadDirection {
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT
    };

    explicit XboxController(const std::string& device = "/dev/input/js0");
    ~XboxController();

    bool isPressed(Button button) const;
    bool isHeld(Button button) const;
    bool isReleased(Button button) const;
    int getAxis(Axis axis) const;
    int getAxisWithDeadband(Axis axis) const;

    void setDeadband(int deadband);

    // DPad方向
    bool isPressed(DPadDirection direction) const;
    bool isHeld(DPadDirection direction) const;
    bool isReleased(DPadDirection direction) const;

    bool update();

private:
    int fd_;
    int deadband_;
    std::vector<bool> buttons_now_;
    std::vector<bool> buttons_prev_;
    std::vector<int> axes_;

    bool dpad_up_, dpad_down_, dpad_left_, dpad_right_;
    bool dpad_up_prev_, dpad_down_prev_, dpad_left_prev_, dpad_right_prev_;
};
