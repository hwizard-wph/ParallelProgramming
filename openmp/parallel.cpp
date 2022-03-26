#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <omp.h>

using namespace std;

const int L = 200 + 5;
const int R = 65536 + 5;
const int N = 80000000 + 5;

char* buf = new char[L];
char** dt = new char* [N];
int* l = new int[N];
char** bufdt = new char* [N];
int* bufl = new int[N];

void radix_sort(int , int , char** , int *); 

int main() {

    freopen("20M_high.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    double start, end;
    start = omp_get_wtime();

    int n = 0;
    int ml = 0;

    while (scanf("%s", buf) != EOF) {
        l[n] = strlen(buf);
        ml = max(ml, l[n]);
        dt[n] = new char[l[n] + 1];
        strcpy(dt[n], buf);
        ++n; 
    }

    int lim = 1; 
    while (lim <= ml) lim <<= 1; 

    radix_sort(lim, n, dt, l); 

    for (int i = 0; i < n;) {
        printf("%s", dt[i]);
        int j = i + 1; 
        for (; j < n && strcmp(dt[i], dt[j]) == 0; ++j); 
        printf("=%d\n", j - i); 
        i = j; 
    }

    end = omp_get_wtime();
    printf("%lfs\n", end - start);

}

void radix_sort(int len, int n, char** data, int *l) {
    
    for (int p = len; p >= 2; p -= 2) {
        int bucket[R] = { 0 };
        int local_bucket[R] = { 0 };
        #pragma omp parallel firstprivate(local_bucket) 
        {
            #pragma omp for nowait
            for (int i = 0; i < n; ++i) {
                local_bucket[(p - 2 < l[i] ? data[i][p - 2] : 0) * 256 + (p - 1 < l[i] ? data[i][p - 1] : 0)]++; 
            }
            #pragma omp critical
            for (int i = 0; i < R; ++i) {
                bucket[i] += local_bucket[i]; 
            }
            #pragma omp barrier
            #pragma omp single 
            for (int i = 1; i < R; ++i) {
                bucket[i] += bucket[i - 1]; 
            }
            int nthreads = omp_get_num_threads(); 
            int tid = omp_get_thread_num(); 
            for (int cur = nthreads - 1; cur >= 0; --cur) {
                if (cur == tid) {
                    for (int i = 0; i < R; ++i) {
                        bucket[i] -= local_bucket[i]; 
                        local_bucket[i] = bucket[i]; 
                    }
                }   else {
                    #pragma omp barrier
                }
            }
            int x; 
            #pragma omp for private(x) 
            for (int i = 0; i < n; ++i) {
                x = (p - 2 < l[i] ? data[i][p - 2] : 0) * 256 + (p - 1 < l[i] ? data[i][p - 1] : 0); 
                bufdt[local_bucket[x]] = data[i]; 
                bufl[local_bucket[x]] = l[i];
                local_bucket[x]++; 
            }
        }
        char** tmpdt = data; 
        data = bufdt; 
        bufdt = tmpdt; 
        int* tmpl = l; 
        l = bufl; 
        bufl = tmpl; 
    }
}