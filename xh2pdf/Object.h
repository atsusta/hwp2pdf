#pragma once
#define MAXSIZE 256

using namespace std;

/** 
 * Class : BodyText, Header, Catalog, Pages, Page, PrimaryFont, SubFont, FontDescriptor, XrefTable, Trailer
 * Description : All classes for generating pdf code.
 * If the document has a general text-based structure, which will be useful to play with it.
 * However, PDF files usually contain non-ASCII ("binary") data(such as font file) and should always be considered binary files.
 *
 * A simple PDF contains 4 parts:
 *
 * the header, with the PDF version (and an option line to specify if the PDF contains binary data) 
 * the body, containing a series of objects that are used in the document 
 * a cross-reference table, that specifies the position of the objects 
 * a trailer, with information about where the document starts 
 * In the complete document structure, one can append additional body+cross-reference+trailer elements to complete an existing document.
 */

/**
 * Class : counter
 * Description : counts number of objects
 */
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

/**
 * Class : Object
 * Description : 'object' means PDF OBJECT.
 * This represented as
 * 'n 0 obj ...(attribute code)... endobj'
 * in PDF code('n' is unsigned integer).
 * This is contained by subsystem 'File body'.
 * This is Primary super class for other objects.
 * This is an abstract class.
 */
class Object : public counter<Object> {

public:
	Object();
	int getObjectNo();
	string getCode();
	void setObjectNumber(int num);
	void setType(string type);

	virtual void referenceObjectNo(int objectNo) = 0; 
	virtual string generateCode() = 0;

protected:
	string type;
	long int objectNo;
	string code;

};
