/*
 * Sudoku.h
 *
 *  Created on: Dec 28, 2015
 *      Author: arotaru
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <cstdlib>

namespace tema3 {

class SudokuTable {
#define uint unsigned int

public:
	SudokuTable() {
		m_Dimension = 0;
		m_ppMatrix = NULL;
	}

	SudokuTable(uint size) : m_Dimension(size) {
		uint *aux = new uint[m_Dimension * m_Dimension];
		m_ppMatrix = new uint*[m_Dimension];
		for(uint i = 0; i < m_Dimension; i++) {
			m_ppMatrix[i] = aux + i * m_Dimension;
		}
	}

	SudokuTable(const SudokuTable& rhs) : m_Dimension(rhs.m_Dimension) {
		uint *aux = new uint[m_Dimension * m_Dimension];
		m_ppMatrix = new uint*[m_Dimension];
		for(uint i = 0; i < m_Dimension; i++) {
			m_ppMatrix[i] = aux + i * m_Dimension;
			for(uint j = 0; j < m_Dimension; j++) {
				m_ppMatrix[i][j] = rhs.m_ppMatrix[i][j];
			}
		}
	}

	SudokuTable& operator=(const SudokuTable& rhs) {
		if(this == &rhs) {
			return *this;
		}
		if(NULL != m_ppMatrix) {
			delete[] *m_ppMatrix;
		}
		m_Dimension = rhs.m_Dimension;
		uint *aux = new uint[m_Dimension * m_Dimension];
		m_ppMatrix = new uint*[m_Dimension];
		for(uint i = 0; i < m_Dimension; i++) {
			m_ppMatrix[i] = aux + i * m_Dimension;
			for(uint j = 0; j < m_Dimension; j++) {
				m_ppMatrix[i][j] = rhs.m_ppMatrix[i][j];
			}
		}
		return *this;
	}

	~SudokuTable() {
		delete[] *m_ppMatrix;
	}

	void PrintTo(std::ostream& out) {
		for(uint i = 0; i < m_Dimension; i++) {
			for(uint j = 0; j < m_Dimension; j++) {
				out << m_ppMatrix[i][j] << " ";
			}
			out << "\n";
		}
	}

	friend std::ostream &operator<<(std::ostream &out, const SudokuTable& t)     //output
	{
		out << t.m_Dimension << " ";
		for(uint i = 0; i < t.m_Dimension; i++) {
			for(uint j = 0; j < t.m_Dimension; j++) {
				if(0 != t.m_ppMatrix[i][j]) {
					out << i << " " << j << " " << t.m_ppMatrix[i][j] << " ";
				}
			}
		}
	    return out;
	}

	friend std::istream &operator>>(std::istream &in, SudokuTable &t)
	{
		uint d;
		in >> d;
		if(t.m_Dimension != d) {
			return in;
		}
		int l = 0, c = 0, v = 0;
		while(!in.eof()) {
			in >> l >> c >> v;
			t.m_ppMatrix[l][c] = v;
		}
		   return in;
	}

	bool SetBox(uint line, uint col, uint value) {
		bool bRet = IsValid(line, col, value);
		if(bRet) {
			m_ppMatrix[line][col] = value;
		}
		return bRet;
	}

	uint GetBox(uint line, uint col) const {
		if(line >= m_Dimension || col >= m_Dimension) {
			return -1;
		}
		return m_ppMatrix[line][col];
	}

	uint GetDimension() const {
		return m_Dimension;
	}

private:
	uint 	m_Dimension;
	uint**	m_ppMatrix;

	bool IsValid(uint l, uint c, uint v) {
		if(l >= m_Dimension || c >= m_Dimension) {
			return false;
		}

		if(0 == v) {
			return true;
		}
		if(v == m_ppMatrix[l][c]) {
			return true;
		}

		for(uint i = 0; i < m_Dimension; i++) {
			if(v == m_ppMatrix[l][i] || v == m_ppMatrix[i][c]) {
				return false;
			}
		}

		return true;
	}
};

/*lowerBound<=x<=upperBound*/
class SudokuMatrixPart {
public:
	SudokuMatrixPart() {
		m_iEndColumn = 0;
		m_iEndLine = 0;
		m_iStartColumn = 0;
		m_iStartLine = 0;
	}
	SudokuMatrixPart(uint sl, uint el, uint sc, uint ec)
	: m_iStartLine(sl)
	, m_iEndLine(el)
	, m_iStartColumn(sc)
	, m_iEndColumn(ec)
	{}

	uint GetBeginLine() const {
		return m_iStartLine;
	}

	uint GetEndingLine() const {
		return m_iEndLine;
	}

	uint GetBeginColumn() const {
		return m_iStartColumn;
	}

	uint GetEndingColumn() const {
		return m_iEndColumn;
	}

	std::pair<uint, uint> GetNextBox(uint l, uint c) {
		if(c < m_iEndColumn) {
			c++;
		} else if (c == m_iEndColumn) {
			c = m_iStartColumn;
			l++;
		}
		return std::make_pair(l, c);
	}

	bool IsEnd(uint l, uint c) {
		return l > m_iEndLine;
	}

private:
	uint m_iStartLine;
	uint m_iEndLine;
	uint m_iStartColumn;
	uint m_iEndColumn;
};


class SudokuTableReader {
public:
	SudokuTableReader(std::string p) {
		m_sPath = p;
		m_stTable = NULL;
		_ReadAndStore();
	}

	SudokuTable* GetTable() const {
		return m_stTable;
	}

	uint GetProblemDimension() const {
		return m_ProblemDimension;
	}

	SudokuMatrixPart GetTaskPart(uint pId) {
		int l = pId / m_ProblemDimension;
		int c = pId % m_ProblemDimension;
		return SudokuMatrixPart(l * m_ProblemDimension
				, l * m_ProblemDimension + m_ProblemDimension - 1
				, c * m_ProblemDimension
				, c * m_ProblemDimension + m_ProblemDimension - 1);
	}

private:
	std::string 	m_sPath;
	SudokuTable*	m_stTable;
	uint 			m_ProblemDimension;

	void _ReadAndStore() {
		std::ifstream in (m_sPath.c_str(), std::ifstream::in);
		in >> m_ProblemDimension;
		m_stTable = new SudokuTable(m_ProblemDimension * m_ProblemDimension);
		for(uint i = 0; i < m_ProblemDimension * m_ProblemDimension; i++) {
			for(uint j = 0; j < m_ProblemDimension * m_ProblemDimension; j++) {
				int x;
				in >> x;
				m_stTable->SetBox(i, j, x);
			}
		}
		in.close();
	}
};
}

#endif /* SUDOKU_H_ */
