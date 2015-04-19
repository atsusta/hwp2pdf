#pragma once

#include "HwpXmlInflation.h"
#include "Object.h"

#include <vector>
#include <string>

/**
 * Class : Stream
 * Description : super class for stream objects.
 *  It has byte-stream contents. And the byte-streams could be encoded.
 *  This is an abstract class.
 */
class Stream : public Object
{

public:
	Stream();
	~Stream();
	void setLength(size_t length);
	void setSubtype(string subtype);
	void setStream(string stream);
	
	virtual void referenceObjectNo(int objectNo) = 0;
	virtual string generateCode() = 0;

protected:
	size_t length;
	string subtype;
	string stream;

};

/**
 * Class : Text
 * Description : Content stream object. This object often has text stream.
 *  Depth 4 of PDF object hierarchy.
 *  If a text object references identical object,
 *  vector<int>& getUsedFonts() is needed.
 *  Text object references PrimaryFont object.
 */
class Text : public Stream
{

public:
	Text();
	vector<int>& getUsedFonts();
	vector<LineSeg>& getLineSegs();
	void setBbox(int b1, int b2, int b3, int b4);
	void setResources(string resource);
	void setLineSegs(vector<LineSeg> lineSegs);
	void setFontNo(int fontNo);
	void setFormType(int type);
	void setMatrix(double m1, double m2, double m3, double m4, double m5, 
		double m6);

	void referenceObjectNo(int objectNo);
	string generateCode();
	string generateCode(size_t orgSize);

private:
	double bbox[4];
	string resources;
	string structParents;
	string procSet;
	vector<int> usedFonts;
	int formType;
	double matrix[6];
	vector<LineSeg> lineSegs;

};
