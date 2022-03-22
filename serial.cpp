#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <omp.h>

using namespace std;

const int L = 200;
const int R = 65540;
const int N = 80000000;

char buf[L + 5];
int r[R + 5];
int l[N + 5];

int main() {

    freopen("5M_low.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    
    double start, end;
    start = omp_get_wtime();

    int n = 0;
    int ml = 0;
    char** s = new char* [N];

    while (scanf("%s", &buf) != EOF) {
        ++n;
        l[n] = strlen(buf);
        ml = max(ml, l[n]);
        s[n] = new char[l[n] + 1];
        strcpy(s[n], buf); 
    }


    int* p = new int[n + 1];
    int* ys = new int[n + 1];
    int* rk = new int[n + 1];

    int lim = 1;
    while (lim < ml) lim <<= 1;
    for (int i = 0; i <= n; ++i) p[i] = i; 
    for (int len = lim; len >= 2; len -= 2) {
        for (int i = 12336; i <= R; ++i) r[i] = 0;
        for (int i = 1; i <= n; ++i) {
            int p1, p2;
            if (l[i] < len) p1 = '0';
            else p1 = s[i][len - 1];
            if (l[i] < len - 1) p2 = '0';
            else p2 = s[i][len - 2];
            ys[i] = p2 * 256 + p1;
            ++r[ys[i]];
        }
        for (int i = 12337; i <= R; ++i) r[i] += r[i - 1];
        for (int i = n; i >= 1; --i) rk[p[i]] = r[ys[p[i]]]--;
        for (int i = 1; i <= n; ++i) p[rk[i]] = i;
    }


    for (int i = 1; i <= n; ++i) {
        printf("%s", s[p[i]]);
        int j;
        for (j = i + 1; j <= n && strcmp(s[p[i]], s[p[j]]) == 0; ++j);
        printf("=%d\n", j - i);
        i = j - 1;
    }

    end = omp_get_wtime(); 
    printf("Work took %lf seconds\n", end - start); 


    return 0;
}