#include "stdafx.h"
#include "Pdf.h"
#include <iomanip>
using namespace std;

// Class : Header, Catalog, Pages, Page, PrimaryFont, SubFont, FontDescriptor, xrefTable, Trailer
// Description : All member functions in Pdf structure classes

// Function : void generateCode(string& target), void referenceObjectNumber(int objectNum)
// Description : Implementations of Functions that generate Pdf structure code
// Parameters : stringstream& target - the target stream of string for Pdf structure code, int objectNum - the number of object that is referenced by
// Return Value : none
//
// Revisions :
// 1. When : 140606
// 	What : getnerateCode(string& target) argument changed to string* ... failed.
//	If so, "target << " code is unavailable because << operator need to be additionally overloaded!! ( POINTER << STUFFS )
//
// 2. When : 140607
//	What : implementation error correction
//	two functions must be implemented by all derived subclasses.
//
// 3. When : 140616
// What : stringstream type inspection
// runtime-error cannot be conquered...
//
// 4. When : 140705
// What : generateCode() change
// using string variable / string.append(), string.push_back() function
//
// 5. When : 140803
// What : ProducePdf()
// use case realization 

Stream::Stream()
{
	length = 0;
	subtype = "";
	stream = "";
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
	pageNumber = 1;
}


LineSeg::~LineSeg()
{
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


void LineSeg::setPageNumber(unsigned int pageNum)
{
	this->pageNumber = pageNum;
}

unsigned int LineSeg::getPageNumber()
{
	return this->pageNumber;
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
		sprintf_s(cd, sizeof(cd), "%.2f -%.2f Td\n",  // td0(+) : 오른쪽으로, td0(-) : 왼쪽으로, td1(+) : 위로, td1(-) : 아래로
			td0, td1);
		code.append(cd);
	}

	// Tf setting
	code.append("/F1 1 Tf\n"); // more analysis required...

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
			//wprintf(L"<%04x>", *it);
			wsprintf(c, "<%04x>", *it); // get code point
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
	pageNumber = 1;
	LineSegCollection.clear();
}


Paragraph::~Paragraph()
{
}


vector<LineSeg>& Paragraph::getLineSegCollection()
{
	return this->LineSegCollection;
}


void Paragraph::setPageNumber(unsigned int pageNum)
{
	this->pageNumber = pageNum;
}

unsigned int Paragraph::getPageNumber()
{
	return this->pageNumber;
}


// Class : PageSize
// Description : saving page size & orientation
void PageSize::setMbox(double mbox0, double mbox1, double mbox2, double mbox3)
{
	this->mbox0 = mbox0;
	this->mbox1 = mbox1;
	this->mbox2 = mbox2;
	this->mbox3 = mbox3;
	this->appliedPageNumber = 1;
}

