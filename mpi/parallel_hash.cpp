#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <omp.h>
#include <mpi.h>

using namespace std;

char mesg[205]; 
unordered_map<string, int> mp;

int main() {

	fstream f, w, log;
	f.open("5M_low.txt", ios::in);
	w.open("out.txt", ios::out);

	int numprocs, myid, source;
	double start, end; 
	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	if (myid == 0) {
		start = omp_get_wtime();
		MPI_Send(&start, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
		while (f >> mesg) {
			MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		}
		strcpy(mesg, "*");
		mesg[1] = '\0'; 
		MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
	}
	else if (myid == 1) {
		MPI_Recv(&start, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		while (1) {
			MPI_Recv(mesg, 200, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
			if (mesg[0] == '*') { break; }
			string s = mesg;
			++mp[s];
		}
		for (auto &it: mp) {
			w << it.first << '=' << it.second << '\n';
		}
		end = omp_get_wtime(); 
		printf("%lf\n", end - start); 
	}
	MPI_Finalize();
	f.close(); 
	w.close(); 
	return 0;
}
