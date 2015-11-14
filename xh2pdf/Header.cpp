#include "Header.h"

// Class : Header
// Description : PDF header object
void Header::setPdfVersion(string pdfVersion)
{
	this->pdfVersion = pdfVersion;
}

string Header::generateCode()
{
	code.reserve(300);
	code.append(this->pdfVersion);	code.append("\n\n");
	this->code.shrink_to_fit();
	
	return this->code;
}

string Header::getCode()
{
	return this->code;
}