void PageSize::setAppliedPageNumber(unsigned int appliedPageNumber)
{
	this->appliedPageNumber = appliedPageNumber;
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

unsigned PageSize::getAppliedPageNumber()
{
	return this->appliedPageNumber;
}


// Class : BodyTextInfo
// Description : Saves BodyText information in HWP file analysis
BodyTextInfo::BodyTextInfo()
{
	pageNumber = 0;
	defaultTabStop = 0;
	tm0 = 0.0;
	tm1 = 0.0;
	tm2 = 0.0;
	tm3 = 0.0;
	tm4 = 0.0;
	tm5 = 0.0;
	upperLineSegY = 0;
	downLineSegY = 0;
	numOfFont = 0;
}

vector<Paragraph>& BodyTextInfo::getParagraphCollection()
{
	return this->ParagraphCollection;
}

vector<LineSeg>& BodyTextInfo::getLineSegCollection()
{
	return this->LineSegCollection;
}

vector<PageSize>& BodyTextInfo::getPageSizeCollection()
{
	return this->PageSizeCollection;
}

//vector<const char*>& BodyTextInfo::getImageCollection()
//{
//	return this->ImageCollection;
//}

void BodyTextInfo::setDefaultTabStops(int defaultTabStop)
{
	this->defaultTabStop = defaultTabStop;
}

void BodyTextInfo::setNextPage()
{
	this->pageNumber++;
}

unsigned int BodyTextInfo::getPageNumber()
{
	return this->pageNumber;
}

void BodyTextInfo::addNumOfFont()
{
	this->numOfFont++;
}

void BodyTextInfo::resetNumOfFont()
{
	this->numOfFont = 0;
}

int BodyTextInfo::getNumOfFont()
{
	return this->numOfFont;
}


// Class : Header
// Description : PDF header object
void Header::setPDFVersion(string version) {
	this->PDFVersion = version;
}


string Header::generateCode() {
	//cout << "ppss" << endl;
	code.reserve(300);
	code.append(this->PDFVersion);	code.append("\n\n");
	//cout << code.size() << endl;
	this->code.shrink_to_fit();
	return this->code;
}


string Header::getCode()
{
	return this->code;
}


// Class : Object
// Description : PDF object class. this is an abtract class. inherit attributes and operations.
Object::Object()
{
	ObjectNumber = 65535;
	code.reserve(320);
}


int Object::getObjectNumber() {
	return this->ObjectNumber;
}


string Object::getCode()
{
	return this->code;
}


void Object::setObjectNumber(int num)
{
	this->ObjectNumber = num;
}


void Object::setType(string type)
{
	this->Type = type;
}


// Class : Catalog
// Description : Root of pdf object hierarchy
Catalog::Catalog() {
	this->setType("/Catalog");
}


void Catalog::referenceObjectNumber(int objectNum) {
	this->Pages = objectNum;
}


string Catalog::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n'); //"/Catalog"
	this->code.append("/Pages ");	this->code.append(to_string(this->Pages));	this->code.append(" 0 R");	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return code;
}


// Class : Page
// Description : Pdf page object. it looks parent
Page::Page() {
	this->setType("/Page");
	for (int i = 0; i < 4; i++) {
		MediaBox[i] = 0.0;
		Cropbox[i] = 0.0;
	}
	this->Parent = 0;
	this->Contents = 0;
	this->Rotate = 0;
}


int Page::getParent() {
	return this->Parent;
}


int Page::getContents() {
	return this->Contents;
}


void Page::setMediaBox(double m1, double m2, double m3, double m4) {
	this->MediaBox[0] = m1;
	this->MediaBox[1] = m2;
	this->MediaBox[2] = m3;
	this->MediaBox[3] = m4;
}


void Page::setCropBox(double c1, double c2, double c3, double c4) {
	this->Cropbox[0] = c1;
	this->Cropbox[1] = c2;
	this->Cropbox[2] = c3;
	this->Cropbox[3] = c4;
}


void Page::setContents(int ref) {
	this->Contents = ref;
}


void Page::setResources(int resource) {
	this->fontResourceCollection.push_back(resource);
}


void Page::referenceObjectNumber(int objectNum) {
	this->Parent = objectNum;
}


string Page::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n');
	this->code.append("/Parent ");	this->code.append(to_string(this->Parent));	this->code.append(" 0 R");	this->code.push_back('\n');
	this->code.append("/MediaBox "); this->code.append("[ ");
	for (int i = 0; i < 4; i++)
	{
		this->code.append(to_string(this->MediaBox[i]));	this->code.append(" ");
	}
	this->code.append("]");	this->code.push_back('\n');

	this->code.append("/Cropbox [ ");

	for (int i = 0; i < 4; i++)
	{
		this->code.append(to_string(this->Cropbox[i]));	this->code.append(" ");
	}
	this->code.append("]");	this->code.push_back('\n');

	this->code.append("/Resources ");	this->code.append("<<");	this->code.push_back('\n');
	this->code.append("\t");	this->code.append("/ExtGState ");	this->code.append("<<");	this->code.push_back('\n');
	this->code.append("\t\t");	this->code.append("/a0 ");	this->code.append("<<");	this->code.append(" /CA 1 /ca 1");	this->code.append(" >>");	this->code.push_back('\n');
	this->code.append("\t");	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("\t");	this->code.append("/Font ");	this->code.append("<<"); this->code.push_back('\n');

	int i = 1;
	// fontResourceCollection has object number for font object
	for (vector<int>::iterator frIterPos = fontResourceCollection.begin();
		frIterPos != fontResourceCollection.end();
		++frIterPos)
	{
		this->code.append("\t\t");
		this->code.append("/F"); this->code.append(to_string(i));	this->code.push_back(' ');
		this->code.append(to_string(*frIterPos));	this->code.append(" 0 R");
		this->code.push_back('\n');
	}

	this->code.append("\t");	this->code.append(">>");	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("/Contents ");	this->code.append(to_string(this->Contents));	this->code.append(" 0 R");	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


