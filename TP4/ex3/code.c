#include <math.h>

void task_light(int N) {
    double x = 0.0;
    for (int i = 0; i < N; i++)
        x += sin(i * 0.001);
}

void task_moderate(int N) {
    double x = 0.0;
    for (int i = 0; i < N; i++)
        x += sqrt(i * 0.5) * cos(i * 0.001);
}

void task_heavy(int N) {
    double x = 0.0;
    for (int i = 0; i < N; i++)
        x += sqrt(i * 0.5) * cos(i * 0.001) * sin(i * 0.0001);
}
