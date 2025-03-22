#include <math.h>
#include <stdio.h>

int sgn(int x)
{
    if (x >= 0) return 1;
    return -1;
}

double map(double x) { return x / 50 - 10; }

double pam(double y) { return (y + 10) * 50; }

void itoa(int x, char* a, int sign)
{
    if (x == 0) {
        *a = '0';
        *(a + 1) = '\0';
        return;
    };
    int negative = x < 0 ? 1 : 0;
    x *= sgn(x);

    int len = 0;
    int temp = x;

    while (temp > 0) {
        temp /= 10;
        len++;
    }

    int tmp_len = len;
    while (x > 0) {
        int digit = x % 10;
        a[--tmp_len] = digit + '0';
        x /= 10;
    }

    a[len] = '\0';
    if (sign && negative) {
        for (int i = len; i >= 0; i--) a[i + 1] = a[i];
        a[0] = '-';
    }
}