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
string s;
unordered_map<string, int> mp;

int main(int argc, char** argv) {
	string infile = argv[1];
	string op = argv[2]; 

	fstream f, w, log;
	f.open(infile, ios::in);
	w.open("out.txt", ios::out);

	int numprocs, myid, source;
	double start, end;
	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	if (myid == 0) {
		start = omp_get_wtime();
		MPI_Send(&start, 1, MPI_DOUBLE, numprocs - 1, 0, MPI_COMM_WORLD);
		while (f >> mesg) {
			int p = mesg[0] % (numprocs - 2) + 1; 
			MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, p, 0, MPI_COMM_WORLD);
		}
		for (int i = 1; i < numprocs - 1; ++i) {
			strcpy(mesg, "*\0");
			MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	}
	else if (myid != numprocs - 1) {
		while (1) {
			MPI_Recv(mesg, 155, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
			if (mesg[0] == '*') { break; }
			s = mesg;
			++mp[s];
		}
		string file = "0.txt";
		file[0] = (char)(myid + '0'); 
		log.open(file, ios::out);
		for (auto it : mp) {
			log << it.first << '\n' << it.second << '\n'; 
		}
		log.close(); 
		if (myid == op[0] % (numprocs - 2) + 1) {
			cout << op << ':' << mp[op] << '\n'; 
		}
		strcpy(mesg, "*\0");
		MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, numprocs - 1, 0, MPI_COMM_WORLD);
	}
	else {
		MPI_Recv(&start, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < numprocs - 1; ++i) {
			MPI_Recv(mesg, 155, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		}
		for (int i = 1; i < numprocs - 1; ++i) {
			string file = "0.txt";
			file[0] = (char)(i + '0');
			//cout << file << '\n'; 
			log.open(file, ios::in);
			while (log >> s) {
				w << s << '\n'; 
			}
			log.close(); 
		}
		end = omp_get_wtime();
		printf("%lf seconds\n", end - start);
	}
	f.close(); 
	w.close(); 
	MPI_Finalize();
	return 0;
}
