
#include "Dictionary.h"

#include <string>

/**
 * Class : Dictionary
 * Description : super class for reference-focused objects.
 *  This is abstract class.
 */
void Dictionary::setSubtype(string subtype) {
	this->subtype = subtype;
}

// Class : Catalog
// Description : Root of pdf object hierarchy
Catalog::Catalog()
{
	// Catalog
	this->setType("/Catalog");
}

void Catalog::referenceObjectNo(int objectNo)
{
	this->pages = objectNo;
}

string Catalog::generateCode()
{
	this->code.append(to_string(this->objectNo));
	this->code.append(" 0 obj");
	this->code.push_back('\n');

	this->code.append("<<\n");

		// Type
		this->code.append("/Type ");
		this->code.append(this->type);
		this->code.push_back('\n');
		
		// Pages
		this->code.append("/Pages ");
		this->code.append(to_string(this->pages));
		this->code.append(" 0 R");	this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return code;
}

// Class : Page
// Description : Pdf page object. it looks parent
Page::Page()
{
	// Child page
	this->setType("/Page");
	for (int i = 0; i < 4; i++)
	{
		mediaBox[i] = 0.0;
		cropbox[i] = 0.0;
	}
	this->parent = 0;
	this->contents = 0;
	this->rotate = 0;
}

int Page::getParent()
{
	return this->parent;
}

int Page::getContents()
{
	return this->contents;
}

void Page::setMediaBox(double m1, double m2, double m3, double m4)
{
	this->mediaBox[0] = m1;
	this->mediaBox[1] = m2;
	this->mediaBox[2] = m3;
	this->mediaBox[3] = m4;
}

void Page::setCropBox(double c1, double c2, double c3, double c4)
{
	this->cropbox[0] = c1;
	this->cropbox[1] = c2;
	this->cropbox[2] = c3;
	this->cropbox[3] = c4;
}

void Page::setContents(int ref)
{
	this->contents = ref;
}

void Page::setResources(int resource)
{
	this->fontResources.push_back(resource);
}

void Page::referenceObjectNo(int objectNum)
{
	this->parent = objectNum;
}

string Page::generateCode()
{
	this->code.append(to_string(this->objectNo));	
	this->code.append(" 0 obj");	
	this->code.push_back('\n');
	
	this->code.append("<<\n");
		// Type
		this->code.append("/Type ");
		this->code.append(this->type);
		this->code.push_back('\n');

		// Parent
		this->code.append("/Parent ");
		this->code.append(to_string(this->parent));
		this->code.append(" 0 R");
		this->code.push_back('\n');

		// MediaBox
		this->code.append("/MediaBox "); this->code.append("[ ");
		for (int i = 0; i < 4; i++)
		{
			this->code.append(to_string(this->mediaBox[i]));
			this->code.append(" ");
		}
		this->code.append("]");	this->code.push_back('\n');

		// Cropbox
		this->code.append("/Cropbox [ ");
		for (int i = 0; i < 4; i++)
		{
			this->code.append(to_string(this->cropbox[i]));
			this->code.append(" ");
		}
		this->code.append("]");	this->code.push_back('\n');

		// Resources (Fonts and Images)
		this->code.append("/Resources ");
		this->code.append("<<");
			this->code.push_back('\n');
			this->code.append("\t");

			// ExtGState
			this->code.append("/ExtGState ");
			this->code.append("<<");
				this->code.push_back('\n');
				this->code.append("\t\t");
				this->code.append("/a0 ");
					this->code.append("<<");
						this->code.append(" /CA 1 /ca 1");
					this->code.append(" >>");
				this->code.push_back('\n');
				this->code.append("\t");
			this->code.append(">>");	
			this->code.push_back('\n');
		
			this->code.append("\t");

			// Font
			this->code.append("/Font ");
			this->code.append("<<");
				this->code.push_back('\n');

				// Font number should be started from 1
				int fontNo = 1;

				// Font resource collection has object number for font object
				for (vector<int>::iterator i = fontResources.begin();
					i != fontResources.end();
					++i)
				{
					this->code.append("\t\t");
					
					// Identical font
					this->code.append("/F");
					this->code.append(to_string(fontNo));
					this->code.push_back(' ');

					this->code.append(to_string(*i));
					this->code.append(" 0 R");
					this->code.push_back('\n');

					// Set next font number
					fontNo++;
				}

				this->code.append("\t");
			this->code.append(">>");
			this->code.push_back('\n');

		this->code.append(">>");
		this->code.push_back('\n');
	
		// Content (Text and Image)
		this->code.append("/Contents ");
		this->code.append(to_string(this->contents));
		this->code.append(" 0 R");
		this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;
}

// Class : Pages
// Description : Parent page. it manages child pages.
Pages::Pages()
{
	// Parent page
	this->setType("/Pages");
	this->count = 0;
}

int Pages::getCount()
{
	return this->count;
}

void Pages::setCount()
{
	this->count++;
}

void Pages::referenceObjectNo(int reference)
{
	// Get page object
	this->kids.push_back(reference);
}

string Pages::generateCode()
{
	this->code.append(to_string(this->objectNo));
	this->code.append(" 0 obj");
	this->code.push_back('\n');

	this->code.append("<<\n");
		
		// Type
		this->code.append("/Type ");
		this->code.append(this->type);
		this->code.push_back('\n');
		
		// Count
		this->code.append("/Count ");
		this->code.append(to_string(this->count));
		this->code.push_back('\n');

		// Kids (child pages)
		this->code.append("/Kids ");
		this->code.append("[");
		for (vector<int>::iterator i = kids.begin(); i != kids.end(); ++i)
		{
			this->code.push_back(' ');
			this->code.append(to_string(*i));
			this->code.append(" 0 R");
		}
		this->code.append(" ]");
		this->code.push_back('\n');

	this->code.append(">>");
	this->code.push_back('\n');
	this->code.append("endobj\n\n");

	this->code.shrink_to_fit();
	return this->code;
}
