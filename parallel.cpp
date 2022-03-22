#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <omp.h>

using namespace std;

const int L = 200;
const int R = 65536;
const int N = 80000000;

char buf[L + 5];
int r[5][R + 5];
int l[N + 5];

char** s; 
int* p, * ys, * rk; 

void clear(int n, int num); 
void Trap(int n, int num, int pos); 
void add(int n, int num); 
void init(int n, int num);
void getP(int n, int num); 
void getReal(int n, int num); 

int main() {

    freopen("5M_low.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    double start, end;
    start = omp_get_wtime();

    int n = 0;
    int ml = 0;
    s = new char* [N];

    while (scanf("%s", &buf) != EOF) {
        ++n;
        l[n] = strlen(buf);
        ml = max(ml, l[n]);
        s[n] = new char[l[n] + 1];
        strcpy(s[n], buf); 
    }

    p = new int[n + 1];
    ys = new int[n + 1];
    rk = new int[n + 1];

    p[0] = 0; 
#   pragma omp parallel num_threads(4)
    {
        init(n, ceil(1.0 * n / 4));
    }

    int lim = 1;
    while (lim < ml) lim <<= 1;
    for (int len = lim; len >= 2; len -= 2) {
        r[0][0] = r[1][0] = r[2][0] = r[3][0] = r[4][0] = 0;
#       pragma omp parallel  num_threads(4)
        {
            clear(R, ceil(1.0 * R / 4)); 
        }
#       pragma omp parallel  num_threads(4)
        {
            Trap(n, ceil(1.0 * n / 4), len);
        }
        r[4][0] = r[0][0] + r[1][0] + r[2][0] + r[3][0];
#       pragma omp parallel  num_threads(4)
        {
            add(R, ceil(1.0 * R / 4)); 
        }
        for (int i = 1; i <= R; ++i) r[4][i] += r[4][i - 1]; 
        for (int i = n; i >= 1; --i) rk[p[i]] = r[4][ys[p[i]]]--;
#       pragma omp parallel  num_threads(4)
        {
            getP(n, ceil(1.0 * n / 4));
        }
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

/*
    for (int i = 1; i <= n; ++i) {
        printf("%d\n", real[i]);
    }
*/

    return 0;
}

void clear(int n, int num) {
    int id = omp_get_thread_num();
    int lp = id * num;
    int rp = min((id + 1) * num, n);
    for (int i = lp; i <= rp; ++i) {
        r[0][i] = r[1][i] = r[2][i] = r[3][i] = r[4][i] = 0; 
    }
}

void Trap(int n, int num, int pos) {
    int id = omp_get_thread_num();
    int lp = id * num + 1; 
    int rp = min((id + 1) * num, n); 
    for (int i = lp; i <= rp; ++i) {
        int p1, p2; 
        if (l[i] < pos) p1 = 0;
        else p1 = s[i][pos - 1];
        if (l[i] < pos - 1) p2 = 0;
        else p2 = s[i][pos - 2]; 
        ys[i] = p2 * 256 + p1; 
        ++r[id][ys[i]]; 
    }
}

void add(int n, int num) {
    int id = omp_get_thread_num();
    int lp = id * num + 1;
    int rp = min((id + 1) * num, n);
    for (int i = lp; i <= rp; ++i) {
        r[4][i] = r[0][i] + r[1][i] + r[2][i] + r[3][i]; 
    }
}

void init(int n, int num) {
    int id = omp_get_thread_num();
    int lp = id * num + 1;
    int rp = min((id + 1) * num, n);
    for (int i = lp; i <= rp; ++i) {
        p[i] = i;
    }
}

void getP(int n, int num) {
    int id = omp_get_thread_num();
    int lp = id * num + 1;
    int rp = min((id + 1) * num, n);
    for (int i = lp; i <= rp; ++i) {
        p[rk[i]] = i;
    }
}