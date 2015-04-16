#pragma once

#include <string>
using namespace std;

/**
 * Class : Header
 * Description : Specify PDF version. the lastest version is 1.7 (2014)
 */
class Header
{

public:
	string getCode();
	void setPdfVersion(string version);
	string generateCode();

private:
	string pdfVersion;
	string code;

};
