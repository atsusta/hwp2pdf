#pragma once

#include "Object.h"

// #include <unordered_map>
// #include <iomanip>
#include <vector>

/**
 * Class : CrossReferenceTable
 * Description : Cross-reference-table of Pdf object.
 *  It provides document access and random pdf object access for PDF renderer.
 */
class CrossReferenceTable : private counter<Object>
{

public:
	CrossReferenceTable();
	~CrossReferenceTable();
	vector<std::pair<int, size_t>>& getOffsets();
	string getCode();
	void setOffset(int objectNo, size_t size);
	string generateCode();

	// unordered_map<int, size_t>& getOffsetMap();

private:
	vector<std::pair<int, size_t>> offsets;
	string code;

	// unordered_map<int, size_t> offsetMap;

};

