#include <mpi.h>
#include <iostream>
#include <fstream>

using namespace std;

#define TAG 0

void distributeInput(void) {
  int target = 0;
  fstream input("numbers", ios::in);
  while (input.peek() != EOF) {
    int number = input.get();
    if (target != 0) cout << " ";
    cout << number;
    MPI_Send(&number, 1, MPI_INT, target++, TAG, MPI_COMM_WORLD);
  }
  cout << endl;
}

int main(int argc, char *argv[]) {
  int numprocs;
  int myid;
  int neighnumber;
  int mynumber;
  MPI_Status stat;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  if (myid == 0) {
    distributeInput();
  }
  MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

  int oddlimit = 2 * (numprocs / 2) - 1;
  int evenlimit = 2 * ((numprocs - 1) / 2);

  int cycles = 0;
  for (int j = 1; j <= numprocs / 2; j++) {
    cycles++;

    if ((!(myid % 2) || myid == 0) && myid < oddlimit) {
      MPI_Send(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD);
      MPI_Recv(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD, &stat);
    } else if (myid <= oddlimit) {
      MPI_Recv(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD, &stat);
      if (neighnumber > mynumber) {
        MPI_Send(&mynumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
        mynumber = neighnumber;
      } else {
        MPI_Send(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
      }
    }

    if (myid % 2 && myid < evenlimit) {
      MPI_Send(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD);
      MPI_Recv(&mynumber, 1, MPI_INT, myid + 1, TAG, MPI_COMM_WORLD, &stat);
    } else if (myid <= evenlimit && myid != 0) {
      MPI_Recv(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD, &stat);
      if (neighnumber > mynumber) {
        MPI_Send(&mynumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
        mynumber = neighnumber;
      } else {
        MPI_Send(&neighnumber, 1, MPI_INT, myid - 1, TAG, MPI_COMM_WORLD);
      }
    }
  }

  // Collect results
  if (myid == 0) {
    cout << mynumber << endl;
    for (int i = 1; i < numprocs; i++) {
      MPI_Recv(&neighnumber, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
      cout << neighnumber << endl;
    }
  } else {
    MPI_Send(&mynumber, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
