
#include "Stream.h"
#include <iostream>
using namespace std;

// Class : Stream
// Description : Content stream object. This object often has text stream. 
Stream::Stream()
{
	length = 0;
	subtype = "";
	stream = "";
}

Stream::~Stream()
{

}

void Stream::setLength(size_t length)
{
	this->length = length;
}

void Stream::setSubtype(string subtype)
{
	this->subtype = subtype;
}

void Stream::setStream(string stream)
{
	this->stream = stream;
}

// Class : Text
// Description : Content stream object. This has text content of document
Text::Text()
{
	for (int i = 0; i < 4; i++)
		this->bbox[i] = 0.0;

	for (int i = 0; i < 5; i++)
		this->matrix[i] = 0.0;

	this->usedFonts.clear();
}

vector<int>& Text::getUsedFonts()
{
	// If text object references identical object
	return this->usedFonts;
}

void Text::setBbox(int b1, int b2, int b3, int b4)
{
	this->bbox[0] = b1;
	this->bbox[1] = b2;
	this->bbox[2] = b3;
	this->bbox[3] = b4;
}

void Text::setResources(string resources)
{
	this->resources = resources;
}

void Text::setFontNo(int fontNo)
{
	this->usedFonts.push_back(fontNo);
}


void Text::setFormType(int formType)
{
	this->formType = formType;
}

//void Text::setNumOfUsedFonts(int num)
//{
//	this->numOfUsedFonts = num;
//}

void Text::setMatrix(double m1, double m2, double m3, double m4, double m5,
	double m6)
{
	this->matrix[0] = m1;
	this->matrix[1] = m2;
	this->matrix[2] = m3;
	this->matrix[3] = m4;
	this->matrix[4] = m5;
	this->matrix[5] = m6;
}

void Text::referenceObjectNo(int objectNo) {
	usedFonts.push_back(objectNo);
}

string Text::generateCode() {
	this->code.append(to_string(this->objectNo));	
	this->code.append(" 0 obj");	
	this->code.push_back('\n');
	this->code.append("<<\n");

		// Length
		this->code.append("/Length ");
		this->code.append(to_string(this->length));
		this->code.push_back('\n');

		// Font
		this->code.append("/Font ");
		this->code.push_back('[');

		for (unsigned int i = 1; i <= this->usedFonts.size(); i++)
		{
			// Identical font
			this->code.append("/F");
			this->code.push_back((char) (((int) '0') + i));
			this->code.push_back(' ');
		}

		this->code.push_back(']');
		this->code.push_back('\n');
	this->code.append(">>\n");

	this->code.append("stream\n");
	this->code.append(this->stream);
	this->code.push_back('\n');

	this->code.append("q\n");
	this->code.append("0 0 0 rg /a0 gs\n");
	this->code.append("BT\n");

	unsigned int previousPageNo = 0;

	// Text
	for (vector<LineSeg>::iterator i = this->lineSegs.begin();
		i != lineSegs.end();
		++i)
	{
		// Line segment
		if (i->getPageNo() != previousPageNo)
		{
			// This is first line segment in next page
			i->generateCode(this->code, true);
		}
		else
		{
			i->generateCode(this->code, false);
		}
		previousPageNo = i->getPageNo();
	}
	this->code.append("ET\n");
	this->code.append("Q\n");
	this->code.append("endstream\n");
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;
}


string Text::generateCode(size_t orgSize) {
	cout << "\nstring reserve" << endl;
	cout << "content ÁøÀÔÁ¡" << endl;
	cout << "code capacity : " << this->code.capacity() << endl << endl;
	
	// Reserve code string
	code.reserve(orgSize);
	
	this->code.append(to_string(this->objectNo));
	this->code.append(" 0 obj");
	this->code.push_back('\n');
	this->code.append("<<\n");

		// Length
		this->code.append("/Length ");
		this->code.append(to_string(this->length));
		this->code.push_back('\n');

		// Font
		this->code.append("/Font ");
		this->code.push_back('[');

		for (unsigned int i = 1; i <= this->usedFonts.size(); i++)
		{
			this->code.append("/F");
			this->code.push_back((char) (((int) '0') + i));
			this->code.push_back(' ');
		}

		this->code.push_back(']');
		this->code.push_back('\n');
	this->code.append(">>\n");

	this->code.append("stream\n");
	this->code.append(this->stream);
	this->code.push_back('\n');

	this->code.append("endstream\n");
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


vector<LineSeg>& Text::getLineSegs()
{
	return this->lineSegs;
}


void Text::setLineSegs(vector<LineSeg> lineSegs)
{
	this->lineSegs = lineSegs;
}
