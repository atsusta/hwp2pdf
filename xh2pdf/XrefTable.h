#pragma once

#include <unordered_map>

#include "Object.h"

/**
 * Class : XrefTable
 * Description : Cross-reference-table of Pdf object.
 * It provides document access and random pdf object access for PDF renderer.
 */
class XrefTable : private counter<Object>
{

public:
	XrefTable();
	~XrefTable();
	vector<std::pair<int, size_t>>& getOffsets();
	unordered_map<int, size_t>& getOffsetMap();
	string getCode();
	void setOffset(int objectNo, size_t size);
	string generateCode();

private:
	vector<std::pair<int, size_t>> offsets;
	unordered_map<int, size_t> offsetMap;
	string code;

};

