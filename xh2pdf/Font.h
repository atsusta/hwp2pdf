#pragma once

#include "Dictionary.h"

#include <vector>

/**
 * Class : PrimaryFont
 * Description : basic font object.
 * Adobe based fonts(e.g. Helvetica, Times New Roman, Arial, ...) 
 * can be identically implied in pdf document and interpreted by PDF renderers.
 * Others can need sub-font and font-descriptor.
 * Depth 5 of PDF object hierarchy
 */
class PrimaryFont : public Dictionary
{

public:
	PrimaryFont();
	void setBaseFont(string fontName);
	void setEncoding(string encoding);
	void setDescendantFonts(int objectNo);

	void referenceObjectNo(int objectNo);
	string generateCode();

protected:
	string baseFont;
	string encoding;
	vector<int> descendantFonts;

};

/** 
 * Class : SubFont
 * Description : descript and reference about Subfont.
 * CIDFont system can be used for Subfont about CJK(Chinese-Japanese-Korean) 
 * language based font support.
 * CIDFonts need Font Descriptor for specifying their glyph identity.
 * A CIDFont dictionary is a PDF object that 
 * contains information about a CIDFont program.
 * !! MORE THAN TWO SUBFONTS MUST BE PRESENTED AS ARRAY SEQUENCE IN PDF CODE !!
 * e.g. [ 7 0 R 8 0 R 9 0 R 10 0 R ]
 * Depth 6 of PDF object hierarchy
 */
class SubFont : public Dictionary
{

public:
	SubFont();
	void setBaseFont(string fontName);
	void setFontDescriptor(int objectNo);
	void setW(int w1, int w2, int w3);
	void setDw(int w);
	void setCidSystemInfo(string cidSystemInfo);

	void referenceObjectNo(int objectNo);
	string generateCode();

private:
	string baseFont;
	int fontDescriptor;
	int w[3];
	int dw;

};

/** 
 * Class : FontDescriptor
 * Description : descript CIDFont font family.
 * It implies core informations about CIDFont such as glyph informations.
 * Depth 7 of pdf object hierarchy
 */
class FontDescriptor : public Object
{

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

	void referenceObjectNo(int objectNo);
	string generateCode();

private:
	string fontName;
	string fontFamily;
	int flags;
	int fontBBox[4];
	int italicAngle;
	int ascent;
	int descent;
	int capHeight;
	int stemV;
	int stemH;
	string fontStretch;
	int fontWeight;
	int xHeight;

};

/**
 * Class : CIDSystemInfo
 * Description : set encoding information for CID fonts.
 * It can be included by font object
 */
class CIDSystemInfo : public Object
{

public:
	void setSupplement(int supplement);
	void setOrderring(string nation);
	void setRegistry(string registry);

	string generateCode();

private:
	int supplement;
	string ordering;
	string registry;

};
