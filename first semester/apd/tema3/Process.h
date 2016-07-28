/*
 * Process.h
 *
 *  Created on: Dec 24, 2015
 *      Author: arotaru
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include "Topology.h"
#include "Sudoku.h"

namespace tema3 {

#define byte char

class Process {
public:
	Process(int);
	Process(int, Topology*, SudokuTable, SudokuMatrixPart);
	virtual ~Process();

	void DiscoverTopology(int size);
	void ComputeRoutingTable();
	void ResolveMatrix();

	int GetId() const;
	int GetParentId() const;
	Topology* GetTopology() const;
	SudokuTable GetSudokuTable() const;

private:
	uint 		m_iProcessId;
	int  		m_iParentId;
	Topology* 	m_pCurrTopology;

	std::vector<std::string> 				m_vCombinedSolutions;
	std::vector<std::vector<std::string> > 	m_vSolutions;
	SudokuTable 							m_stCompleteTable;
	SudokuTable								m_tempTable;
	SudokuMatrixPart						m_mySudokuTablePart;

	void _GenerateMySolutions(uint l, uint c, std::vector<uint>&);
	void _GenerateSolutionFromChildrenSolutions(uint currId, uint finalId, SudokuTable st);
};

} /* namespace tema3 */

#endif /* PROCESS_H_ */
