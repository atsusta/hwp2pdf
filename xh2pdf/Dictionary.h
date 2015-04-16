#pragma once

#include "Object.h"

#include <vector>
#include <string>

/**
 * Class : Dictionary
 * Description : super class for reference-focused objects.
 *  This is abstract class.
 */
class Dictionary : public Object {

public:
	void setSubtype(string subtype);

	virtual void referenceObjectNo(int objectNo) = 0;
	virtual string generateCode() = 0;

protected:
	string subtype;

};

/**
 * Class : Catalog
 * Description : Root of PDF object hierarchy.
 *  Depth 1 of PDF object hieararchy.
 *  Child pages reference ParentPage.
 */
class Catalog : public Dictionary
{

public:
	Catalog();
	void referenceObjectNo(int objectNo);
	string generateCode();

private:
	int pages;

};

/**
 * Class : Page
 * Description : PDF page object. it looks their parent.
 *  Depth 3 of PDF object hieararchy.
 *  Page references its Parent a.k.a Pages, Content a.k.a Text.
 */
class Page : public Dictionary {

public:
	Page();
	int getParent();
	int getContents();
	void setMediaBox(double m1, double m2, double m3, double m4);
	void setCropBox(double c1, double c2, double c3, double c4);
	void setContents(int objectNo);
	void setResources(int resource);

	void referenceObjectNo(int objectNo);
	string generateCode();

private:
	int parent;
	double mediaBox[4];
	double cropbox[4];
	int contents;
	string resources;
	int rotate;
	vector<int> fontResources;

};

/**
 * Class : Pages
 * Description : Parent page. it manages child pages.
 *  Depth 2 of PDF object hieararchy.
 *  Parent page references child pages by array and knows number of its childs.
 */
class Pages : public Dictionary
{

public:
	Pages();
	int getCount();
	void setCount();

	void referenceObjectNo(int objectNo);
	string generateCode();

private:
	vector<int> kids;
	int count;

};
