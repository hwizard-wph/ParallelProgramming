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

const int MX = 500000; 

string rd;
string buf[MX];
unordered_map<string, int> mp; 


int main() {

	fstream f;
	f.open("5M_low.txt", ios::in);

	double start, end;
	start = omp_get_wtime();

	volatile int head, tail, nxt; 
	head = tail = 0; 
	bool mut;
	mut = 0;

	//fstream log; 
	//log.open("log.txt", ios::out);

	#pragma omp parallel num_threads(2)
	{
		int t = omp_get_thread_num();
		if (t == 0) {
			while (f >> rd) {
				buf[tail] = rd;
				//log << '(' << head << ',' << tail << "): " << buf[tail] << '\n';
				nxt = (tail + 1) % MX; 
				while (nxt == head); 
				tail = nxt;
			}
			mut = 1;
		}
		else {
			while (mut == 0) {
				while (head != tail) {
					mp[buf[head]]++;
					//log << '(' << head << ',' << tail << "): " << buf[head] << '\n';
					head = (head + 1) % MX; 
				}
			}
			while (head != tail) {
				mp[buf[head]]++;
				//log << '(' << head << ',' << tail << "): " << buf[head] << '\n';
				head = (head + 1) % MX; 
			}
		}
	}

	//log.close(); 

	f.close();
	f.open("out.txt", ios::out);

	for (auto& it : mp) {
		f << it.first << '=' << it.second << '\n';
	}
	
	end = omp_get_wtime();
	printf("%lfs\n", end - start);

	return 0;
}