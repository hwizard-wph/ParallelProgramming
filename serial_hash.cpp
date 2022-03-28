#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <omp.h>
#include <fstream>

using namespace std;

int main() {

    fstream f;
    f.open("5M_low.txt", ios::in);

    
    double start, end;
    start = omp_get_wtime();

    string buf;
    unordered_map<string, int> mp;

    while (f >> buf) {
        mp[buf]++; 
    }

    f.close();
    f.open("output.txt",ios::out);

    for (auto &it : mp) {
        f << it.first << "=" << it.second << '\n';
    }

    end = omp_get_wtime(); 
    printf("%lf seconds\n", end - start); 


    return 0;
}