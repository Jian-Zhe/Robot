#include "XboxController.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <cstring>
#include <iostream>

XboxController::XboxController(const std::string& device)
    : deadband_(800), buttons_now_(16, false), buttons_prev_(16, false), axes_(8, 0),
      dpad_up_(false), dpad_down_(false), dpad_left_(false), dpad_right_(false),
      dpad_up_prev_(false), dpad_down_prev_(false), dpad_left_prev_(false), dpad_right_prev_(false) {

    fd_ = open(device.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd_ < 0) {
        perror("Failed to open joystick device");
    }
}

XboxController::~XboxController() {
    if (fd_ >= 0) {
        close(fd_);
    }
}

bool XboxController::isPressed(Button button) const {
    if (button >= 0 && button < static_cast<int>(buttons_now_.size())) {
        return buttons_now_[button] && !buttons_prev_[button];
    }
    return false;
}

bool XboxController::isHeld(Button button) const {
    if (button >= 0 && button < static_cast<int>(buttons_now_.size())) {
        return buttons_now_[button];
    }
    return false;
}

bool XboxController::isReleased(Button button) const {
    if (button >= 0 && button < static_cast<int>(buttons_now_.size())) {
        return !buttons_now_[button] && buttons_prev_[button];
    }
    return false;
}

int XboxController::getAxis(Axis axis) const {
    int value = 0;
    if (axis >= 0 && axis < static_cast<int>(axes_.size())) {
        value = axes_[axis];
    }

    // DPad方向影響左搖桿
    if (axis == LEFT_X) {
        if (dpad_right_) value = 32767;
        else if (dpad_left_) value = -32767;
    }
    else if (axis == LEFT_Y) {
        if (dpad_up_) value = -32767;
        else if (dpad_down_) value = 32767;
    }

    // DPad作為X軸或Y軸
    else if (axis == DPAD_X) {
        if (dpad_right_) value = 32767;
        else if (dpad_left_) value = -32767;
    }
    else if (axis == DPAD_Y) {
        if (dpad_up_) value = -32767;
        else if (dpad_down_) value = 32767;
    }

    return value;
}

int XboxController::getAxisWithDeadband(Axis axis) const {
    int value = getAxis(axis);

    // DPad方向影響左搖桿
    if (axis == LEFT_X) {
        if (dpad_right_) value = 32767;
        else if (dpad_left_) value = -32767;
    }
    else if (axis == LEFT_Y) {
        if (dpad_up_) value = -32767;
        else if (dpad_down_) value = 32767;
    }

    // DPad作為X軸或Y軸
    else if (axis == DPAD_X) {
        if (dpad_right_) value = 32767;
        else if (dpad_left_) value = -32767;
    }
    else if (axis == DPAD_Y) {
        if (dpad_up_) value = -32767;
        else if (dpad_down_) value = 32767;
    }

    if (value > -deadband_ && value < deadband_) {
        return 0;
    }

    return value;
}

void XboxController::setDeadband(int deadband) {
    if (deadband >= 0) {
        deadband_ = deadband;
    }
}

bool XboxController::isPressed(DPadDirection direction) const {
    switch (direction) {
        case DPAD_UP: return dpad_up_ && !dpad_up_prev_;
        case DPAD_DOWN: return dpad_down_ && !dpad_down_prev_;
        case DPAD_LEFT: return dpad_left_ && !dpad_left_prev_;
        case DPAD_RIGHT: return dpad_right_ && !dpad_right_prev_;
    }
    return false;
}

bool XboxController::isHeld(DPadDirection direction) const {
    switch (direction) {
        case DPAD_UP: return dpad_up_;
        case DPAD_DOWN: return dpad_down_;
        case DPAD_LEFT: return dpad_left_;
        case DPAD_RIGHT: return dpad_right_;
    }
    return false;
}

bool XboxController::isReleased(DPadDirection direction) const {
    switch (direction) {
        case DPAD_UP: return !dpad_up_ && dpad_up_prev_;
        case DPAD_DOWN: return !dpad_down_ && dpad_down_prev_;
        case DPAD_LEFT: return !dpad_left_ && dpad_left_prev_;
        case DPAD_RIGHT: return !dpad_right_ && dpad_right_prev_;
    }
    return false;
}

bool XboxController::update() {
    if (fd_ < 0) return false;

    buttons_prev_ = buttons_now_;
    dpad_up_prev_ = dpad_up_;
    dpad_down_prev_ = dpad_down_;
    dpad_left_prev_ = dpad_left_;
    dpad_right_prev_ = dpad_right_;

    struct js_event e;
    while (read(fd_, &e, sizeof(e)) > 0) {
        e.type &= ~JS_EVENT_INIT;

        if (e.type == JS_EVENT_BUTTON) {
            if (e.number < buttons_now_.size()) {
                buttons_now_[e.number] = (e.value != 0);
            }
        }
        else if (e.type == JS_EVENT_AXIS) {
            if (e.number < axes_.size()) {
                axes_[e.number] = e.value;
            }
        }

        // DPad方向按鈕判定
        if (e.type == JS_EVENT_AXIS) {
            if (e.number == 6) {  // DPAD_X
                dpad_left_ = (e.value < -10000);
                dpad_right_ = (e.value > 10000);
            } else if (e.number == 7) {  // DPAD_Y
                dpad_up_ = (e.value < -10000);
                dpad_down_ = (e.value > 10000);
            }
        }
    }
    return true;
}
