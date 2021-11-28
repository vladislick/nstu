#include <stdio.h>
#include <math.h>

#define DT 0.2
#define PI 3.14159
#define OMEGA 3.0
#define DELTAW 0.5
#define L 225
#define N 2*L+1

// Множитель a от k
float a(int k) {
    float t = k*DT*(OMEGA + DELTAW/2);
    if (k == 0) return 0.0;
    else return 2*(PI/DT)*(sin(t) - t*cos(t))/pow(PI*(float)k, 2);
}

float sigma(float k) {
    return 1.0;
}

// Отсчёт ИХ
float wk(int k) {
    if (k >= 0 && k <= L) return 0.5*sigma(L-k)*a(L-k);
    else return -0.5*sigma(k-L)*a(k-L);
}

// АЧХ на частоте w
float A(float w) {
    float res = 0.0;
    for (int k = 1; k <= L; k++) res += 2*wk(L-k)*sin((float)k*DT*w);
    if (res < 0) res = 0 - res;
    return res;
}

// ФЧХ на частоте w
float f(float w) {
    return PI/2 - L * DT * w;
}

int main(int argc, char* argv[]) {
    char str[100];
    float w = 0.0;
    printf("t\tW(t)\tw\tA(w)\tf(w)\n");
    for (int i = 0; i < N; i++) {
        if (w <= 10.1) sprintf(str, "%f\t%f\t%f\t%f\t%f", i*DT, wk(i), floor(w*10)/10, A(w), f(w));
        else sprintf(str, "%f\t%f", i*DT, wk(i));
        for (int j = 0; str[j] != '\0'; j++) 
            if (str[j] == '.') str[j] = ',';
        printf("%s\n", str);
        if (w < 11) w += 0.1;
    }
    return 0;
}