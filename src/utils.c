#include "utils.h"
#include <unistd.h>

#include <GLFW/glfw3.h>

float clamp(float value, float min, float max) {
    if (value < min) return min;
    else if (value > max) return max;
    else return value;
}

// void sleep(int microseconds) {
//     usleep(microseconds * glfwGetTime());
// }