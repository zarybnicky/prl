#include <mpi.h>
#include <iostream>
#include <fstream>

using namespace std;

void loadInput(int *data) {
  fstream input("numbers", ios::in);
  for (int i = 0; input.peek() != EOF; i++) {
    data[i] = input.get();
    if (i > 0) cout << " ";
    cout << data[i];
  }
  cout << endl;
  input.close();
}

void exchangeOnly(int *number, int rank) {
  MPI_Status stat;
  MPI_Send(number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  MPI_Recv(number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &stat);
}

void exchangeAndCompare(int *number, int rank) {
  int neighbor[1];
  MPI_Status stat;
  MPI_Recv(neighbor, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &stat);
  if (*neighbor > *number) {
    MPI_Send(number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    *number = *neighbor;
  } else {
    MPI_Send(neighbor, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
  }
}

int main(int argc, char *argv[]) {
  int numprocs;
  int rank;
  int number[1];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Load and distribute data
  int *data = new int[numprocs];
  if (rank == 0) {
    loadInput(data);
  }
  MPI_Scatter(data, 1, MPI_INT, number, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Actually do the sorting
  for (int j = 0; j < numprocs; j++) {
    if (j % 2 == 0) {
      // Even processes do the comparing
      if (rank % 2 == 0) {
        if (rank > 0) exchangeAndCompare(number, rank);
      } else {
        if (rank < numprocs - 1) exchangeOnly(number, rank);
      }
    } else {
      // Odd processes do the comparing
      if (rank % 2 == 1) {
        if (rank > 0) exchangeAndCompare(number, rank);
      } else {
        if (rank < numprocs - 1) exchangeOnly(number, rank);
      }
    }
  }

  // Collect and print
  MPI_Gather(number, 1, MPI_INT, data, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    for (int i = 0; i < numprocs; i++) cout << data[i] << endl;
  }

  MPI_Finalize();
  return 0;
}