// Class : Pages
// Description : Parent page. it manages child pages.
Pages::Pages() {
	this->setType("/Pages");
	this->Count = 0;
}


int Pages::getCount() {
	return this->Count;
}


void Pages::setCount() {
	this->Count++;
}


void Pages::referenceObjectNumber(int reference) { // get page object
	this->Kids.push_back(reference);
}


string Pages::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n'); //"/Pages"
	this->code.append("/Count ");	this->code.append(to_string(this->Count));	this->code.push_back('\n');

	this->code.append("/Kids ");	this->code.append("[");
	for (vector<int>::iterator IterPos = Kids.begin();
		IterPos != Kids.end();
		++IterPos)
	{
		this->code.push_back(' ');
		this->code.append(to_string(*IterPos));	this->code.append(" 0 R");
	}
	this->code.append(" ]");	this->code.push_back('\n');

	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


// Class : Image
void Image::referenceObjectNumber(int ref) {

}

string Image::generateCode() {
	return "";
}


// Class : SubFont
// Description : CIDFont system can be used for Subfont to present CJK(Chinese-Japanese-Korean) language based font.
// CIDFonts need Font Descriptor for specifying their identity.
// !! ALL SUBFONTS MUST BE PRESENTED AS ARRAY SEQUENCE IN PDF CODE !!
void SubFont::referenceObjectNumber(int ref) { // hollow implementation
	if (this->FontDescriptor == 0)
		this->FontDescriptor = ref;
}


SubFont::SubFont() {
	this->setType("/Font");
	this->setSubtype("/CIDFontType2");
	this->FontDescriptor = 0;
	for (int i = 0; i < 3; i++)
		this->W[i] = 0;
}


string SubFont::getCIDSystemInfo() {
	return this->CIDSystemInfo;
}


void SubFont::setBaseFont(string fontName) {
	this->BaseFont = fontName;
}


void SubFont::setFontDescriptor(int ref) {
	this->FontDescriptor = ref;
}


void SubFont::setW(int w1, int w2, int w3) {
	this->W[0] = w1;
	this->W[1] = w2;
	this->W[2] = w3;
}


void SubFont::setDW(int w) {
	this->DW = w;
}


void SubFont::setCIDSystemInfo(string info) {
	this->CIDSystemInfo = info;
}


string SubFont::generateCode()
{
	//this->code.append(to_string(this->ObjectNumber - 1));	this->code.append(" 0 obj");	this->code.push_back('\n');
	//this->code.append("[ ");	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 R ]");	this->code.push_back('\n');
	//this->code.append("endobj\n\n");

	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n'); //"/Font"
	this->code.append("/Subtype ");	this->code.append(this->Subtype);	this->code.push_back('\n');
	this->code.append("/FontDescriptor ");	this->code.append(to_string(this->FontDescriptor));	this->code.append(" 0 R");	this->code.push_back('\n');
	this->code.append("/BaseFont ");	this->code.push_back('/');	this->code.append(this->BaseFont);	this->code.push_back('\n');
	this->code.append("/W");	this->code.append(" [ ");
	for (int i = 0; i < 3; i++) {
		this->code.append(to_string(this->W[i]));	this->code.append(" ");
	}
	this->code.append("]");	this->code.push_back('\n');
	//this->code // temporary code
	this->code.append("/CIDSystemInfo ");	this->code.append(this->CIDSystemInfo);	this->code.push_back('\n');
	this->code.append("/DW ");	this->code.append(to_string(this->DW));	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;

	//char cd[32];
	//sprintf(cd, "%d 0 obj\n", this->ObjectNumber - 1);
	//sprintf(cd, "[ %d 0 R ]\n", this->ObjectNumber);
	//code.append("endobj\n\n");
}


