#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <unordered_map>
#include <pthread.h> 
#include <omp.h>
#pragma comment(lib,"pthreadVC2.lib")

using namespace std; 

const int MX = 50000; 

int thread_count;
string rd;
string buf[3][MX];
unordered_map<string, int> mp[3];
volatile int head[3], tail[3], nxt[3];
fstream in, out; 
bool mut; 

pthread_mutex_t mutex1; 

void* work(void* rank);

int main() {

    double start, end;
    start = omp_get_wtime();
    for (int i = 0; i < 3; ++i) {
        head[i] = tail[i] = 0;
    }

    long       thread;
    pthread_t* thread_handles;

    mut = 0; 

    thread_count = 3;
    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));

    pthread_mutex_init(&mutex1, NULL); 

    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, work, (void*)thread);

    in.open("5M_low.txt", ios::in); 
    out.open("out.txt", ios::out); 

    while (in >> rd) {
        int w = rd[0] % 3;
        buf[w][tail[w]] = rd;
        nxt[w] = (tail[w] + 1) % MX;
        while (nxt[w] == head[w]);
        tail[w] = nxt[w];
    }
    mut = 1;

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);

    in.close();
    out.close();
    pthread_mutex_destroy(&mutex1); 

    end = omp_get_wtime();
    printf("%lfs\n", end - start);

    return 0;
} 

void* work(void* rank) {
    long t = (long)rank; 
    while (mut == 0) {
        while (head[t] != tail[t]) {
            mp[t][buf[t][head[t]]]++;
            head[t] = (head[t] + 1) % MX;
        }
    }
    while (head[t] != tail[t]) {
        mp[t][buf[t][head[t]]]++;
        head[t] = (head[t] + 1) % MX;
    }
    pthread_mutex_lock(&mutex1); 
    for (auto it : mp[t]) {
        out << it.first << '=' << it.second << '\n';
    }
    pthread_mutex_unlock(&mutex1);
    return NULL;
}  
