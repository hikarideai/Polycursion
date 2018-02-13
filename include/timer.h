#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

struct Timer {
    double last, delay;
    Timer() : last(glfwGetTime()) {}
    Timer(double delay) : last(glfwGetTime()), delay(delay) {}
    bool tick() {
        if (glfwGetTime() - last >= delay) {
            last = glfwGetTime();
            return true;
        }
        return false;
    }
    double elapsed() {
        return glfwGetTime() - last;
    }

    double left() {
        return delay - (glfwGetTime() - last);
    }

    void refresh() {
        last = glfwGetTime();
    }
};

#endif // TIMER_H_INCLUDED
