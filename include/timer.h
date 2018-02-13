#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

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

#endif // TIMER_H_INCLUDED
