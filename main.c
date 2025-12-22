#define GL_SILENCE_DEPRECATION

#include "gl_dummy_bleh.h"
#include "utils.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

typedef struct {
    float x; // Top-Left X Coordinate
    float y; // Top-Left Y Coordinate
    float w; // Width
    float h; // Height
} Rect;

// Clear the window and load background
void clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

// Swap Buffers and poll for event inputs
void swap_and_poll(GLFWwindow* window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Exit and terminate window process
void window_exit(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Draw a Triangle (Alpha)
void draw_triangle(float alpha) {
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.0f, 0.0f, alpha);
        glVertex2f(-0.5f * (1 + alpha * 0.5f), -0.5f * (1 + alpha * 0.5f));

        glColor4f(0.0f, 1.0f, 0.0f, alpha);
        glVertex2f(0.5f * (1 + alpha * 0.5f), -0.5f * (1 + alpha * 0.5f));

        glColor4f(0.0f, 0.0f, 1.0f, alpha);
        glVertex2f(0.0f * (1 + alpha * 0.5f), 0.5f * (1 + alpha * 0.5f));
    glEnd();
}

// Draw a Rectangle as an outline — border offset (X-Position, Y-Position, Width, Height; Red, Green, Blue, Alpha)
void draw_rectangle_outline(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x,         y);
        glVertex2f(x + w,     y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
}

// Draw a Rectangle from Rectangle Object (Rect Object; Red, Green, Blue, Alpha)
void draw_rectangle(Rect rect, float r, float g, float b, float a) {
    glBegin(GL_TRIANGLES);
        glColor4f(r, g, b, a); glVertex2f(rect.x, rect.y);
        glColor4f(r, g, b, a); glVertex2f(rect.x + rect.w, rect.y);
        glColor4f(r, g, b, a); glVertex2f(rect.x + rect.w, rect.y + rect.h);

        glColor4f(r, g, b, a); glVertex2f(rect.x, rect.y);
        glColor4f(r, g, b, a); glVertex2f(rect.x + rect.w, rect.y + rect.h);
        glColor4f(r, g, b, a); glVertex2f(rect.x, rect.y + rect.h);
    glEnd();
}

// Return if the mouse is over an element (Rectangle, Mouse-X, Mouse-Y)
int is_mouse_over(Rect rect, float mx, float my) {
    return mx >= rect.x && mx <= rect.x + rect.w && my >= rect.y && my <= rect.y + rect.h;
}

// Fading in the screen
void fade_in_screen(GLFWwindow* window) {
    float counter = -0.5f;

    while (!glfwWindowShouldClose(window) && counter < 0.3f) {
        clear(
            clamp(counter, 0.0f, 0.2f), 
            clamp(counter, 0.0f, 0.2f), 
            clamp(counter, 0.0f, 0.2f), 
            1.0f
        );
        swap_and_poll(window);
        counter += 0.02f;
        usleep(32000);
    }
}

// Loading screen; kewl triangle
void loading_screen(GLFWwindow* window) {
    float alpha = 0.0f;
    int fadeIn = 1;
    int loading_done = 0;

    while (!glfwWindowShouldClose(window) && !loading_done) {
        clear(0.2f, 0.2f, 0.2f, 1.0f);
        draw_triangle(alpha);

        // Update alpha
        if (fadeIn) alpha += 0.1f;
        else alpha -= 0.1f;

        if (alpha >= 1.0f) {
            fadeIn = 0;
            usleep(1000000);
        }
        if (alpha <= 0.0f && !fadeIn) loading_done = 1;

        swap_and_poll(window);
        usleep(25000);
    }
    alpha = 1.0f;
}

void options_menu(GLFWwindow* window) {
    ;
}


int main() {
    float border = 0.01f;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to intialize GLFW");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwInitHint(GLFW_PLATFORM_COCOA, GLFW_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glfwCreateCursor()

    fade_in_screen(window);
    loading_screen(window);
    usleep(1000000);

    int should_exit = 0;
    int playing = 0;

    int left_down_last_frame = 0;

    Rect playButton =    {-0.5f, 0.0f, 1.00f, 0.30f};
    Rect optionsButton = {-0.5f, -0.35f, 1.00f, 0.30f};
    Rect exitButton =    {-0.5f, -0.7f, 1.00f, 0.30f};

    while (!glfwWindowShouldClose(window) && !should_exit) {
        clear(0.2f, 0.2f, 0.2f, 1.0f);

        int selected = -1;

        int left_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        double mouse_x_pixels, mouse_y_pixels;
        glfwGetCursorPos(window, &mouse_x_pixels, &mouse_y_pixels);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        float mouse_x = (float)(mouse_x_pixels / width) * 2.0f - 1.0f;
        float mouse_y = 1.0f - (float)(mouse_y_pixels / height) * 2.0f;

        if (!playing) {
            /* Check Hover & Clicks */
            // If Play Button is Pressed
            if (is_mouse_over(playButton, mouse_x, mouse_y)) {
                if (left_down && !left_down_last_frame) playing = 1;
                selected = 0;
            }

            // If Options Button is Pressed
            if (is_mouse_over(optionsButton, mouse_x, mouse_y)) {
                if (left_down && !left_down_last_frame) options_menu(window);
                selected = 1;
            }

            // If Exit Button is Pressed
            if (is_mouse_over(exitButton, mouse_x, mouse_y)) {
                if (left_down && !left_down_last_frame) should_exit = 1;
                selected = 2;
            }

            /* Draw Menu */
            // Play
            draw_rectangle(playButton, 0.5f, 0.5f, 0.5f, 1.0f);
            draw_rectangle_outline(-0.5f - border, 0.0f - border, 1.0f + border * 2, 0.3f + border * 2, 
                selected == 0 ? 1.0f : 0.5f,
                selected == 0 ? 1.0f : 0.5f,
                selected == 0 ? 1.0f : 0.5f,
                1.0f
            );

            // Options
            draw_rectangle(optionsButton, 0.5f, 0.5f, 0.5f, 1.0f);
            draw_rectangle_outline(-0.5f - border,     -0.35f - border, 1.0f + border * 2, 0.3f + border * 2, 
                selected == 1 ? 1.0f : 0.5f,
                selected == 1 ? 1.0f : 0.5f,
                selected == 1 ? 1.0f : 0.5f,
                1.0f
            );

            // Exit
            draw_rectangle(exitButton, 0.5f, 0.5f, 0.5f, 1.0f);
            draw_rectangle_outline(-0.5f - border, -0.7f - border, 1.0f + border * 2, 0.3f + border * 2, 
                selected == 2 ? 1.0f : 0.5f,
                selected == 2 ? 1.0f : 0.5f,
                selected == 2 ? 1.0f : 0.5f,
                1.0f
            );
        } else {
            // Gameplay Logic
            printf("Playing");
        }

        left_down_last_frame = left_down;        

        swap_and_poll(window);
    }

    window_exit(window);
}
