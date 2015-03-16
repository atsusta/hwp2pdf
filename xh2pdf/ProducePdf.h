#pragma once
#include "Pdf.h"

// Class : ProducePdf
// Description : Produce Pdf object and code.
// Control class for use case 'Initialize new PDF objects', 'Get page content information', 'Connect page content to page' and 'Generate a new PDF code'.
class ProducePdf
{
public:
	ProducePdf();
	~ProducePdf();
	bool getPageContentInformation(char* inputFileName, char* outputFileName, ofstream& pdfCode/*, char* imageFileName*/);
	
	void initializeNewPdfObject(BodyTextInfo bodyTextInfo,
		vector<PageSize>& tempPageSizeCollection,
		vector<string> documentFontCollection,
		vector<Text> textCollection,
		char* outputFileName,
		ofstream& pdfCode);

	void connectPageContentToPage(vector<Text>& textCollection, vector<Page>& pageCollection);
	
	void generateANewPdfCode(
		Header& header,
		Catalog& catalog,
		Pages& parentPage,
		vector<Page>& pageCollection,
		vector<PrimaryFont>& primaryFontCollection,
		vector<Text>& textCollection,
		vector<SubFont>& subFontCollection,
		vector<FontDescriptor>& fontDescriptorCollection,
		Trailer& trailer,
		XrefTable& xrefTable,
		ofstream& code
		);

	char* readFileBytes(const char *name);
};

