#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <random>
#include <ctime>
#include <windows.h>

#include "shader.hpp"
#include "timer.h"
#include "lodepng.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

using namespace std;

void handleWindowTitle();
void handleHoldKeyEvents(double dt);
void keyCallback(GLFWwindow *, int key, int scancode, int action, int mode);
void genPolygon(int n);
int save_screenshoot(const char *filename);
string to_string(double n, int d = 2);
void updatePoly();
GLfloat rand(GLfloat a, GLfloat b);
void frameBufferSizeCallback(GLFWwindow *, int width, int height);

template<class C>
C lerp(C a, C b, float t) {
    return a + (b - a) * t;
}


static const char *WINDOW_TITLE = "Polycursion";
static const int WINDOW_WIDTH  = 1280, WINDOW_HEIGHT = 760;
static const double shift = 2, zoom_add = 2;  // Per second
const double pi = 3.14159265358979323846264338327950288419716939937510582;
const int INF = 1e9;

GLFWwindow *window = nullptr;
default_random_engine generator;

GLuint vvbo, cvbo, vao;  // Vertex VBO, Color VBO
vector<glm::vec2> pts;   // Vertex data
vector<glm::vec3> colors;
int steps = 1, MAX_STEPS = 1000;
int NGON = 22;
double zoom = 1;
double shx = 0, shy = 0;
int drawn = 0;

Timer msg_t(2);
int input_type;
bool wait_input = false;
string input_str;
enum InputType {
    IT_IDLE, IT_POLYSIDE, IT_MESSAGE, IT_MSTEPS
};


int main() {
    // CORE INITIALIZATION
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 0, 0);
	if (!window) cerr << "Use ur FOKING GPU!!!\n";
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) cerr << "GLEW is dead\n";
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // INIT SHADERS
    ShaderProgram prg;
    prg.loadFromFile("vert", "frag");

    // INIT DATA
    genPolygon(NGON);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vvbo);
    glGenBuffers(1, &cvbo);
    updatePoly();

    // INIT VETEX ATTRIBUTES (according to vert)
    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // APP CYCLE
    Timer spf(1.0/60), fps_upd(1), ft;  // Frame management
    int frames = 0;
    while (!glfwWindowShouldClose(window)) {
        // Fixed framerate
        if (steps >= MAX_STEPS && !spf.tick())
            Sleep(spf.left() * 1e3), spf.refresh();

        // Message handling
        if (input_type == IT_MESSAGE && msg_t.tick())
            input_type = IT_IDLE;

        double dt = ft.elapsed();
        ft.refresh();
        handleHoldKeyEvents(dt);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // PROGRESS
        if (steps < MAX_STEPS) {
            steps++;
            for (int i = 0; i < NGON - 1; i++) {
                pts.push_back(lerp(pts[pts.size() - NGON], pts[pts.size() - NGON + 1], 0.2));
            }
            pts.push_back(lerp(pts[pts.size() - NGON], pts[pts.size() - 2 * NGON + 1], 0.2));

            for (int i = 0; i < NGON; i++)
                colors.emplace_back(rand(0, 1), rand(0, 1), rand(0, 1));

            updatePoly();
        }

        // DRAW
        glBindVertexArray(vao);
        use(&prg);
        prg.setFloat("zoom", zoom);
        prg.setFloat("shx", shx);
        prg.setFloat("shy", shy);
        for (int i = 0; i < min(MAX_STEPS, steps); i++) {
            glDrawArrays(GL_TRIANGLE_STRIP, i * NGON, NGON);
        }
        glBindVertexArray(0);
        use();

        // FINISH OFF
        glfwPollEvents();
        glfwSwapBuffers(window);

        handleWindowTitle();

        frames++;
        if (fps_upd.tick())
            drawn = frames, frames = 0;
    }

    // -FATALITY-
    glfwTerminate();
    return 0;
}

