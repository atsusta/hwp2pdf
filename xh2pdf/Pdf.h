#pragma once
//#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "tinyxml.h"
#include "tinystr.h"
#include <windows.h>
#include "UTF8Conv.h"
#include <unordered_map>
using namespace std;

#define MAXSIZE 256

// Class : BodyText, Header, Catalog, Pages, Page, PrimaryFont, SubFont, FontDescriptor, XrefTable, Trailer
// Description : All classes for generating pdf code.
// If the document has a general text-based structure, which will be useful to play with it.
// However, PDF files usually contain non-ASCII ("binary") data(such as font file) and should always be considered binary files.
//
// A simple PDF contains 4 parts:
//
// the header, with the PDF version (and an option line to specify if the PDF contains binary data) 
// the body, containing a series of objects that are used in the document 
// a cross-reference table, that specifies the position of the objects 
// a trailer, with information about where the document starts 
// In the complete document structure, one can append additional body+cross-reference+trailer elements to complete an existing document.


// Class : LineSeg
// Description : Line segment information archive from XML file
class LineSeg
{
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
	unsigned int pageNumber;

public:
	LineSeg();
	~LineSeg();
	void setTd(double td0, double td1);
	void setTm(double tm0, double tm1, double tm2, double tm3, double tm4, double tm5);
	void setSeg(wstring seg);
	void setFont(string fontName);
	void setLength(size_t length);
	void setPageNumber(unsigned int pageNum);

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
	unsigned int getPageNumber();

	void generateCode(string& code, bool isFirst);
};


// Class : Paragraph
// Description : Paragraph information archive from XML file
class Paragraph
{
private:
	unsigned int pageNumber;
	vector<LineSeg> LineSegCollection;
public:
	Paragraph();
	~Paragraph();
	vector<LineSeg>& getLineSegCollection();
	void setPageNumber(unsigned int pageNum);
	unsigned int getPageNumber();
};


// Class : PageSize
// Description : saving page size & orientation
class PageSize
{
private:
	double mbox0;
	double mbox1;
	double mbox2;
	double mbox3;
	unsigned int appliedPageNumber;
public:
	void setMbox(double mbox0, double mbox1, double mbox2, double mbox3);
	void setAppliedPageNumber(unsigned int appliedPageNumber);
	double getMbox0();
	double getMbox1();
	double getMbox2();
	double getMbox3();
	unsigned int getAppliedPageNumber();
};


// Class : BodyTextInfo
// Description : Saves BodyText information in HWP file analysis
class BodyTextInfo {
private:
	unsigned int pageNumber;

	int defaultTabStop;
	double tm0;
	double tm1;
	double tm2;
	double tm3;
	double tm4;
	double tm5;
	int upperLineSegY;
	int downLineSegY;
	unsigned int numOfFont;

	vector<Paragraph> ParagraphCollection;
	vector<LineSeg> LineSegCollection;
	vector<PageSize> PageSizeCollection;
	//vector<const char*> ImageCollection;
public:
	BodyTextInfo();
	void setNextPage();
	unsigned int getPageNumber();
	void setDefaultTabStops(int defaultTabStop);

	vector<Paragraph>& getParagraphCollection();
	vector<LineSeg>& getLineSegCollection();
	vector<PageSize>& getPageSizeCollection();
	//vector<const char*>& getImageCollection();

	void addNumOfFont();
	void resetNumOfFont();
	int getNumOfFont();
};


// Class : Header
// Description : Specify PDF version. the lastest version is 1.7 (2014)
class Header {
private:
	string PDFVersion;
	string code;
public:
	void setPDFVersion(string version);
	//void generateCode(stringstream& target);
	string generateCode();
	string getCode();
};


// Class : counter <- struct type
// Description : counts number of objects
template <typename T>
struct counter
{
	static int objects_created;
	static int objects_alive;

	counter()
	{
		++objects_created;
		++objects_alive;
	}

	counter(const counter&)
	{
		++objects_created;
		++objects_alive;
	}
protected:
	~counter() // objects should never be removed through pointers of this type
	{
		--objects_alive;
	}
};
template <typename T> int counter<T>::objects_created(0);
template <typename T> int counter<T>::objects_alive(0);


// Class : Object
// Description : 'object' means PDF OBJECT.
//               represented as 'n 0 obj ...(attribute code)... endobj' in pdf code('n' is unsigned integer).
//               File body - objects, Primary super class. It is abstract class.
class Object : public counter<Object> {
private:
protected:
	string Type;
	long int ObjectNumber;
	string code;
public:
	Object();
	int getObjectNumber();
	void setObjectNumber(int num);
	void setType(string type);
	string getCode();

