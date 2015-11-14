#include "CrossReferenceTable.h"
#include <iostream>

using namespace std;

// Class : CrossReferenceTable
// Description : Cross reference table of Pdf object.
// It provides random access for PDF viewer.
CrossReferenceTable::CrossReferenceTable()
{
	this->offsets.clear();
	// this->offsetMap.clear();
}

CrossReferenceTable::~CrossReferenceTable()
{

}

void CrossReferenceTable::setOffset(int objectNo, size_t size)
{
	if (offsets.empty())
	{
		this->offsets.push_back(std::make_pair(objectNo, size));
		cout << "object number : " << objectNo << 
			"\t next object's offset : " << size << endl;
	}
	else
	{
		size_t preObjSize = this->offsets.back().second;
		this->offsets.push_back(std::make_pair(objectNo, preObjSize + size));
		cout << "object number : " << objectNo << 
			"\t next object's offset : " << preObjSize + size << endl;
	}
}

vector<pair<int, size_t>>& CrossReferenceTable::getOffsets()
{
	return this->offsets;
}

/* Deprecated unordered_map version : getOffsetMap() */
//unordered_map<int, size_t>& CrossReferenceTable::getOffsetMap()
//{
//	return this->offsetMap;
//}

string CrossReferenceTable::getCode()
{
	return this->code;
}

string CrossReferenceTable::generateCode() {
	this->code.append("xref");
	this->code.push_back('\n');
	this->code.append("0 ");
	this->code.append(to_string(counter<Object>::objects_alive));
	this->code.push_back('\n');

	// Offset of header
	char bo[11] = { NULL };
	char rev[10] = { NULL };
	
	size_t xrefTableOffset = 0;

	cout << "\nsort offset of each objects" << endl;

	// Sort offset of each objects
	// 직전 code의 크기가 해당 object의 offset이 된다.
	for (vector<pair<int, size_t>>::reverse_iterator i = offsets.rbegin();
		i != offsets.rend();
		++i)
	{
		if (i == offsets.rbegin())
		{
			xrefTableOffset = i->second;
		}

		if (i == --offsets.rend())
		{
			// The offset of the first object is 0
			i->second = 0;
			cout << "---------------------------------\n"
				<< "object number : 0\t offset : " << i->second << endl;
		}
		else
		{
			// Byte size of previous object is byte offset of current object
			vector<pair<int, size_t>>::reverse_iterator j = i;
			++j;
			i->second = j->second;
			cout << "object number : " << i->first << 
				"\t offset : " << i->second << endl;
		}
	}

	// Cross reference table element code
	// This is ordered by object number
	for (int objectNo = 0; objectNo <= objects_alive; objectNo++)
	{ 
		// Find each number
		for (vector<pair<int, size_t>>::iterator i = offsets.begin();
			i != offsets.end();
			++i)
		{
			if (i->first == objectNo)
			{
				// Add byte offset
				sprintf_s(bo, sizeof(bo), "%010d", static_cast<int>(i->second));
				code.append(bo);
				code.push_back(' ');

				if (i == offsets.begin())
				{
					// There is revisions
					sprintf_s(rev, sizeof(rev), "%05d", 65535);
				}
				else
				{
                    // There is not revisions
					sprintf_s(rev, sizeof(rev), "%05d", 0);
				}
				code.append(rev);
				code.push_back(' ');

				if (i == offsets.begin())
				{
					// Marker, free
					code.push_back('f');
				}
				else
				{
					// Marker, in use
					code.push_back('n');
				}
				code.push_back('\n');
			}
		}
	}

	// Cross reference table offset setting
	this->offsets.push_back(make_pair(65535, xrefTableOffset));
	this->code.shrink_to_fit();
	
    return this->code;

	/* Deprecated unordered_map version - generate code */
	//for (unordered_map<int, size_t>::iterator findIter = this->offsetMap.begin();
	//	findIter != this->offsetMap.end();
	//	++findIter)
	//{
	//	cout << "object number : " << findIter->first << ", offset : " << findIter->second << endl;
	//}

	//// set each offset's to right position 
	//for (unordered_map<int, size_t>::iterator findIter = this->offsetMap.end();
	//	findIter != this->offsetMap.begin();
	//	--findIter)
	//{
	//	unordered_map<int, size_t>::iterator preFindIter = findIter;
	//	preFindIter--;
	//	cout << "preFindIter - object number : " << preFindIter->first << ", offset : " << preFindIter->second << endl;
	//	if (findIter == this->offsetMap.end())
	//	{
	//		preFindIter->second = 0;
	//		cout << "first object offset : " << preFindIter->second << endl;
	//	}
	//	
	//	else
	//	{
	//		findIter->second = preFindIter->second;
	//		cout << "object number : " << findIter->first << ", offset : " << findIter->second << endl;
	//	}
	//		
	//}

	//// offset of other objects
	//for (int i = 0; i < counter<Object>::objects_alive; ++i)
	//{
	//	unordered_map<int, size_t>::iterator findIter = this->offsetMap.find(i);
	//	if (findIter != offsetMap.end())
	//	{
	//		cout << "object number : " << findIter->first << ", next number object's offset : " << findIter->second << endl;
	//		sprintf_s(bo, sizeof(bo), "%010d", findIter->second); // byte offset
	//		code.append(bo);
	//		code.push_back(' ');

	//		sprintf_s(rev, sizeof(rev), "%05d", 0);
	//		code.append(rev);
	//		code.push_back(' ');

	//		code.push_back('n'); // marker
	//		code.push_back('\n');
	//	}
	//	else
	//		cout << "not found" << endl;
	//}

}

