/*
 * Topology.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: arotaru
 */

#include "Topology.h"
#include <queue>
#include <set>

namespace tema3 {

void Topology::Show(unsigned int what, std::ostream& where) {
	if (what & SHOW_MATRIX) {
		for (int i = 0; i < m_iSize; i++) {
			for (int j = 0; j < m_iSize; j++) {
				where << m_ppMatrix[i][j] << "|";
			}
			where << "\n";
		}
		where << "\n";
	}

	if(what & SHOW_ROUTING_TABLE)
	for (int i = 0; i < m_iSize; i++) {
		where << m_pRoutingTable[i] << ",";
	}
	where << "\n";
}

void Topology::Init(int s) {
	m_iSize = s;
	m_ppMatrix = new int*[s];
	int *aux = new int[s * s];
	for (int i = 0; i < s * s; i++) {
		aux[i] = 0;
	}
	m_pRoutingTable = new int[s];
	for (int i = 0; i < s; i++) {
		m_ppMatrix[i] = aux + i * s;
		m_pRoutingTable[i] = -1;
	}

	m_iNonZeroCnt = 0;
}

void Topology::Combine(const Topology& top) {
	for (int i = 0; i < top.GetSize(); i++) {
		for (int j = 0; j < top.GetSize(); j++) {
			if (1 == top.GetValue(i, j)) {
				SetValue(i, j, 1);
			}
		}
	}
}

void Topology::Combine(const Topology* top) {
	Combine(*top);
}

Topology::Topology(int s) {
	Init(s);
}

std::vector<int> Topology::GetNeighbors(int procId) {
	std::vector<int> ret;
	if (procId < 0 || procId >= m_iSize) {
		return ret;
	}
	for (int i = 0; i < m_iSize; i++) {
		if (1 == m_ppMatrix[procId][i]) {
			ret.push_back(i);
		}
	}
	return ret;
}

Topology::~Topology() {
	m_iSize = 0;
	delete[] *m_ppMatrix;
	m_ppMatrix = NULL;
	delete[] m_pRoutingTable;
	m_pRoutingTable = NULL;
}

int Topology::GetSize() const {
	return m_iSize;
}

bool Topology::SetValue(int line, int col, int value) {
	if (line < 0 || line >= m_iSize || col < 0 || col >= m_iSize) {
		return false;
	}

	if (0 != value && 0 == m_ppMatrix[line][col]) {
		m_iNonZeroCnt++;
	} else if (0 != m_ppMatrix[line][col] && 0 == value) {
		m_iNonZeroCnt--;
	}

	m_ppMatrix[line][col] = value;
	m_ppMatrix[col][line] = value;

	return true;
}

int Topology::GetValue(int line, int col) const {
	if (line < 0 || line >= m_iSize || col < 0 || col >= m_iSize) {
		return 0;
	}
	return m_ppMatrix[line][col];
}

bool Topology::ComputeRoutingTable(int forWho, int par) {
	if (forWho < 0 || forWho >= m_iSize) {
		return false;
	}
	std::vector<int> children = GetNeighbors(forWho);
	std::set<int> visited;
	for (int i = 0; i < m_iSize; i++) {
		SetNextHop(i, par);
	}
	SetNextHop(forWho, forWho);
	visited.insert(forWho);
	visited.insert(par);
	for (unsigned int i = 0; i < children.size(); i++) {
		if (children[i] == par)
			continue;
		SetNextHop(children[i], children[i]);
		std::queue<int> q;
		if (visited.end() == visited.find(children[i])) {
			q.push(children[i]);
			visited.insert(children[i]);
		}
		while (!q.empty()) {
			int currNode = q.front();
			q.pop();
			std::vector<int> v = GetNeighbors(currNode);
			for (unsigned int j = 0; j < v.size(); j++) {
				if (visited.end() == visited.find(v[j])) {
					visited.insert(v[j]);
					q.push(v[j]);
					SetNextHop(v[j], children[i]);
				}
			}
		}
	}
	return true;
}

int Topology::GetNextHopTo(int id) {
	if (id < 0 || id > m_iSize) {
		return -1;
	} else {
		return m_pRoutingTable[id];
	}
}

bool Topology::SetNextHop(int dest, int nextHop) {
	if (nextHop < 0 || nextHop >= m_iSize || dest < 0 || dest >= m_iSize) {
		return false;
	}
	m_pRoutingTable[dest] = nextHop;
	return true;
}

} /* namespace tema3 */