void handleWindowTitle() {
    string title;
    if (input_type == IT_IDLE) {
        title = (string)WINDOW_TITLE + " " + to_string(100 * steps / MAX_STEPS) + "% [" + to_string(drawn) + "fps] | Zoom " + to_string(zoom, 2) + "x | " + to_string(NGON) + "-gon | Depth " + to_string(steps) + "/" + to_string(MAX_STEPS);
    } else if (input_type == IT_POLYSIDE) {
        title = "Generate " + input_str + "-gon";
    } else if (input_type == IT_MESSAGE) {
        title = input_str;
    } else if (input_type == IT_MSTEPS) {
        title = "No. steps to perform = " + input_str;
    }
    glfwSetWindowTitle(window, title.c_str());
}

void handleHoldKeyEvents(double dt) {
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            zoom *= pow(zoom_add, dt);
        } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            zoom /= zoom > 1 ? pow(zoom_add, min(dt, log(zoom) / log(zoom_add))) : 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            shx += dt * shift / zoom;
        } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            shx -= dt * shift / zoom;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            shy -= dt * shift / zoom;
        } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            shy += dt * shift / zoom;
        }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        if (wait_input) {
                if (GLFW_KEY_0 <= key && key <= GLFW_KEY_9) {
                    input_str.push_back(key);
                } else if (GLFW_KEY_KP_0 <= key && key <= GLFW_KEY_KP_9) {
                    input_str.push_back(key - GLFW_KEY_KP_0 + '0');
                } else if (!input_str.empty() && key == GLFW_KEY_BACKSPACE) {
                    input_str.pop_back();
                } else if (key == GLFW_KEY_ENTER) {
                    if (input_type == IT_POLYSIDE && !input_str.empty()) {
                        NGON = stoi(input_str);
                        genPolygon(NGON);
                        steps = 1;
                    } else if (input_type == IT_MSTEPS && !input_str.empty()) {
                        MAX_STEPS = stoi(input_str);
                    }
                    wait_input = false;
                    input_type = IT_IDLE;
                }
        } else if (key == GLFW_KEY_C) {
            glm::vec3 clA(rand(0, 1), rand(0, 1), rand(0, 1)), clB(rand(0, 1), rand(0, 1), rand(0, 1));

            steps = 1;
            genPolygon(NGON);
            updatePoly();
        } else if (key == GLFW_KEY_V) {
            shx = 0;
            shy = 0;
            zoom = 1;
        } else if (key == GLFW_KEY_N) {
            input_str.clear();
            wait_input = true;
            input_type = IT_POLYSIDE;
        } else if (key == GLFW_KEY_S) {
            time_t now = time(0);
            tm *date = localtime(&now);
            string filename = to_string(date->tm_hour) + "-" +
                           to_string(date->tm_min) + "-" +
                           to_string(date->tm_sec) + " " +
                           to_string(date->tm_mday) + "." +
                           to_string(date->tm_mon + 1) + "." +
                           to_string(date->tm_year + 1900) + ".png";

            glfwSetWindowTitle(window, "Saving...");
            int error = save_screenshoot(filename.c_str());
            if (!error)
                input_str = "Saved as " + filename;
            else
                input_str = "Save failed: " + (string)lodepng_error_text(error);
            input_type = IT_MESSAGE;
            msg_t.refresh();
        } else if (key == GLFW_KEY_M) {
            input_str.clear();
            input_type = IT_MSTEPS;
            wait_input = true;
        }
    }
}

void genPolygon(int n) {
    pts.clear();
    for (int i = 0; i < NGON; i++) {
        pts.emplace_back(rand(-1, 1), rand(-1, 1));
    }
    colors.clear();
    for (int i = 0; i < NGON; i++) {
        colors.emplace_back(rand(0, 1), rand(0, 1), rand(0, 1));
    }
}

int save_screenshoot(const char *filename) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    vector<GLubyte> pixels(3 * w * h);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);

    return lodepng::encode(filename, pixels, w, h, LCT_RGB);
}

string to_string(double n, int d) {
    string out;
    int64_t a = n * pow(10, d);
    while (a != 0) {
            out.push_back(a % 10 + '0');
            d--;
            if (!d)
                out.push_back('.');
            a /= 10;
    }
    reverse(out.begin(), out.end());
    return out;
}

void updatePoly() {
    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(glm::vec2), &pts[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLfloat rand(GLfloat a, GLfloat b) {
    mt19937 g(time(0));
    uniform_real_distribution<GLfloat> dist(a, b);
    return dist(generator);
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