/* Deprecated array version - methods*/
//void CrossReferenceTable::setOffset(long int* offset, int objectSize) {
//	this->offset = new long[objectSize];
//		
//	for (int i = 0; i < objectSize; i++)
//		this->offset[i] = offset[i];
//	this->offset = offset;
//}

// offset mapping using sequencial collection class (vector)
// 'offset' is accumulated bytes from file front to pdf object's position. cross reference table is ordered by object's number.
//void CrossReferenceTable::setOffset(size_t size)
//{
//	if (!offsetCollection.empty())
//	{
//		size_t preObjSize = offsetCollection.back();
//		this->offsetCollection.push_back(preObjSize + size);
//	}
//	else
//		this->offsetCollection.push_back(size);
//}

// offset mapping using association collection class (unordered_map)
// convenient for ordering by object number 
//void CrossReferenceTable::setOffset(int objectNum, size_t size)
//{
//	if (!offsetMap.empty())
//	{
//		size_t preObjSize = offsetMap.begin()->second;
//		this->offsetMap.insert(this->offsetMap.begin(), unordered_map<int, size_t>::value_type(objectNum, preObjSize + size));
//		unsigned int n = offsetMap.bucket_count();
//		cout << "offsetMap has " << n << " buckets.\n";
//		cout << "object number : " << objectNum << ", next object's offset : " << preObjSize+size << endl;
//	}
//	else
//	{
//		this->offsetMap.insert(unordered_map<int, size_t>::value_type(objectNum, size));
//		cout << "object number : " << objectNum << ", next object's offset : " << size << endl;
//	}
//}

//void CrossReferenceTable::setMarker(char* isUse, int objectSize) {
//	this->marker = new char[objectSize];
//	
//	for (int i = 0; i < objectSize; i++) {
//		if (isUse[i] == 'n')
//			this->marker[i] = 'f';
//		else
//			this->marker[i] = 'n';
//	}
//	this->marker = isUse;
//}
//
//
//void CrossReferenceTable::setRevision(long int* answer, int objectSize) {
//	this->revision = new long[objectSize];
//		
//	for(int i = 0; i < objectSize; i++) {
//		if (answer[i] == 'y' || answer[i] == 'Y')
//			this->revision[i] = 65535;
//		else
//			this->revision[i] = 0;
//	}
//	this->revision = answer;
//}
