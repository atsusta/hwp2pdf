// xh2pdf.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
// typedef wchar_t _TCHAR
//#include <stdlib.h>
//#include <ctime>
#include "stdafx.h"
#include "ProducePdf.h"

#include <fstream>
#include <iostream>
using namespace std;

int _tmain(int argc, char* argv[]) //int _tmain(int argc, _TCHAR* argv[])
{
	cout << "****************************" << endl;
	cout << "* Parsed HWP to PDF Module *" << endl;
	cout << "* version 0.90             *" << endl;
	cout << "****************************" << endl;

	if (argc < 3)
	{
		cout << "How to use :" << '\n' << ">>xh2pdf XML_FILE TARGET_FILE_NAME" << '\n';
		exit(1);
	}
	else
	{
		// File read process
		cout << "process start......" << endl;
		cout << "output of pyhwp :\t" << argv[1] << endl;
		cout << "name of PDF file :\t" << argv[2] << endl;
	}

	// Control class
	ProducePdf producePdf;

	// File stream
	ofstream fout;

	try
	{
		// BodyText.xml to PDF code generating
		bool isFileRight = producePdf.getPageContentInformation(
			argv[1], argv[2], fout/*, argv[3]*/);
		if (isFileRight == false)
			throw isFileRight;
	}
	catch (bool isFileRight)
	{
		cerr << "File produce error : " << isFileRight << endl;
		return -1;
	}

}
