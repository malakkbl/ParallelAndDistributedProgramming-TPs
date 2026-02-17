#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

const int N = 100000000;

void task_light(int N);
void task_moderate(int N);
void task_heavy(int N);

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

int main() {
    double start, end;
    
    start = (double) clock() / CLOCKS_PER_SEC;
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            task_light(N);
        }
        
        #pragma omp section
        {
            task_moderate(N);
        }
        
        #pragma omp section
        {
            task_heavy(N);
        }
    }
    
    end = (double) clock() / CLOCKS_PER_SEC;
    printf("Execution_time...%lf seconds\n", end - start);
    
    return 0;
}