// Class : FontDescriptor
// Description : descript CIDFont font family.
// It implies core informations about CIDFont such as glyph informations.
FontDescriptor::FontDescriptor() {
	this->setType("/FontDescriptor");
	for (int i = 0; i < 4; i++)
		FontBBox[i] = 0;
}

void FontDescriptor::setFontName(string fontName) {
	this->FontName = fontName;
}

void FontDescriptor::setFontFamily(string fontFamily) {
	this->FontFamily = fontFamily;
}

void FontDescriptor::setFlags(int flags) {
	this->Flags = flags;
}

void FontDescriptor::setFontBBox(int b1, int b2, int b3, int b4) {
	this->FontBBox[0] = b1;
	this->FontBBox[1] = b2;
	this->FontBBox[2] = b3;
	this->FontBBox[3] = b4;
}

void FontDescriptor::setItalicAngle(int angle) {
	this->ItalicAngle = angle;
}

void FontDescriptor::setAscent(int ascent) {
	this->Ascent = ascent;
}

void FontDescriptor::setDescent(int descent) {
	this->Descent = descent;
}

void FontDescriptor::setCapHeight(int height) {
	this->CapHeight = height;
}

void FontDescriptor::setStemV(int h) {
	this->StemH = h;
}

void FontDescriptor::setStemH(int v) {
	this->StemV = v;
}

void FontDescriptor::setFontStretch(string stretch) {
	this->FontStretch = stretch;
}

void FontDescriptor::setFontWeight(int weight) {
	this->FontWeight = weight;
}

void FontDescriptor::setXHeight(int height) {
	this->XHeight = height;
}

void FontDescriptor::referenceObjectNumber(int ref) { // hollow implementation

}

string FontDescriptor::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n');
	this->code.append("/FontName ");	this->code.push_back('/');	this->code.append(this->FontName);	this->code.push_back('\n');
	this->code.append("/FontFamily ");	this->code.push_back('(');	this->code.append(this->FontFamily);	this->code.push_back(')');	this->code.push_back('\n');
	this->code.append("/Flags ");	this->code.append(to_string(this->Flags));	this->code.push_back('\n');
	this->code.append("/FontBBox ");	this->code.append(" [ ");
	for (int i = 0; i < 4; i++) {
		this->code.append(to_string(this->FontBBox[i]));	this->code.append(" ");
	}
	this->code.append(" ]");	this->code.push_back('\n');

	this->code.append("/ItalicAngle ");	this->code.append(to_string(this->ItalicAngle));	this->code.push_back('\n');
	this->code.append("/Ascent ");	this->code.append(to_string(this->Ascent));	this->code.push_back('\n');
	this->code.append("/Descent ");	this->code.append(to_string(this->Descent));	this->code.push_back('\n');
	this->code.append("/CapHeight ");	this->code.append(to_string(this->CapHeight));	this->code.push_back('\n');
	this->code.append("/StemV ");	this->code.append(to_string(this->StemV));	this->code.push_back('\n');
	this->code.append("%/StemH ");	this->code.append(to_string(this->StemH));	this->code.push_back('\n');
	this->code.append("/FontWeight ");	this->code.append(to_string(this->FontWeight));	this->code.push_back('\n');
	this->code.append("/FontStretch ");	this->code.append(this->FontStretch);	this->code.push_back('\n');
	this->code.append("/XHeight ");	this->code.append(to_string(this->XHeight));	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


// Class : PrimaryFont
// Description : contains font name, encoding, subfont information.
PrimaryFont::PrimaryFont() {
	this->setType("/Font");
	this->SubFont.clear();
	this->Encoding = "none";
}


void PrimaryFont::setBaseFont(string fontName) {
	this->BaseFont = fontName;
}


void PrimaryFont::setEncoding(string encoding) {
	this->Encoding = encoding;
}


void PrimaryFont::setDescendantFonts(int ref) {
	this->SubFont.push_back(ref);
}


