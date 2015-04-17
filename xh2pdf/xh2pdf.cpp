// xh2pdf.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//#include <stdlib.h>
//#include <ctime>
#include "stdafx.h"
#include "ProducePdf.h"

#include <fstream>
#include <iostream>
using namespace std;

/* int _tmain(int argc, _TCHAR* argv[]) */
int _tmain(int argc, char* argv[]) 
{
	cout << "****************************" << endl;
	cout << "* Parsed HWP to PDF Module *" << endl;
	cout << "* version 0.90             *" << endl;
	cout << "****************************" << endl;

	if (argc < 3)
	{
		cout << "How to use :" << endl;
		cout << ">> xh2pdf (XML_FILE_NAME) (TARGET_FILE_NAME)" << endl;
		exit(1);
	}
	else
	{
		// File read process
		cout << "Process start......" << endl;
		cout << "Output of pyhwp :\t" << argv[1] << endl;
		cout << "Name of PDF file :\t" << argv[2] << endl;
	}

	// Control class
	ProducePdf producePdf;

	// File stream
	ofstream fout;

	try
	{
		// BodyText.xml to PDF code generating
		bool isFileRight = producePdf.getPageContentInformation(
			argv[1], argv[2], fout /*, argv[3]*/);

		if (isFileRight == false)
		{
			throw isFileRight;
		}
	}
	catch (bool isFileRight)
	{
		cerr << "File produce error. file is " << isFileRight << endl;
		return -1;
	}

}
