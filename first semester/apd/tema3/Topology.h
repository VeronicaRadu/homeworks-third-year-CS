/*
 * Topology.h
 *
 *  Created on: Dec 22, 2015
 *      Author: arotaru
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <vector>
#include <iostream>
#include <fstream>

namespace tema3 {

#define SHOW_MATRIX 1
#define SHOW_ROUTING_TABLE 2

class Topology {
public:
	Topology(int);
	virtual ~Topology();

	void 	Combine(const Topology&);
	void 	Combine(const Topology*);
	std::vector<int> GetNeighbors(int);

	int 	GetSize() const;
	bool 	SetValue(int line, int col, int value);
	int		GetValue(int line, int col) const;

	bool 	ComputeRoutingTable(int forWho, int par);
	int 	GetNextHopTo(int id);
	bool 	SetNextHop(int dest, int nextHop);

	void Show(unsigned int, std::ostream&);

	friend std::ostream &operator<<(std::ostream &out, const Topology& t)     //output
	{
		out << t.m_iSize << " " << t.m_iSize << " ";
		out << t.m_iNonZeroCnt << " ";
		for(int i = 0; i < t.m_iSize; i++) {
			for(int j = i; j < t.m_iSize; j++) {
				if(1 == t.m_ppMatrix[i][j]) {
					out << i << " " << j << " ";
				}
			}
		}
		for(int i = 0; i < t.m_iSize; i++) {
			out << t.m_pRoutingTable[i] << " ";
		}
	    return out;
	}

	friend std::istream &operator>>(std::istream &in, Topology &t)     //input
	{
		int l, c;
		in >> l >> c;
		bool init = false;
		if(l != t.m_iSize) {
			t.Init(l);
			init = true;
		}
		int cnt;
		in >> cnt;

		for(int i = 0 ; i < cnt; i++) {
			int lin, col;
			in >> lin >> col;
			t.SetValue(lin, col, 1);
		}
		if(init) {
			for(int i = 0; i < t.m_iSize; i++) {
				in >> t.m_pRoutingTable[i];
			}
		}
	    return in;
	}

private:
	int 	m_iSize;
	int** 	m_ppMatrix;
	int*	m_pRoutingTable;
	int 	m_iNonZeroCnt;

	void Init(int s);
};

} /* namespace tema3 */

#endif /* TOPOLOGY_H_ */
