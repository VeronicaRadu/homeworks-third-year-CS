/*
 ============================================================================
 Name        : SudokuSolver.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Compute Pi in MPI C++
 ============================================================================
 */
//#include <iostream>
#include "mpi.h"
//#include <mpi/mpi.h>
#include "Topology.h"
#include "TopologyReader.h"
#include "Process.h"
#include "Sudoku.h"
#include <unistd.h>
#include <sstream>

int main(int argc, char *argv[]) {
	if (argc != 4) {
		return -1;
	}

	MPI_Init(&argc, &argv);
	int size = 0, rank = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	std::string path = argv[1];
	tema3::TopologyReader treader;
	tema3::Topology *currTopology = treader.GetTopologyFromFile(path, rank, size);
	tema3::SudokuTableReader str(argv[2]);
	tema3::SudokuTable* table = str.GetTable();
	tema3::SudokuMatrixPart myPart = str.GetTaskPart(rank);
	tema3::SudokuTable leTablau = *table;
	tema3::Process *currProcess = new tema3::Process(rank, currTopology, leTablau, myPart);
	currProcess->DiscoverTopology(size);
	currProcess->ComputeRoutingTable();
	std::stringstream output_file_maker;
	output_file_maker << "process#" << currProcess->GetId() << ".out";
	std::ofstream out(output_file_maker.str().c_str(), std::ifstream::out);
	if (0 == currProcess->GetId()) {
		currTopology->Show(SHOW_MATRIX | SHOW_ROUTING_TABLE, out);
	} else {
		currTopology->Show(SHOW_ROUTING_TABLE, out);
	}
	out.close();

	currProcess->ResolveMatrix();
	if (0 == currProcess->GetId()) {
		std::ofstream sol(argv[3], std::ofstream::out);
		sol << str.GetProblemDimension() << "\n";
		currProcess->GetSudokuTable().PrintTo(sol);
		sol.close();
	}

	MPI_Finalize();
	return 0;
}

