#include "Trailer.h"

// Class : Trailer
// Description : set root of file body.
// Trailer also provides document information object(optional).
void Trailer::setSize(int size)
{
	this->size = size;
}

void Trailer::setRoot(int root)
{
	this->root = root;
}

string Trailer::generateCode()
{
	this->code.append("trailer");
	this->code.push_back('\n');
	this->code.append("<<");
		this->code.push_back('\n');

		// Size
		this->code.append("/Size ");
		this->code.append(to_string(this->size));
		this->code.push_back('\n');

		// Root
		this->code.append("/Root ");
		this->code.append(to_string(this->root));
		this->code.append(" 0 R");
		this->code.push_back('\n');
		//<< "/Info " << this->Info << '\n'

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("startxref");
	this->code.push_back('\n');

	this->code.append("492");

	// Replaced to byte size mesured by cross reference table
	this->code.append("\n%%EOF");

	this->code.shrink_to_fit();
	return this->code;
}

string Trailer::generateCode(size_t crossReferenceTableOffset)
{
	this->code.append("trailer");
	this->code.push_back('\n');
	this->code.append("<<");
		this->code.push_back('\n');

		// Size
		this->code.append("/Size ");
		this->code.append(to_string(this->size));
		this->code.push_back('\n');

		// Root
		this->code.append("/Root ");
		this->code.append(to_string(this->root));
		this->code.append(" 0 R");
		this->code.push_back('\n');
		//<< "/Info " << this->Info << '\n'
	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("startxref");
	this->code.push_back('\n');

	this->code.append(to_string(crossReferenceTableOffset));

	// Replaced to byte size mesured by cross reference table
	this->code.append("\n%%EOF");

	this->code.shrink_to_fit();
	return this->code;
}

