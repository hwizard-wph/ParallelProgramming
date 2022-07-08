#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <pthread.h> 
#include <omp.h>

using namespace std; 

const int MX = 50000; 
const int T = 2; 

int thread_count;
string rd;
string buf[T][MX];
unordered_map<string, int> mp[T];
volatile int head[T], tail[T], nxt[T];
bool mut; 
fstream in, out;

pthread_mutex_t mutex1; 

void* work(void* rank);

int main(int argc, char** argv) {

    string file = argv[1];
    string op = argv[2];
    double start, end;
    start = omp_get_wtime();
    for (int i = 0; i < T; ++i) {
        head[i] = tail[i] = 0;
    }

    long       thread;
    pthread_t* thread_handles;

    mut = 0; 

    thread_count = T;
    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));

    pthread_mutex_init(&mutex1, NULL); 

    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, work, (void*)thread);

    in.open(file, ios::in);
    out.open("out.txt", ios::out);

    while (in >> rd) {
        int w = rd[0] % T;
        buf[w][tail[w]] = rd;
        nxt[w] = (tail[w] + 1) % MX;
        while (nxt[w] == head[w]);
        tail[w] = nxt[w];
    }
    mut = 1;

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);

    cout << op << ':' << mp[op[0] % T][op] << '\n';

    in.close();
    out.close();
    pthread_mutex_destroy(&mutex1); 

    end = omp_get_wtime();
    cout << end - start << "s\n"; 

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
        out << it.first << '\n' << it.second << '\n';
    }
    pthread_mutex_unlock(&mutex1);
    return NULL;
}  
