
#include "Object.h"

// Class : Object
// Description : PDF object class. this is an abtract class. inherit attributes and operations.
Object::Object()
{
	objectNo = 65535;
	code.reserve(320);
}

int Object::getObjectNo()
{
	return this->objectNo;
}

string Object::getCode()
{
	return this->code;
}

void Object::setObjectNo(int num)
{
	this->objectNo = num;
}

void Object::setType(string type)
{
	this->type = type;
}
