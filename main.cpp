#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <random>
#include <ctime>

#include <windows.h>

#include "shader.hpp"
#include "tgaimage.h"
#include "glm/vec3.hpp"

using namespace std;

static const int WINDOW_WIDTH  = 1280,
				 WINDOW_HEIGHT = 760;

// Per second
static const double shift = 2, zoom_add = 2;

const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066;
const int INF = 1e9;

GLFWwindow *window = nullptr;
default_random_engine generator;

void keyCallback(GLFWwindow *, int key, int scancode, int action, int mode);
void frameBufferSizeCallback(GLFWwindow *, int width, int height);

struct point {
    GLfloat x, y;
    point() {}
    point(GLfloat x, GLfloat y) : x(x), y(y) {}
    friend point operator+(point a, point b) {
        return point(a.x + b.x, a.y + b.y);
    }
};

struct Timer {
    double passed, delay;
    Timer() {}
    Timer(double delay) : passed(glfwGetTime()), delay(delay) {}
    bool tick() {
        if (glfwGetTime() - passed >= delay) {
            passed = glfwGetTime();
            return true;
        }
        return false;
    }
    double left() {
        return delay - (glfwGetTime() - passed);
    }

    void refresh() {
        passed = glfwGetTime();
    }
};

bool save_screenshoot(const char *filename) {
    TGAImage img(WINDOW_WIDTH, WINDOW_HEIGHT, TGAImage::RGB);
    GLubyte *pixels = (GLubyte *)malloc(TGAImage::RGB * WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(GLubyte));
    glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            img.set(x, y, TGAColor(pixels[3 * (WINDOW_WIDTH*y + x)], pixels[3 * (WINDOW_WIDTH*y + x) + 1], pixels[3 * (WINDOW_WIDTH*y + x) + 2], 255));
        }
    }
    free(pixels);
    return img.write_tga_file(filename);
}

// Gives you a point from interpolation of a and b coords
// t - is interpolation step from [0..1]
point interpolate(point &a, point &b, double t) {
    point out;
    out.x = a.x + (b.x - a.x) * t;
    out.y = a.y + (b.y - a.y) * t;
    return out;
}

glm::vec3 interpolate(glm::vec3 a, glm::vec3 b, double t) {
    glm::vec3 out;
    out.x = a.x + (b.x - a.x) * t;
    out.y = a.y + (b.y - a.y) * t;
    out.z = a.z + (b.z - a.z) * t;
    return out;
}

double dist(point a, point b) {
    return hypot(b.x - a.x, b.y - a.y);
}

GLfloat rand(GLfloat a, GLfloat b) {
    mt19937 g(time(0));
    uniform_real_distribution<GLfloat> dist(a, b);
    return dist(generator);
}

string to_string(double n, int d = 2) {
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

GLuint vvbo, cvbo, vao;
vector<point> pts;
vector<glm::vec3> colors;
int steps = 1, ord = 1, NGON = 22, MAX_STEPS = 1000;
double zoom = 1;
double shx = 0, shy = 0;

void updatePoly() {
    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(point), &pts[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Timer save_t(2);
int input_type;
bool wait_input = false;
string input_str;

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

int main() {
    // INITIALIZATION
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Polycursion", 0, 0);
	if (!window) cerr << "Use ur FOKING GPU pls\n";
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) cerr << "GLEW is dead\n";
/*    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
    glBlendFuncSeparate(GL_DST_COLOR, GL_SRC_ALPHA, GL_ONE, GL_ZERO);
*/
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // SHADERS
    ShaderProgram prg;
    prg.loadFromFile("vert", "frag");

    // DATA
    genPolygon(NGON);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vvbo);
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), &pts[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vvbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // APP CYCLE
    Timer watch(0.01), fps(1.0/60), sec(1);
    cout << "SPF = " << fps.delay << endl;
    int frames = 0, frames_ps = 0;
    double frame_till = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        if (steps >= MAX_STEPS && !fps.tick())
            Sleep(fps.left() * 1e3), fps.refresh();

        if (input_type == 2 && save_t.tick())
            input_type = 0;

        double dt = glfwGetTime() - frame_till;
        frame_till = glfwGetTime();

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

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        if (steps < MAX_STEPS) {
            steps++;
            for (int i = 0; i < NGON - 1; i++) {
                pts.push_back(interpolate(pts[pts.size() - NGON], pts[pts.size() - NGON + 1], rand(0, 1)));
            }
            pts.push_back(interpolate(pts[pts.size() - NGON], pts[pts.size() - 2 * NGON + 1], rand(0, 1)));

            for (int i = 0; i < NGON; i++)
                colors.emplace_back(rand(0, 1), rand(0, 1), rand(0, 1));

            updatePoly();
        }

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

        glfwPollEvents();
        glfwSwapBuffers(window);
        frames++;

        string title;
        if (input_type == 0) {
            title = "Polycursion " + to_string(100 * steps / MAX_STEPS) + "% [" + to_string(frames_ps) + "fps] | Zoom " + to_string(zoom, 2) + "x | " + to_string(NGON) + "-gon | Depth " + to_string(steps) + "/" + to_string(MAX_STEPS);
        } else if (input_type == 1) {
            title = "Generate " + input_str + "-gon";
        } else if (input_type == 2) {
            title = input_str;
        } else if (input_type == 3) {
            title = "No. steps to perform = " + input_str;
        }
        glfwSetWindowTitle(window, title.c_str());
        if (sec.tick())
            frames_ps = frames, frames = 0;
    }

    glfwTerminate();
    return 0;
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
                    if (input_type == 1 && !input_str.empty()) {
                        NGON = stoi(input_str);
                        genPolygon(NGON);
                        steps = 1;
                    } else if (input_type == 3 && !input_str.empty()) {
                        MAX_STEPS = stoi(input_str);
                    }
                    wait_input = false;
                    input_type = 0;
                }
        } else if (key == GLFW_KEY_C) {
            glm::vec3 clA(rand(0, 1), rand(0, 1), rand(0, 1)), clB(rand(0, 1), rand(0, 1), rand(0, 1));

            puts("!!!RECLEAN!!!");
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
            input_type = 1;
        } else if (key == GLFW_KEY_S) {
            time_t now = time(0);
            tm *date = localtime(&now);
            string filename = to_string(date->tm_hour) + "-" +
                           to_string(date->tm_min) + "-" +
                           to_string(date->tm_sec) + " " +
                           to_string(date->tm_mday) + "." +
                           to_string(date->tm_mon + 1) + "." +
                           to_string(date->tm_year + 1900) + ".tga";
            if (save_screenshoot(filename.c_str()))
                input_str = "Saved as " + filename;
            else
                input_str = "Save filed";
            input_type = 2;
            save_t.refresh();
        } else if (key == GLFW_KEY_M) {
            input_str.clear();
            input_type = 3;
            wait_input = true;
        }
    }
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