	virtual void referenceObjectNumber(int refObjectNum) = 0; // polymorphism
	virtual string generateCode() = 0; // polymorphism
};


// Class : Dictionary
// Description : super class for reference-focused objects. it is abstract class.
class Dictionary : public Object {
protected:
	string Subtype;
public:
	void setSubtype(string subtype) {
		this->Subtype = subtype;
	}
	virtual void referenceObjectNumber(int refObjectNum) = 0; // polymorphism
	virtual string generateCode() = 0; // polymorphism
};


// Class : Stream
// Description : super class for stream objects.
//               it has byte-stream contents. the byte-stream could be encoded. it is  abstract class.
class Stream : public Object {
protected:
	size_t Length;
	string Subtype;
	string stream;
public:
	Stream()
	{
		Length = 0;
		Subtype = "";
		stream = "";
	}
	~Stream(){}

	void setLength(size_t length) {
		this->Length = length;
	}

	void setSubtype(string subtype) {
		this->Subtype = subtype;
	}

	void setStream(string stream) {
		this->stream = stream;
	}

	virtual void referenceObjectNumber(int refObjectNum) = 0; // polymorphism
	virtual string generateCode() = 0; // polymorphism
};


// Class : Catalog
// Description : Root of PDF object hierarchy
// Depth 1 of PDF object hieararchy
class Catalog : public Dictionary {
private:
	int Pages; // child pages reference ParentPage
public:
	Catalog();
	void referenceObjectNumber(int objectNum); // realization
	string generateCode(); // realization
};


// Class : Page
// Description : PDF page object. it looks their parent.
// Depth 3 of PDF object hieararchy
class Page : public Dictionary {
private:
	int Parent; // ref
	double MediaBox[4];
	double Cropbox[4];
	int Contents; // ref
	string Resources;
	int Rotate; // ?
	vector<int> fontResourceCollection;
public:
	Page();
	int getParent();
	int getContents();
	//void setParent(int ref);
	void setMediaBox(double m1, double m2, double m3, double m4);
	void setCropBox(double c1, double c2, double c3, double c4);
	void setContents(int ref);
	void setResources(int resource);
	void referenceObjectNumber(int ref); // realization
	string generateCode(); // realization
};


// Class : Pages
// Description : Parent page. it manages child pages.
// Depth 2 of PDF object hieararchy
class Pages : public Dictionary {
private:
	vector<int> Kids; // parent page references child pages by array
	int Count; // number of child pages
	//int kidCounter;
	//vector<Page*>* ownedPageCollection; // identifier for collection class
public:
	Pages();
	void setCount();
	//void setOwnedPageCollection(vector<Page*>* collection);
	//vector<Page*>* getOwnedPageCollection();
	int getCount();
	void referenceObjectNumber(int reference); // realization
	string generateCode(); // realization
};


// 필요 없을 수 있음
class Image : public Object {
private:
	string Subtype;
	int Length;
	string Name;
	bool ImageMask;
	int BitsPerComponent;
	int Width;
	int Height;
public:
	void setSubtype(string subtype) {
		this->Subtype = subtype;
	}

	void setLength(int length) {
		this->Length = length;
	}

	void setName(int name) {
		this->Name = name;
	}

	void setImageMask(bool imagemask) {
		this->ImageMask = imagemask;
	}

	void setBitsPerComponent(int bitspercomponent) {
		this->BitsPerComponent = bitspercomponent;
	}

	void setWidth(int width) {
		this->Width = width;
	}

	void setHeight(int height) {
		this->Height = height;
	}

	void referenceObjectNumber(int ref); // realization
	string generateCode(); // implemetnation
};


// Class : PrimaryFont
// Description : basic font object.
// Adobe based fonts(e.g. Helvetica, Times New Roman, Arial, ...) can be identically implied in pdf document and interpreted by PDF renderers.
// Others can need sub-font and font-descriptor.
// Depth 5 of PDF object hierarchy
class PrimaryFont : public Dictionary {
protected:
	string BaseFont;
	string Encoding;
	vector<int> SubFont; // object number reference
public:
	//int getDescendantFonts() {
	//	return this->SubFont;
	//}

	PrimaryFont();
	void setBaseFont(string fontName);
	void setEncoding(string encoding);

	void setDescendantFonts(int ref);

	void referenceObjectNumber(int ref); // realization
	string generateCode(); // realization
};


