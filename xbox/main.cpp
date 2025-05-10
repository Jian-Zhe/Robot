#include "XboxController.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    XboxController controller;
    controller.setDeadband(1200);  // 可以自己設定死區

    while (true) {
        controller.update();

        if (controller.isPressed(XboxController::DPAD_RIGHT)) {
            std::cout << "DPad Right Pressed!\n";
        }

        if (controller.isHeld(XboxController::DPAD_LEFT)) {
            std::cout << "DPad Left is being held.\n";
        }

        if (controller.isPressed(XboxController::A)) {
            std::cout << "A Pressed!\n";
        }

        if (controller.isHeld(XboxController::B)) {
            std::cout << "B is being held down.\n";
        }

        int leftX = controller.getAxisWithDeadband(XboxController::LEFT_X);
        int leftY = controller.getAxisWithDeadband(XboxController::LEFT_Y);

        int rightX = controller.getAxisWithDeadband(XboxController::RIGHT_X);
        int rightY = controller.getAxisWithDeadband(XboxController::RIGHT_Y);

        int lt = controller.getAxis(XboxController::LT);
        int rt = controller.getAxis(XboxController::RT);

        std::cout << "Left Stick: (" << leftX << ", " << leftY << ") ";
        std::cout << "Right Stick: (" << rightX << ", " << rightY << ") ";
        std::cout << "LT: " << lt << " RT: " << rt << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