void PrimaryFont::referenceObjectNumber(int ref) {
	this->SubFont.push_back(ref);
}


string PrimaryFont::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Type ");	this->code.append(this->Type);	this->code.push_back('\n'); //"/Font"
	this->code.append("/Subtype ");	this->code.append(this->Subtype);	this->code.push_back('\n');
	this->code.append("/BaseFont ");	this->code.push_back('/');	this->code.append(this->BaseFont);	this->code.push_back('\n');

	// cid subfont dictionary code
	if (this->SubFont.size() > 0)
		this->code.append("/DescendantFonts [");
	for (vector<int>::iterator IterPos = this->SubFont.begin();
		IterPos != this->SubFont.end();
		++IterPos)
	{
		this->code.append(to_string(*IterPos));
		this->code.append(" 0 R");
	}
	this->code.append(" ]");
	this->code.push_back('\n');

	if (this->Encoding != "none")
		this->code.append("/Encoding ");	this->code.append(this->Encoding);	this->code.push_back('\n');
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}

// Class : Text
// Description : Content stream object. This object often has text stream.
Text::Text() {
	for (int i = 0; i < 4; i++)
		this->Bbox[i] = 0.0;
	for (int i = 0; i < 5; i++)
		this->Matrix[i] = 0.0;
	this->usedFonts.clear();
}


vector<int>& Text::getUsedFonts() { // if text object references identical object
	return this->usedFonts;
}


void Text::setBbox(int b1, int b2, int b3, int b4) {
	this->Bbox[0] = b1;
	this->Bbox[1] = b2;
	this->Bbox[2] = b3;
	this->Bbox[3] = b4;
}


void Text::setResources(string resource) {
	this->Resources = resource;
}


void Text::setFontNumber(int fontNumber) {
	this->usedFonts.push_back(fontNumber);
}


void Text::setFormType(int type) {
	this->FormType = type;
}


//void Text::setNumOfUsedFonts(int num)
//{
//	this->numOfUsedFonts = num;
//}


void Text::setMatrix(double m1, double m2, double m3, double m4, double m5, double m6) {
	this->Matrix[0] = m1;
	this->Matrix[1] = m2;
	this->Matrix[2] = m3;
	this->Matrix[3] = m4;
	this->Matrix[4] = m5;
	this->Matrix[5] = m6;
}


void Text::referenceObjectNumber(int ref) {
	//usedFonts.push_back(ref);
}


