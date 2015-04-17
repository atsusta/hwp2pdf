#pragma once

#include "Dictionary.h"

#include <vector>
using namespace std;

/**
 * Class : LineSeg
 * Description : Line segment information archive from XML file
 */
class LineSeg
{

public:
	LineSeg();
	~LineSeg();
	double getTm0();
	double getTm1();
	double getTm2();
	double getTm3();
	double getTm4();
	double getTm5();
	double getTd0();
	double getTd1();
	string getFont();
	wstring getLineSeg();
	size_t getLength();
	unsigned int getPageNo();
	void setTd(double td0, double td1);
	void setTm(double tm0, double tm1, double tm2, double tm3, double tm4, 
		double tm5);
	void setSeg(wstring seg);
	void setFont(string fontName);
	void setLength(size_t length);
	void setPageNo(unsigned int pageNo);

	void generateCode(string& code, bool isFirst);

private:
	wstring lineSeg;
	string font;
	double tm0;
	double tm1;
	double tm2;
	double tm3;
	double tm4;
	double tm5;
	double td0;
	double td1;
	size_t length;
	unsigned int pageNo;

};

/**
 * Class : Paragraph
 * Description : Paragraph information archive from XML file
 * 
 */
class Paragraph
{

public:
	Paragraph();
	~Paragraph();
	unsigned int getPageNo();
	vector<LineSeg>& getLineSegs();
	void setPageNo(unsigned int pageNo);

private:
	unsigned int pageNo;
	vector<LineSeg> lineSegs;

};

/**
* Class : PageSize
* Description : saving page size & orientation
*/
class PageSize
{

public:
	double getMbox0();
	double getMbox1();
	double getMbox2();
	double getMbox3();
	unsigned int getAppliedPageNo();
	void setMbox(double mbox0, double mbox1, double mbox2, double mbox3);
	void setAppliedPageNo(unsigned int appliedPageNumber);

private:
	double mbox0;
	double mbox1;
	double mbox2;
	double mbox3;
	unsigned int appliedPageNo;
};

/**
* Class : BodyTextInfo
* Description : Saves BodyText information in analyzed HWP file (XML format)
*/
class BodyTextInfo
{

public:
	BodyTextInfo();
	unsigned int getPageNo();
	int getNFont();
	void setNextPage();
	void setDefaultTabStops(int defaultTabStop);

	vector<Paragraph>& getParagraphs();
	vector<LineSeg>& getLineSegs();
	vector<PageSize>& getPageSizes();

	void addNFont();
	void resetNFont();

private:
	unsigned int pageNo;
	int defaultTabStop;
	double tm0;
	double tm1;
	double tm2;
	double tm3;
	double tm4;
	double tm5;
	int upperLineSegY;
	int downLineSegY;
	unsigned int nFont;

	vector<Paragraph> Paragraphs;
	vector<LineSeg> LineSegs;
	vector<PageSize> PageSizes;

};
