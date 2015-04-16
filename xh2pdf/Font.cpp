
#include "Font.h"

// Class : PrimaryFont
// Description : contains font name, encoding, subfont information.
PrimaryFont::PrimaryFont()
{
	this->setType("/Font");
	this->descendantFonts.clear();
	this->encoding = "none";
}

void PrimaryFont::setBaseFont(string fontName)
{
	this->baseFont = fontName;
}

void PrimaryFont::setEncoding(string encoding)
{
	this->encoding = encoding;
}

void PrimaryFont::setDescendantFonts(int ref)
{
	this->descendantFonts.push_back(ref);
}

void PrimaryFont::referenceObjectNo(int ref)
{
	this->descendantFonts.push_back(ref);
}

string PrimaryFont::generateCode()
{
	this->code.append(to_string(this->objectNo));
	this->code.append(" 0 obj");
	this->code.push_back('\n');

	this->code.append("<<\n");

	// Type
	this->code.append("/Type ");
	this->code.append(this->type);
	this->code.push_back('\n');

	// Subtype
	this->code.append("/Subtype ");
	this->code.append(this->subtype);
	this->code.push_back('\n');

	// BaseFont
	this->code.append("/BaseFont ");
	this->code.push_back('/');
	this->code.append(this->baseFont);
	this->code.push_back('\n');

	if (this->descendantFonts.size() > 0)
	{
		// CID subfont dictionary code
		this->code.append("/DescendantFonts [");
	}

	for (vector<int>::iterator i = this->descendantFonts.begin();
		i != this->descendantFonts.end();
		++i)
	{
		this->code.append(to_string(*i));
		this->code.append(" 0 R");
	}
	this->code.append(" ]");
	this->code.push_back('\n');

	if (this->encoding != "none")
	{
		// Encoding
		this->code.append("/Encoding ");
	}
	this->code.append(this->encoding);
	this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;
}

// Class : SubFont
// Description : CIDFont system can be used for Subfont to present CJK(Chinese-Japanese-Korean) language based font.
//  CIDFonts need Font Descriptor for specifying their identity.
//  !! ALL SUBFONTS MUST BE PRESENTED AS ARRAY SEQUENCE IN PDF CODE !!
void SubFont::referenceObjectNo(int objectNo)
{ 
	if (this->fontDescriptor == 0)
		this->fontDescriptor = objectNo;
}

SubFont::SubFont()
{
	this->setType("/Font");
	this->setSubtype("/CIDFontType2");
	this->fontDescriptor = 0;
	for (int i = 0; i < 3; i++)
		this->w[i] = 0;
}

string SubFont::getCidSystemInfo()
{
	return this->cidSystemInfo;
}

void SubFont::setBaseFont(string fontName)
{
	this->baseFont = fontName;
}

void SubFont::setFontDescriptor(int ref)
{
	this->fontDescriptor = ref;
}

void SubFont::setW(int w1, int w2, int w3)
{
	this->w[0] = w1;
	this->w[1] = w2;
	this->w[2] = w3;
}

void SubFont::setDw(int w)
{
	this->dw = w;
}


void SubFont::setCidSystemInfo(string info)
{
	this->cidSystemInfo = info;
}


string SubFont::generateCode()
{
	//this->code.append(to_string(this->ObjectNumber - 1));	this->code.append(" 0 obj");	this->code.push_back('\n');
	//this->code.append("[ ");	this->code.append(to_string(this->ObjectNumber));	this->code.append(" 0 R ]");	this->code.push_back('\n');
	//this->code.append("endobj\n\n");

	this->code.append(to_string(this->objectNo));
	this->code.append(" 0 obj");
	this->code.push_back('\n');
	this->code.append("<<\n");
		this->code.append("/Type ");
		this->code.append(this->type);
		this->code.push_back('\n');
		this->code.append("/Subtype ");
		this->code.append(this->subtype);
		this->code.push_back('\n');
		
		this->code.append("/FontDescriptor ");
		this->code.append(to_string(this->fontDescriptor));
		this->code.append(" 0 R");
		this->code.push_back('\n');
		this->code.append("/BaseFont ");
		this->code.push_back('/');	
		this->code.append(this->baseFont);	
		this->code.push_back('\n');

		this->code.append("/W");
		this->code.append(" [ ");
		for (int i = 0; i < 3; i++) {
			this->code.append(to_string(this->w[i]));
			this->code.append(" ");
		}
		this->code.append("]");
		this->code.push_back('\n');
		
		this->code.append("/CIDSystemInfo ");
		this->code.append(this->cidSystemInfo);
		this->code.push_back('\n');

		this->code.append("/DW ");
		this->code.append(to_string(this->dw));
		this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;

}

