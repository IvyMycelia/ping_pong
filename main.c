#define GL_SILENCE_DEPRECATION

#include "gl_dummy_bleh.h"
#include "utils.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


typedef struct {
    float x, y;
    float w, h;
} Paddle;

typedef struct {
    float x, y;
    float radius;
    float vx, vy;
} Ball;

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
    
    glfwInitHint(GLFW_PLATFORM_COCOA, GLFW_TRUE);
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

    Paddle leftPaddle = {-0.9f, -0.15f, 0.05f, 0.3f};
    Paddle rightPaddle = {0.85f, -0.15f, 0.05f, 0.3f};
    Ball ball = {0.0f, 0.0f, 0.03f, (rand() % 2 ? 0.01f : -0.01f), (rand() % 2 ? 0.015f : -0.015f)};

    while (!glfwWindowShouldClose(window) && !should_exit) {
        clear(0.2f, 0.2f, 0.2f, 1.0f);

        int selected = -1;

        int left_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        double mouse_x_pixels, mouse_y_pixels;
        glfwGetCursorPos(window, &mouse_x_pixels, &mouse_y_pixels);
        
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float scale_x = (float)fb_width / width;
        float scale_y = (float)fb_height / height;

        float mouse_x_fb = mouse_x_pixels * scale_x;
        float mouse_y_fb = mouse_y_pixels * scale_y;

        float mouse_x = (float)(mouse_x_fb / fb_width) * 2.0f - 1.0f;
        float mouse_y = 1.0f - (float)(mouse_y_fb / fb_height) * 2.0f;

        if (!playing) {
            /* Check Hover & Clicks */
            // If Play Button is Pressed
            if (is_mouse_over(playButton, mouse_x, mouse_y)) {
                if (left_down && !left_down_last_frame) playing = 1;
                selected = 0;
            }

            // If Options Button is Pressed
            // if (is_mouse_over(optionsButton, mouse_x, mouse_y)) {
            //     if (left_down && !left_down_last_frame) options_menu(window);
            //     selected = 1;
            // }

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
            // draw_rectangle(optionsButton, 0.5f, 0.5f, 0.5f, 1.0f);
            // draw_rectangle_outline(-0.5f - border,     -0.35f - border, 1.0f + border * 2, 0.3f + border * 2, 
            //     selected == 1 ? 1.0f : 0.5f,
            //     selected == 1 ? 1.0f : 0.5f,
            //     selected == 1 ? 1.0f : 0.5f,
            //     1.0f
            // );

            // Exit
            draw_rectangle(exitButton, 0.5f, 0.5f, 0.5f, 1.0f);
            draw_rectangle_outline(-0.5f - border, -0.7f - border, 1.0f + border * 2, 0.3f + border * 2, 
                selected == 2 ? 1.0f : 0.5f,
                selected == 2 ? 1.0f : 0.5f,
                selected == 2 ? 1.0f : 0.5f,
                1.0f
            );
        } else {
            // Exit Back to Main Menu
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) playing = 0;


            /* Gameplay Logic */

            // Move Paddles
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) leftPaddle.y += 0.02f;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) leftPaddle.y -= 0.02f;
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) rightPaddle.y += 0.02f;
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) rightPaddle.y -= 0.02f;
            // Clamp Paddles
            if (leftPaddle.y < -1.0f) leftPaddle.y = -1.0f;
            if (leftPaddle.y + leftPaddle.h > 1.0f) leftPaddle.y = 1.0f - leftPaddle.h;
            if (rightPaddle.y < -1.0f) rightPaddle.y = -1.0f;
            if (rightPaddle.y + rightPaddle.h> 1.0f) rightPaddle.y = 1.0f - rightPaddle.h;


            // Move Ball
            ball.x += ball.vx;
            ball.y += ball.vy;


            // Bounce off Top / Bottom
            if (ball.y + ball.radius >= 1.0f || ball.y - ball.radius <= -1.0f) ball.vy *= -1;


            /* Bounce off Paddles */

            // Left Paddle
            if (ball.x - ball.radius <= leftPaddle.x + leftPaddle.w &&
                ball.y >= leftPaddle.y && ball.y <= leftPaddle.y + leftPaddle.h) {
                    ball.vx *= -1.0f;
                    ball.x = leftPaddle.x + leftPaddle.w + ball.radius; // Prevent Sticking
            }
            // Right Paddle
            if (ball.x + ball.radius >= rightPaddle.x &&
                ball.y >= rightPaddle.y && ball.y <= rightPaddle.y + rightPaddle.h) {
                    ball.vx *= -1.0f;
                    ball.x = rightPaddle.x - ball.radius; // Prevent Sticking
            }


            // Reset if Ball goes too far Left / Right
            if (ball.x < -1.1f || ball.x > 1.1f) {
                ball.x = ball.y = 0.0f;
                ball.vx = (rand() % 2 ? 0.01f : -0.01f);
                ball.vy = ball.vx = (rand() % 2 ? 0.015f : -0.015f);
            }


            // Draw Paddles
            draw_rectangle((Rect){leftPaddle.x, leftPaddle.y, leftPaddle.w, leftPaddle.h}, 1.0f, 1.0f, 1.0f, 1.0f);
            draw_rectangle((Rect){rightPaddle.x, rightPaddle.y, rightPaddle.w, rightPaddle.h}, 1.0f, 1.0f, 1.0f, 1.0f);

            
            // Draw Ball (Square lol)
            draw_rectangle((Rect){ball.x - ball.radius, ball.y - ball.radius, ball.radius * 2, ball.radius * 2}, 1.0f, 0.1f, 0.1f, 1.0f);
        }

        left_down_last_frame = left_down;        

        swap_and_poll(window);
    }

    window_exit(window);
}
