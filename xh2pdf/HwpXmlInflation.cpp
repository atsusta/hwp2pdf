#include "HwpXmlInflation.h"
#include <windows.h>

// Class : BodyTextInfo
// Description : Saves BodyText information in HWP file analysis
BodyTextInfo::BodyTextInfo()
{
	pageNo = 0;
	defaultTabStop = 0;
	tm0 = 0.0;
	tm1 = 0.0;
	tm2 = 0.0;
	tm3 = 0.0;
	tm4 = 0.0;
	tm5 = 0.0;
	upperLineSegY = 0;
	downLineSegY = 0;
	nFont = 0;
}

vector<Paragraph>& BodyTextInfo::getParagraphs()
{
	return this->Paragraphs;
}

vector<LineSeg>& BodyTextInfo::getLineSegs()
{
	return this->LineSegs;
}

vector<PageSize>& BodyTextInfo::getPageSizes()
{
	return this->PageSizes;
}

//vector<const char*>& BodyTextInfo::getImages()
//{
//	return this->Images;
//}

void BodyTextInfo::setDefaultTabStops(int defaultTabStop)
{
	this->defaultTabStop = defaultTabStop;
}

void BodyTextInfo::setNextPage()
{
	this->pageNo++;
}

unsigned int BodyTextInfo::getPageNo()
{
	return this->pageNo;
}

void BodyTextInfo::addNFont()
{
	this->nFont++;
}

void BodyTextInfo::resetNFont()
{
	this->nFont = 0;
}

int BodyTextInfo::getNFont()
{
	return this->nFont;
}

// Class : LineSeg
// Description : Line segment information archive from XML file
LineSeg::LineSeg()
{
	tm1 = 0.0;
	tm2 = 0.0;
	tm3 = 0.0;
	tm4 = 0.0;
	tm5 = 0.0;
	td0 = 0.0;
	td1 = 0.0;
	font = "";
	pageNo = 1;
}

LineSeg::~LineSeg()
{
}

double LineSeg::getTm0()
{
	return this->tm0;
}

double LineSeg::getTm1()
{
	return this->tm1;
}

double LineSeg::getTm2()
{
	return this->tm2;
}

double LineSeg::getTm3()
{
	return this->tm3;
}

double LineSeg::getTm4()
{
	return this->tm4;
}

double LineSeg::getTm5()
{
	return this->tm5;
}

double LineSeg::getTd0()
{
	return this->td0;
}

double LineSeg::getTd1()
{
	return this->td1;
}

string LineSeg::getFont()
{
	return this->font;
}

wstring LineSeg::getLineSeg()
{
	return this->lineSeg;
}

size_t LineSeg::getLength()
{
	return this->length;
}

unsigned int LineSeg::getPageNo()
{
	return this->pageNo;
}

void LineSeg::setTd(double td0, double td1)
{
	this->td0 = td0;
	this->td1 = td1;
}

void LineSeg::setTm(double tm0, double tm1, double tm2, double tm3, double tm4, double tm5)
{
	this->tm0 = tm0;
	this->tm1 = tm1;
	this->tm2 = tm2;
	this->tm3 = tm3;
	this->tm4 = tm4;
	this->tm5 = tm5;
}

void LineSeg::setSeg(wstring seg)
{
	this->lineSeg = seg;
}

void LineSeg::setFont(string fontName)
{
	this->font = fontName;
}

void LineSeg::setLength(size_t length)
{
	this->length = length;
}

void LineSeg::setPageNo(unsigned int pageNum)
{
	this->pageNo = pageNum;
}

void LineSeg::generateCode(string& code, bool isFirst)
{
	// Tm -> Td -> Tf -> TJ
	// Tm setting
	char cd[32];

	sprintf_s(cd, sizeof(cd), "%d %d %d %d %d %.2f Tm\n",
		(int)tm0, (int)tm1, (int)tm2, (int)tm3, (int)tm4, tm5);

	code.append(cd);

	// Td setting
	if (isFirst == false)
	{
		// td0(+) : 오른쪽으로, 
		// td0(-) : 왼쪽으로, 
		// td1(+) : 위로, 
		// td1(-) : 아래로
		sprintf_s(cd, sizeof(cd), "%.2f -%.2f Td\n", td0, td1);
		code.append(cd);
	}

	// Tf setting
	// More analysis required...
	code.append("/F1 1 Tf\n");

	// TJ - iterate unicode code point of characters for Hangul CID-Font

	// test
	//u_charFromName()
	//std::locale::global(std::locale("kor"));
	//wcout << L"유니코드잼" << endl;
	//wchar_t c = 0xac00;
	//wchar_t d = 0x0020;
	//wprintf(L"%c%c:%c<%x>\n", c, d,d, c);

	//cout << "TJ code : ";
	code.push_back('[');
	{
		for (wstring::iterator it = lineSeg.begin();
			it != lineSeg.end();
			++it)
		{
			char c[7];
			// Get code point
			wsprintf(c, "<%04x>", *it);
			for (int i = 0; i < 7; i++)
				code.push_back(c[i]);
		}
	}
	code.append("] TJ\n");
}

// Class : Paragraph
// Description : Paragraph information archive from XML file
Paragraph::Paragraph()
{
	pageNo = 1;
	lineSegs.clear();
}

Paragraph::~Paragraph()
{
}

vector<LineSeg>& Paragraph::getLineSegs()
{
	return this->lineSegs;
}

unsigned int Paragraph::getPageNo()
{
	return this->pageNo;
}

void Paragraph::setPageNo(unsigned int pageNum)
{
	this->pageNo = pageNum;
}

// Class : PageSize
// Description : saving page size & orientation
void PageSize::setMbox(double mbox0, double mbox1, double mbox2, double mbox3)
{
	this->mbox0 = mbox0;
	this->mbox1 = mbox1;
	this->mbox2 = mbox2;
	this->mbox3 = mbox3;
	this->appliedPageNo = 1;
}

double PageSize::getMbox0()
{
	return this->mbox0;
}

double PageSize::getMbox1()
{
	return this->mbox1;
}

double PageSize::getMbox2()
{
	return this->mbox2;
}

double PageSize::getMbox3()
{
	return this->mbox3;
}

unsigned PageSize::getAppliedPageNo()
{
	return this->appliedPageNo;
}

void PageSize::setAppliedPageNo(unsigned int appliedPageNo)
{
	this->appliedPageNo = appliedPageNo;
}
