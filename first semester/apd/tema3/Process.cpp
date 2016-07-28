/*
 * Process.cpp
 *
 *  Created on: Dec 24, 2015
 *      Author: arotaru
 */

#include "Process.h"
#include "mpi.h"
//#include <mpi/mpi.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <stdio.h>

namespace tema3 {

//#define debug

enum MessageType {
	sondaj = 0, ecou, solutie, ciclu, rezolva, rezolvare, terminat
};

Process::Process(int id) {
	m_iProcessId = id;
	m_iParentId = -1;
	m_pCurrTopology = NULL;
}

Process::Process(int id, Topology* top, SudokuTable st, SudokuMatrixPart part) {
	m_iProcessId = id;
	m_iParentId = -1;
	m_pCurrTopology = top;
	m_mySudokuTablePart = part;
	m_stCompleteTable = st;
	m_tempTable = st;
	//std::cout << "*" << st.GetDimension() << "," << m_stCompleteTable.GetDimension() << ", " << m_tempTable.GetDimension() << "\n";
}

Process::~Process() {
	delete m_pCurrTopology;
}
// tag 0 = sondaj
// tag 1 = raspuns
// tag 2 = sudok
// tag 3 = taie legatura

char* toPChar(const std::string& s) {
	int l = s.length();
	char* ptr = new char[l + 1];
	memcpy(ptr, s.c_str(), l);
	return ptr;
}

void Process::DiscoverTopology(int size) {
	std::stringstream b;
	if (0 == m_iProcessId) {
#ifdef debug
		std::cout << "Process#"<< m_iProcessId << "sends :\n";
#endif
		//m_pCurrTopology->Show();
		b << (*m_pCurrTopology);
		int tag = sondaj;
		int len = b.str().length();

		std::vector<int> v = m_pCurrTopology->GetNeighbors(m_iProcessId);
		for (uint i = 0; i < v.size(); i++) {
			int dest = v[i];
			MPI_Send(&len, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
#ifdef debug
			std::cout << "to#"<< dest << "an sondaj\n";
#endif
		}

		MPI_Status status;
		tag = ecou;
		for (uint i = 0; i < v.size(); i++) {
			MPI_Recv(&len, 1, MPI_INT, v[i], tag, MPI_COMM_WORLD, &status);
			byte *data = new byte[len];
			MPI_Recv(data, len, MPI_INT, v[i], tag, MPI_COMM_WORLD, &status);
			std::stringstream aux(data);
			aux >> (*m_pCurrTopology);
		}
	} else {
		MPI_Status status;
		int len = 0;
		int tag = sondaj;
		MPI_Recv(&len, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD,
				&status);
		m_iParentId = status.MPI_SOURCE;
		byte *data = NULL;
#ifdef debug
		std::cout << "Process#"<< m_iProcessId << ":\n";
		m_pCurrTopology->Show();
		std::cout << "received from its parent#"<< m_iParentId << " a \"sondaj\"\n";
#endif
		b << (*m_pCurrTopology);
		len = b.str().length();

		std::vector<int> v = m_pCurrTopology->GetNeighbors(m_iProcessId);
		for (uint i = 0; i < v.size(); i++) {
			if (v[i] == m_iParentId)
				continue;
			MPI_Send(&len, 1, MPI_INT, v[i], tag, MPI_COMM_WORLD);
#ifdef debug
			std::cout << "Process#" << m_iProcessId << "sends to his child process#" << v[i]<< " a \"sondaj\"\n";
#endif
		}

		for (uint i = 0; i < v.size(); i++) {
			if (v[i] == m_iParentId)
				continue;
			MPI_Recv(&len, 1, MPI_INT, v[i], MPI_ANY_TAG, MPI_COMM_WORLD,
					&status);
			tag = status.MPI_TAG;
			if (ecou == tag) {
#ifdef debug
				std::cout << "Process#" << m_iProcessId << "receives from his child process#" << v[i] << " a \"ecou\"\n";
#endif
				data = new byte[len];
				MPI_Recv(data, len, MPI_INT, v[i], tag, MPI_COMM_WORLD,
						&status);
				std::stringstream ss(data);
				ss >> (*m_pCurrTopology);
			} else if (sondaj == tag) {
				m_pCurrTopology->SetValue(m_iProcessId, v[i], 7); // stergem legatura
#ifdef debug
						std::cout << "Process#" << m_iProcessId << "receives from his child process#" << v[i] << " a \"sondaj\"\n";
#endif
				data = new byte[len];
				MPI_Send(&len, 1, MPI_INT, v[i], tag, MPI_COMM_WORLD);
			}
		}

#ifdef debug
		std::cout << "Process#" << m_iProcessId << " is sending to parent#" << m_iParentId << "\n";
		m_pCurrTopology->Show();
#endif

		std::stringstream m;
		tag = ecou;
		m << (*m_pCurrTopology);
		len = m.str().length();
		MPI_Send(&len, 1, MPI_INT, m_iParentId, tag, MPI_COMM_WORLD);
		MPI_Send(toPChar(m.str()), len, MPI_CHAR, m_iParentId, tag,
		MPI_COMM_WORLD);
	}
}

void Process::ComputeRoutingTable() {
	m_pCurrTopology->ComputeRoutingTable(m_iProcessId, m_iParentId);
}

void Process::ResolveMatrix() {
	std::vector<int> children = m_pCurrTopology->GetNeighbors(m_iProcessId);
	if (std::find(children.begin(), children.end(), m_iParentId)
			!= children.end()) {
		children.erase(
				std::find(children.begin(), children.end(), m_iParentId));
	}
	m_vSolutions = std::vector<std::vector<std::string> >(children.size() + 1,
			std::vector<std::string>());

	std::vector<uint> helper(m_pCurrTopology->GetSize());
	for (uint l = m_mySudokuTablePart.GetBeginLine();
			l <= m_mySudokuTablePart.GetEndingLine(); l++) {
		for (uint c = m_mySudokuTablePart.GetBeginColumn();
				c <= m_mySudokuTablePart.GetEndingColumn(); c++) {
			if (0 != m_tempTable.GetBox(l, c)) {
				helper[m_tempTable.GetBox(l, c)] = 1;
			}
		}
	}
	_GenerateMySolutions(m_mySudokuTablePart.GetBeginLine(),
			m_mySudokuTablePart.GetBeginColumn(), helper);
	bool bIamLeaf = (m_pCurrTopology->GetNeighbors(m_iProcessId).size() == 1);
	if (true == bIamLeaf) {
		std::stringstream ss;
		for (uint i = 0; i < m_vSolutions[m_vSolutions.size() - 1].size(); i++) {
			ss << m_vSolutions[m_vSolutions.size() - 1][i] << "|";
		}
		uint tag = rezolvare;
		uint len = ss.str().length();
		char *data = toPChar(ss.str());
		MPI_Send(&len, 1, MPI_INT, m_iParentId, tag, MPI_COMM_WORLD);
		MPI_Send(data, len, MPI_CHAR, m_iParentId, tag, MPI_COMM_WORLD);
	} else {
		for (uint i = 0; i < children.size(); i++) {
			uint len = 0;
			char *data = NULL;
			uint tag = rezolvare;
			MPI_Status status;
			MPI_Recv(&len, 1, MPI_INT, children[i], tag, MPI_COMM_WORLD, &status);
			data = new char[len];
			MPI_Recv(data, len, MPI_CHAR, children[i], tag, MPI_COMM_WORLD, &status);
			char *p = strtok(data, "|");
			while (p) {
				m_vSolutions[i].push_back(std::string(p));
				p = strtok(NULL, "|");
			}
		}
		_GenerateSolutionFromChildrenSolutions(0, m_vSolutions.size(), m_stCompleteTable);

		if (0 != m_iProcessId) {
			std::stringstream ss;
			for (uint i = 0; i < m_vCombinedSolutions.size(); i++) {
				ss << m_vCombinedSolutions[i] << "|";
			}
			uint tag = rezolvare;
			uint len = ss.str().length();
			char *data = toPChar(ss.str());
			MPI_Send(&len, 1, MPI_INT, m_iParentId, tag, MPI_COMM_WORLD);
			MPI_Send(data, len, MPI_CHAR, m_iParentId, tag, MPI_COMM_WORLD);
		} else {
			std::stringstream ss(m_vCombinedSolutions[0]);
			ss >> m_stCompleteTable;
			/*SudokuTable st = m_stCompleteTable;
			std::cout << m_iProcessId << " has a solution=================================================\n";
			for (uint i = 0; i < st.GetDimension(); i++) {
				for (uint j = 0; j < st.GetDimension(); j++) {
					if (j % 3 == 0)
						std::cout << "|";
					std::cout << st.GetBox(i, j) << " ";
				}
				if (i % 3 == 2)
					std::cout << "\n=====================";
				std::cout << "\n";
			}*/
		}
	}
}

void Process::_GenerateMySolutions(uint line, uint col, std::vector<uint>& assigned) {
	if (m_mySudokuTablePart.IsEnd(line, col)) {
		std::stringstream ss;
		ss << m_tempTable;
		m_vSolutions[m_vSolutions.size() - 1].push_back(ss.str());
		return;
	}

	if (0 != m_tempTable.GetBox(line, col)) {
		std::pair<uint, uint> npos = m_mySudokuTablePart.GetNextBox(line, col);
		uint newline = npos.first;
		uint newcol = npos.second;
		_GenerateMySolutions(newline, newcol, assigned);
	} else {

		for (uint val = 1; val <= m_tempTable.GetDimension(); val++) {
			if (1 == assigned[val]) continue;
			bool bRet = m_tempTable.SetBox(line, col, val);
			if (bRet) {
				assigned[val] = 1;
				std::pair<uint, uint> npos = m_mySudokuTablePart.GetNextBox(line, col);
				uint newline = npos.first;
				uint newcol = npos.second;
				_GenerateMySolutions(newline, newcol, assigned);
				m_tempTable.SetBox(line, col, 0);
				assigned[val] = 0;
			}
		}
	}
}

/*I receive a solution from a child then try to merge it with solutions from other children*/
void Process::_GenerateSolutionFromChildrenSolutions(uint currId, uint finalId, SudokuTable st) {

	if (currId >= finalId) {
		std::stringstream ss;
		ss << st;
		m_vCombinedSolutions.push_back(ss.str());
		return;
	}

	for (uint solIdx = 0; solIdx < m_vSolutions[currId].size(); solIdx++) {
		SudokuTable temp = st;
		std::stringstream ss(m_vSolutions[currId][solIdx]);
		bool bNotConflict = true;
		uint d;
		ss >> d;
		while (!ss.eof()) {
			uint l, c, v;
			ss >> l >> c >> v;
			if (!temp.SetBox(l, c, v)) {
				bNotConflict = false;
				break;
			}
		}
		if (bNotConflict) {
			_GenerateSolutionFromChildrenSolutions(currId + 1, finalId, temp);
		}
	}
}

int Process::GetId() const {
	return m_iProcessId;
}

int Process::GetParentId() const {
	return m_iParentId;
}

Topology* Process::GetTopology() const {
	return m_pCurrTopology;
}

SudokuTable Process::GetSudokuTable() const {
	return m_stCompleteTable;
}

} /* namespace tema3 */