// Class : SubFont
// Description : descript and reference about Subfont.
// CIDFont system can be used for Subfont about CJK(Chinese-Japanese-Korean) language based font support.
// CIDFonts need Font Descriptor for specifying their glyph identity.
// A CIDFont dictionary is a PDF object that contains information about a CIDFont program.
// !! MORE THAN TWO SUBFONTS MUST BE PRESENTED AS ARRAY SEQUENCE IN PDF CODE !! e.g. [ 7 0 R 8 0 R 9 0 R 10 0 R ]
// Depth 6 of PDF object hierarchy
class SubFont : public Dictionary {
private:
	string BaseFont;
	int FontDescriptor; // ref FontDescriptor
	int W[3];
	int DW;
	string CIDSystemInfo; // ref 가능.. 고려
public:
	SubFont();
	string getCIDSystemInfo();
	void setBaseFont(string fontName);
	void setFontDescriptor(int ref);
	void setW(int w1, int w2, int w3);
	void setDW(int w);
	void setCIDSystemInfo(string info);
	void referenceObjectNumber(int ref); // realization
	string generateCode(); // realization
};


// Class : FontDescriptor
// Description : descript CIDFont font family.
// It implies core informations about CIDFont such as glyph informations.
// Depth 7 of pdf object hierarchy
class FontDescriptor : public Object {
private:
	string FontName;
	string FontFamily;
	int Flags;
	int FontBBox[4];
	int ItalicAngle;
	int Ascent;
	int Descent;
	int CapHeight;
	int StemV;
	int StemH;
	string FontStretch;
	int FontWeight;
	int XHeight;
public:
	FontDescriptor();
	void setFontName(string fontName);
	void setFontFamily(string fontFamily);
	void setFlags(int flags);
	void setFontBBox(int b1, int b2, int b3, int b4);
	void setItalicAngle(int angle);
	void setAscent(int ascent);
	void setDescent(int descent);
	void setCapHeight(int height);
	void setStemV(int h);
	void setStemH(int v);
	void setFontStretch(string stretch);
	void setFontWeight(int weight);
	void setXHeight(int height);
	void referenceObjectNumber(int ref); // realization
	string generateCode(); // realization
};


// Class : Text
// Description : Content stream object. This object often has text stream.
// Depth 4 of PDF object hierarchy
class Text : public Stream {
private:
	double Bbox[4];
	string Resources;
	string StructParents;
	string ProcSet;
	//string FontID;
	vector<int> usedFonts; // ref PrimaryFont
	//int numOfUsedFonts;
	int FormType;
	double Matrix[6];
	//string PieceInfo;
	vector<LineSeg> InPageLineSegCollection;
public:
	Text();
	vector<int>& getUsedFonts(); // if text object references identical object
	void setBbox(int b1, int b2, int b3, int b4);
	void setResources(string resource);
	void setFontNumber(int fontNumber);
	//void setNumOfUsedFonts(int num);
	//void setFont(int ref);
	void setFormType(int type);
	void setMatrix(double m1, double m2, double m3, double m4, double m5, double m6);
	void referenceObjectNumber(int ref); // realization
	string generateCode(); // realization
	string generateCode(size_t orgSize); // realization - overload
	vector<LineSeg>& getInPageLineSegCollection();
	void setInPageLineSegCollection(vector<LineSeg> lineSegCollection);
};


// Class : XrefTable
// Description : Cross-reference-table of Pdf object.
// It provides document access / random pdf object access for PDF renderer.
class XrefTable : private counter<Object> {
private:
	//long int* offset;
	//long int* revision;
	//char* marker;
	//vector<size_t> offsetCollection;
	unordered_map<int, size_t> offsetMap;
	vector<std::pair<int, size_t>> offsetCollection;
	string code;
public:
	XrefTable();
	~XrefTable();
	//void setOffset(long int* offset, int objectSize);
	//void setOffset(size_t size);
	void setOffset(int objectNum, size_t size);
	//void setRevision(long int* answer, int objectSize);
	//void setMarker(char* isUse, int objectSize);
	//vector<size_t>& getOffsetCollection();
	vector<std::pair<int, size_t>>& getOffsetCollection();
	unordered_map<int, size_t>& getOffsetMap();
	string getCode();
	string generateCode();
};


// Class : Trailer
// Description : tail of cross reference table
// It sets root in file body structure / file information(this is optional) / offset of cross reference table
class Trailer {
private:
	int Size;
	int Root; // ref
	string code;
public:
	void setSize(int size);
	void setRoot(int root);
	string generateCode();
	string generateCode(size_t XrefTableOffset);
};


// Class : CIDSystemInfo
// Description : set encoding information for CID fonts.
// It can be included by font object
class CIDSystemInfo : public Object {
private:
	int Supplement;
	string Ordering;
	string Registry;
public:
	void setSupplement(int sup);
	void setOrderring(string nation);
	void setRegistry(string reg);
	string generateCode();
};