string Text::generateCode() {
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Length ");	this->code.append(to_string(this->Length));	this->code.push_back('\n');
	this->code.append("/Font ");	this->code.push_back('[');

	for (unsigned int i = 1; i <= this->usedFonts.size(); i++)
	{
		this->code.append("/F");
		this->code.push_back((char)(((int)'0') + i));
		this->code.push_back(' ');
	}

	this->code.push_back(']');	this->code.push_back('\n');
	this->code.append(">>\n");
	this->code.append("stream\n");
	this->code.append(this->stream);	this->code.push_back('\n');

	this->code.append("q\n");
	this->code.append("0 0 0 rg /a0 gs\n");
	this->code.append("BT\n");

	unsigned int previousPageNumber = 0;

	// Text
	for (vector<LineSeg>::iterator lIterPos = this->InPageLineSegCollection.begin();
		lIterPos != InPageLineSegCollection.end();
		++lIterPos)
	{
		// line segment
		if (lIterPos->getPageNumber() != previousPageNumber) // is first line segment in next page
		{
			lIterPos->generateCode(this->code, true);
		}
		else
		{
			lIterPos->generateCode(this->code, false);
		}
		previousPageNumber = lIterPos->getPageNumber();
	}
	this->code.append("ET\n");
	this->code.append("Q\n");
	this->code.append("endstream\n");
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


string Text::generateCode(size_t orgSize) {
	cout << "\nstring reserve" << endl;
	code.reserve(orgSize);
	cout << "content 진입점" << endl;
	cout << "code capacity : " << this->code.capacity() << endl << endl;
	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 obj");	this->code.push_back('\n');
	this->code.append("<<\n");
	this->code.append("/Length ");	this->code.append(to_string(this->Length));	this->code.push_back('\n');
	this->code.append("/Font ");	this->code.push_back('[');

	for (unsigned int i = 1; i <= this->usedFonts.size(); i++)
	{
		this->code.append("/F");
		this->code.push_back((char)(((int)'0') + i));
		this->code.push_back(' ');
	}

	this->code.push_back(']');	this->code.push_back('\n');
	this->code.append(">>\n");
	this->code.append("stream\n");
	this->code.append(this->stream);	this->code.push_back('\n');
	this->code.append("endstream\n");
	this->code.append("endobj\n\n");
	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


vector<LineSeg>& Text::getInPageLineSegCollection()
{
	return this->InPageLineSegCollection;
}


void Text::setInPageLineSegCollection(vector<LineSeg> lineSegCollection)
{
	this->InPageLineSegCollection = lineSegCollection;
}


// Class : XrefTable
// Description : Cross reference table of Pdf object.
// It provides random access for PDF viewer.
XrefTable::XrefTable() {
	//this->offset = NULL;
	this->offsetCollection.clear();
	this->offsetMap.clear();
	//this->revision = NULL;
	//this->marker = NULL;
}


XrefTable::~XrefTable() {

}


// using array
//void XrefTable::setOffset(long int* offset, int objectSize) {
//	this->offset = new long[objectSize];
//		
//	for (int i = 0; i < objectSize; i++)
//		this->offset[i] = offset[i];
//	this->offset = offset;
//}


// offset mapping using sequencial collection class (vector)
// 'offset' is accumulated bytes from file front to pdf object's position. cross reference table is ordered by object's number.
//void XrefTable::setOffset(size_t size)
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
//void XrefTable::setOffset(int objectNum, size_t size)
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


void XrefTable::setOffset(int objectNum, size_t size)
{
	if (offsetCollection.empty())
	{
		this->offsetCollection.push_back(std::make_pair(objectNum, size));
		cout << "object number : " << objectNum << "\t next object's offset : " << size << endl;
	}
	else
	{
		size_t preObjSize = this->offsetCollection.back().second;
		this->offsetCollection.push_back(std::make_pair(objectNum, preObjSize + size));
		cout << "object number : " << objectNum << "\t next object's offset : " << preObjSize + size << endl;
	}
}


//void XrefTable::setMarker(char* isUse, int objectSize) {
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
//void XrefTable::setRevision(long int* answer, int objectSize) {
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


//vector<size_t>& XrefTable::getOffsetCollection()
//{
//	return this->offsetCollection;
//}


vector<std::pair<int, size_t>>& XrefTable::getOffsetCollection()
{
	return this->offsetCollection;
}


unordered_map<int, size_t>& XrefTable::getOffsetMap()
{
	return this->offsetMap;
}


string XrefTable::getCode()
{
	return this->code;
}


string XrefTable::generateCode() {
	this->code.append("xref");	this->code.push_back('\n');
	this->code.append("0 ");	this->code.append(to_string(counter<Object>::objects_alive));	this->code.push_back('\n');

	//for(int i = 0; i < counter<Object>::objects_created+1; i++)
	//{
	//	code
	//		<< setfill('0') << setw(10) << this->offset[i] << " "
	//		<< setfill('0') << setw(5) << this->revision[i] << " "
	//		<< this->marker[i] <<  '\n';
	//}

	// offset of header
	char bo[11] = { NULL };
	//sprintf_s(bo, sizeof(bo), "%010d", 0); // byte offset
	//for (int i = 0; bo[i] != NULL; i++)
	//	code.push_back(bo[i]);
	//code.push_back(' ');

	char rev[10] = { NULL };
	//sprintf_s(rev, sizeof(rev), "%05d", 65535); // revision
	//code.append(rev);
	//code.push_back(' ');
	//
	//code.push_back('f'); // marker
	//code.push_back('\n');

	size_t xrefTableOffset = 0;

	cout << "\nsort offset of each objects" << endl;

	// sort offset of each objects
	// 직전 code의 크기가 해당 object의 offset이 된다.
	for (vector<std::pair<int, size_t>>::reverse_iterator szIterPos = offsetCollection.rbegin();
		szIterPos != offsetCollection.rend();
		++szIterPos)
	{
		if (szIterPos == offsetCollection.rbegin())
		{
			xrefTableOffset = szIterPos->second;
		}

		if (szIterPos == --offsetCollection.rend())
		{
			szIterPos->second = 0; // #object 0 : offset 0
			cout << "---------------------------------\n"
				<< "object number : 0\t offset : " << szIterPos->second << endl;
		}
		else
		{
			// set byte size of previous object as byte offset of present object
			vector<std::pair<int, size_t>>::reverse_iterator reverseNextSzIterPos = szIterPos;
			++reverseNextSzIterPos;
			szIterPos->second = reverseNextSzIterPos->second;
			cout << "object number : " << szIterPos->first << "\t offset : " << szIterPos->second << endl;
		}
	}

	// cross reference table element code
	// ordered by object number
	for (int objectNum = 0; objectNum <= objects_alive; objectNum++)
	{ // find each number
		for (vector<std::pair<int, size_t>>::iterator szIterPos = offsetCollection.begin();
			szIterPos != offsetCollection.end();
			++szIterPos)
		{
			if (szIterPos->first == objectNum)
			{
				sprintf_s(bo, sizeof(bo), "%010d", static_cast<int>(szIterPos->second)); // byte offset
				code.append(bo);
				code.push_back(' ');

				if (szIterPos == offsetCollection.begin())
					sprintf_s(rev, sizeof(rev), "%05d", 65535); // revision yes
				else
					sprintf_s(rev, sizeof(rev), "%05d", 0); // revision no
				code.append(rev);
				code.push_back(' ');

				if (szIterPos == offsetCollection.begin())
					code.push_back('f'); // marker, free
				else
					code.push_back('n'); // marker, in use
				code.push_back('\n');
			}
		}
	}

	// cross reference table offset setting
	this->offsetCollection.push_back(make_pair(65535, xrefTableOffset));

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

	//this->code.push_back('\0');
	this->code.shrink_to_fit();
	return this->code;
}


// Class : Trailer
// Description : set root of file body.
// Trailer also provides document information object(optional).
void Trailer::setSize(int size) {
	this->Size = size;
}


void Trailer::setRoot(int root) {
	this->Root = root;
}


string Trailer::generateCode() {
	this->code.append("trailer");	this->code.push_back('\n');
	this->code.append("<<");	this->code.push_back('\n');
	this->code.append("/Size ");	this->code.append(to_string(this->Size));	this->code.push_back('\n');
	this->code.append("/Root ");	this->code.append(to_string(this->Root));	this->code.append(" 0 R");	this->code.push_back('\n');
	//<< "/Info " << this->Info << '\n'
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("startxref");	this->code.push_back('\n');

	this->code.append("492");	this->code.append("\n%%EOF"); // replaced to byte size mesured by cross reference table

	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


string Trailer::generateCode(size_t xrefTableOffset) {
	this->code.append("trailer");	this->code.push_back('\n');
	this->code.append("<<");	this->code.push_back('\n');
	this->code.append("/Size ");	this->code.append(to_string(this->Size));	this->code.push_back('\n');
	this->code.append("/Root ");	this->code.append(to_string(this->Root));	this->code.append(" 0 R");	this->code.push_back('\n');
	//<< "/Info " << this->Info << '\n'
	this->code.append(">>");	this->code.push_back('\n');
	this->code.append("startxref");	this->code.push_back('\n');

	this->code.append(to_string(xrefTableOffset));	this->code.append("\n%%EOF"); // replaced to byte size mesured by cross reference table

	//this->code.push_back('\0');

	this->code.shrink_to_fit();
	return this->code;
}


// Class : CIDSystemInfo
// Description : set encoding information for CID fonts.
void CIDSystemInfo::setSupplement(int sup) {
	this->Supplement = sup;
}


void CIDSystemInfo::setOrderring(string nation) {
	this->Ordering = nation;
}


void CIDSystemInfo::setRegistry(string reg) {
	this->Registry = reg;
}
