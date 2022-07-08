#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <omp.h>
#include <queue>
#include <fstream>
#include <exception>

using namespace std;

const int MX = 50000;
const int T = 3; 

string str;
string buf[T][MX];
unordered_map<string, int> mp[T];
volatile int head[T], tail[T], nxt[T];


int main(int argc, char** argv) {

	string file = argv[1];
	string op = argv[2]; 
	fstream f, w;
	f.open(file, ios::in);
	w.open("out.txt", ios::out);

	double start, end;
	start = omp_get_wtime();

	for (int i = 0; i < T; ++i) {
		head[i] = tail[i] = nxt[i] = 0; 
	}
	bool mut;
	mut = 0;


#pragma omp parallel num_threads(T)
	{
		int t = omp_get_thread_num();
		if (t == 0) {
			while (f >> str) {
				int p = str[0] % (T-1) + 1; 
				buf[p][tail[p]] = str;
				nxt[p] = (tail[p] + 1) % MX;
				while (nxt[p] == head[p]);
				tail[p] = nxt[p];
			}
			mut = 1;
		}
		else {
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
#			pragma omp critical
			for (auto& it : mp[t]) {
				w << it.first << '\n' << it.second << '\n';
			}
		}
	}

	cout << op << ':' << mp[op[0] % (T-1) + 1][op] << '\n';

	end = omp_get_wtime();
	cout << end - start << "s\n";

	f.close();
	w.close(); 

	return 0;
}