// Class : FontDescriptor
// Description : descript CIDFont font family.
//  It implies core informations about CIDFont such as glyph informations.
FontDescriptor::FontDescriptor()
{
	this->setType("/FontDescriptor");
	for (int i = 0; i < 4; i++)
		fontBbox[i] = 0;
}

void FontDescriptor::setFontName(string fontName)
{
	this->fontName = fontName;
}

void FontDescriptor::setFontFamily(string fontFamily)
{
	this->fontFamily = fontFamily;
}

void FontDescriptor::setFlags(int flags)
{
	this->flags = flags;
}

void FontDescriptor::setFontBBox(int b1, int b2, int b3, int b4)
{
	this->fontBbox[0] = b1;
	this->fontBbox[1] = b2;
	this->fontBbox[2] = b3;
	this->fontBbox[3] = b4;
}

void FontDescriptor::setItalicAngle(int angle)
{
	this->italicAngle = angle;
}

void FontDescriptor::setAscent(int ascent)
{
	this->ascent = ascent;
}

void FontDescriptor::setDescent(int descent)
{
	this->descent = descent;
}

void FontDescriptor::setCapHeight(int height)
{
	this->capHeight = height;
}

void FontDescriptor::setStemV(int h)
{
	this->stemH = h;
}

void FontDescriptor::setStemH(int v)
{
	this->stemV = v;
}

void FontDescriptor::setFontStretch(string stretch)
{
	this->fontStretch = stretch;
}

void FontDescriptor::setFontWeight(int weight)
{
	this->fontWeight = weight;
}

void FontDescriptor::setXHeight(int height)
{
	this->xHeight = height;
}

void FontDescriptor::referenceObjectNo(int ref)
{

}

string FontDescriptor::generateCode()
{
	this->code.append(to_string(this->objectNo));	
	this->code.append(" 0 obj");	
	this->code.push_back('\n');
	this->code.append("<<\n");

		// Type
		this->code.append("/Type ");
		this->code.append(this->type);
		this->code.push_back('\n');

		// FontName
		this->code.append("/FontName ");
		this->code.push_back('/');
		this->code.append(this->fontName);
		this->code.push_back('\n');

		// FontFamily
		this->code.append("/FontFamily ");
		this->code.push_back('(');
		this->code.append(this->fontFamily);
		this->code.push_back(')');
		this->code.push_back('\n');

		// Flags
		this->code.append("/Flags ");
		this->code.append(to_string(this->flags));
		this->code.push_back('\n');

		// FontBBox
		this->code.append("/FontBBox ");
		this->code.append(" [ ");
		for (int i = 0; i < 4; i++) {
			this->code.append(to_string(this->fontBbox[i]));
			this->code.append(" ");
		}
		this->code.append(" ]");
		this->code.push_back('\n');

		// ItalicAngle
		this->code.append("/ItalicAngle ");
		this->code.append(to_string(this->italicAngle));
		this->code.push_back('\n');
		this->code.append("/Ascent ");
		this->code.append(to_string(this->ascent));
		this->code.push_back('\n');

		// Descent
		this->code.append("/Descent ");
		this->code.append(to_string(this->descent));
		this->code.push_back('\n');
		
		// CapHeight
		this->code.append("/CapHeight ");
		this->code.append(to_string(this->capHeight));
		this->code.push_back('\n');

		// StemV
		this->code.append("/StemV ");
		this->code.append(to_string(this->stemV));
		this->code.push_back('\n');

		// StemH
		this->code.append("%/StemH ");	
		this->code.append(to_string(this->stemH));	
		this->code.push_back('\n');

		// FontWeight
		this->code.append("/FontWeight ");
		this->code.append(to_string(this->fontWeight));
		this->code.push_back('\n');

		// FontStretch
		this->code.append("/FontStretch ");
		this->code.append(this->fontStretch);
		this->code.push_back('\n');

		// XHeight
		this->code.append("/XHeight ");
		this->code.append(to_string(this->xHeight));
		this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;
}

// Class : CIDSystemInfo
// Description : set encoding information for CID fonts.
void CIDSystemInfo::setSupplement(int supplement)
{
	this->supplement = supplement;
}

void CIDSystemInfo::setOrderring(string language)
{
	this->ordering = language;
}

void CIDSystemInfo::setRegistry(string registry)
{
	this->registry = registry;
}
