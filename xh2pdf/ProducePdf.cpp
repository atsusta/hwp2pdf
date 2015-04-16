#pragma once

#include "stdafx.h"
#include "ProducePdf.h"
#include "tinyxml.h"
#include "UTF8Conv.h"

#include <algorithm>
#include <iostream>
using namespace std;

ProducePdf::ProducePdf()
{
}


ProducePdf::~ProducePdf()
{
}


// Function : getPageContentInformation
// Description : for use case 'Get page content information'
bool ProducePdf::getPageContentInformation(char* inputFileName, char* outputFileName, ofstream& pdfCode/*, char* imageFileName*/)
{
	cout << "saving bodytext.xml analysis" << endl;

	// Saving BodyText.xml analysis
	BodyTextInfo bodyTextInfo;

	// Unmarshalling XML file IO using TinyXML.
	TiXmlDocument document;
	document.LoadFile(inputFileName);

	// Root node : <BodyText> setting
	TiXmlElement* pRoot = document.FirstChildElement("BodyText");
	if (pRoot == NULL)
	{
		cout << "file does not exists" << '\n';
		return false;
	}

	// Page number, size, and orientation setting ready
	vector<PageSize>& tempPageSizes = bodyTextInfo.getPageSizes();

	// Get reference of lineSegs in BodyText object
	vector<LineSeg>& tempLineSegs = bodyTextInfo.getLineSegs();

	// Image file to byte array
	char* imageFileToByteArray = NULL;

	// Depth 1 : <SectionDef> analysis
	for (TiXmlElement* pElem = pRoot->FirstChildElement("SectionDef");
		pElem;
		pElem = pRoot->NextSiblingElement())
	{
		if (pElem)
		{
			// New SectionDef : new page orientation
			bodyTextInfo.setNextPage();

			// Tabstop setting
			int defaultTabStops = atoi(pElem->Attribute("defaultTabStops"));
			bodyTextInfo.setDefaultTabStops(defaultTabStops);

			// Depth 2 : 
			// <PageDef>, <FootNoteShape>, <PageBorderFill>, <Paragraph> analysis
			double mbox0 				= 0.0;
			double mbox1 				= 0.0;
			double mbox2 				= 0.0;
			double mbox3 				= 0.0;
			double tm0					= 0.0;
			double tm1					= 0.0;
			double tm2					= 0.0;
			double tm3					= 0.0;
			double tm4					= 0.0;
			double tm5					= 0.0;
			double td0					= 0.0;
			double td1					= 0.0;
			int lineSegX				= 0;
			int upperLineSegY			= 0;
			int downerLineSegY			= 0;
			bool isPageChangedInLineSeg = false;

			for (TiXmlElement* pChild = pElem->FirstChildElement();
				pChild;
				pChild = pChild->NextSiblingElement()) 
			{
				// Fetch each existing children
				// (PageDef, FootnoteShpe, PageBorderFill, Paragraph)

				bool hasPageDef = !memcmp(pChild->Value(), "PageDef", 
					sizeof("PageDef"));
				bool hasFootNoteShape = !memcmp(pChild->Value(), "FootNoteShape", 
					sizeof("FootNoteShape"));
				bool hasPageBorderFill = !memcmp(pChild->Value(), "PageBorderFill",
					sizeof("PageBorderFill"));
				bool hasParagraph = !memcmp(pChild->Value(), "Paragraph", 
					sizeof("Paragraph"));

				if (hasPageDef)
				{
					// Analyze when <PageDef> exists,
					// PAGE SCOPE : page size setting
					{
						// Page orientation alternation (/MediaBox in PDF)
						PageSize tempPageSize;

						bool isPortrait = !memcmp(pChild->Attribute("orientation"), "portrait", sizeof("portrait"));
						if (isPortrait)
						{ 
							// Portrait orientation
							mbox2 = (double)atoi(pChild->Attribute("width"));
							mbox3 = (double)atoi(pChild->Attribute("height"));
							tempPageSize.setMbox(mbox0, mbox1, mbox2 / 100.0, mbox3 / 100.0);
							cout << "MediaBox : " << mbox0 << " " << mbox1 << " " << mbox2 << 
								" " << mbox3 << endl;
						}
						else
						{ 
							// Landscape orientation
							mbox2 = (double)atoi(pChild->Attribute("height"));
							mbox3 = (double)atoi(pChild->Attribute("width"));
							tempPageSize.setMbox(mbox0, mbox1, mbox2 / 100.0, mbox3 / 100.0);
						}

						// Page1 - first element, page2 - second element, ...
						tempPageSizes.push_back(tempPageSize);
						tempPageSize.setAppliedPageNo(bodyTextInfo.getPageNo());
					} // end PAGE SCOPE

					// PARAGRAPH SCOPE : page margin analyze (/Tm in PDF)
					tm4 = (double) atoi(pChild->Attribute("left-offset"));
					tm5 = (double) ( atoi(pChild->Attribute("height")) 
						- atoi(pChild->Attribute("top-offset")) 
						- atoi(pChild->Attribute("header-offset")) );
				}

				else if (hasFootNoteShape)
				{
					// <FootNoteShape> analysis
					// blank
				}

				else if (hasPageBorderFill)
				{
					// <PageBorderFill> analysis
					// blank
				}

				else if (hasParagraph)
				{
					// <Paragraph> exists, analyze
					// Paragraphs -1--owns--*- Paragraph
					//									Paragraph -1--has--1- LineSegs
					//														  LineSegs -1--owns--*- LineSeg

					for (TiXmlElement* pChild1 = pChild->FirstChildElement();
						pChild1;
						pChild1 = pChild1->NextSiblingElement()) // fetch each children (LineSeg)
					{
						// <LineSeg> analysis
						// depth 3 : <LineSeg> setting
						bool hasLineSeg = !memcmp(pChild1->Value(), "LineSeg", sizeof("LineSeg"));
						if (hasLineSeg)
						{
							LineSeg tempLineSeg;

							// Tm setting - font size (e.g. 10 0 0 10 566.9 785.19 matrix)
							tm0 = (double) atoi(pChild1->Attribute("height2"));
							tm3 = (double) atoi(pChild1->Attribute("height"));

							cout << "Text Matrix : \t" << tm0 << " " << 
								tm1 << " " << tm2 << " " << tm3 << " " << 
								tm4 << " " << tm5 << endl;

							tempLineSeg.setTm(tm0 / 100.0, tm1 / 100.0, 
								tm2 / 100.0, tm3 / 100.0, 
								tm4 / 100.0, tm5 / 100.0);

							// Page number setting
							lineSegX = atoi(pChild1->Attribute("x"));
							downerLineSegY = atoi(pChild1->Attribute("y"));

							if (upperLineSegY > downerLineSegY) 
							{
								// Offset of line segment got over in Paragraph scope
								bodyTextInfo.setNextPage();

								// Each paragraph has page number
								tempLineSeg.setPageNo(bodyTextInfo.getPageNo());
							}
							else 
							{ 
								// Next y postion is smaller than previous y position -> next page
								tempLineSeg.setPageNo(bodyTextInfo.getPageNo());
							}

							// Td setting for line height and indentation
							td0 = lineSegX / 1000.0;
							td1 = (downerLineSegY / 1000.0) * (1000.0 / tm0);
							tempLineSeg.setTd(td0, td1);

							upperLineSegY = downerLineSegY;

							// Font setting (temporary preference)
							// In this version all PDF documents 
							// has the font Nanum Gothic Coding only...
							tempLineSeg.setFont("NanumGothicCoding");

							// Text(string) setting
							string strAnsi = "";
							wstring strUnicode = L"";

							for (TiXmlElement* pChild2 = pChild1->FirstChildElement();
								pChild2;
								pChild2 = pChild2->NextSiblingElement())
							{
								// <Text> / <GShapeObjectControl> analysis
								pChild2->SetCondenseWhiteSpace(false);

								bool hasText = !memcmp(pChild2->Value(), "Text", 
									sizeof("Text"));
								bool hasImage = !memcmp(pChild1->Value(), "GShapeObjectControl", 
									sizeof("GShapeObjectControl"));

								if (hasText)
								{
									const char* strUtf8 = pChild2->GetText();
									if (strUtf8 == 0)
									{
										// Proceed when <Text> element has no value
										cout << "no text : insert '\\n'" << endl;
									}

									else
									{ 
										// Proceed when <Text> element has a value
										// Converting method to re-encode UTF8 string from the XML file
										
										// 1. UTF-8 -> UTF-16 -> MultiByte(ANSI)

										//char strUTF8[128] = { 0, };
										//wchar_t strUnicode[128] = { 0, };
										//strcpy_s(strUTF8, 128, pChild->GetText());

										//int nLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8, strlen(strUTF8), NULL, NULL);
										//MultiByteToWideChar(CP_UTF8, 0, strUTF8, strlen(strUTF8), strUnicode, nLen);

										//char	strMultibyte[128] = { 0, };
										////wcscpy_s(strUnicode, 256, L"유니코드");
										//int len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
										//WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strMultibyte, len, NULL, NULL);

										//cout << strMultibyte << endl;

										//tempLineSeg.setSeg(strMultibyte);
										//tempLineSegs.push_back(tempLineSeg); // each paragraph has line one or more segments


										// 2. UTF8Conv macro functions
										UTF8_CONVERSION;
										strAnsi = UTF82A(strUtf8); // ANSI string for console display
										wstring strUnicodeSegment = UTF82W(strUtf8); // UNICODE string for pdf code generating
										
										// Unit test
										cout << strAnsi;
										cout << '\n';

										// Copy segment
										strUnicode += strUnicodeSegment;
									}
								}
								else if (hasImage)
								{
									// When document has an image object
									//imageFileToByteArray = readFileBytes(imageFileName);
									//tempImages.push_back(imageFileToByteArray);
									//cout << "image file test" << endl;
								}
							} // End text(string) setting
							tempLineSeg.setSeg(strUnicode); // set line segment (TJ element)
							tempLineSeg.setLength(sizeof(strUnicode)); // set line segment length for xrefTable
							tempLineSegs.push_back(tempLineSeg);
						} // End <LineSeg> setting (depth 3)
					}
				} // End <PageDef>, <Paragraph> (depth 2)
			} 
		}
	} // End <SectionDef> (depth 1)

	// Paragraph assigned by text objects ordered by page number
	vector<Text> texts;

	// 전체 문서에 있는 폰트의 종류
	vector<string> documentFonts;

	for (unsigned int i = 1; i <= bodyTextInfo.getPageNo(); i++)
	{
		// Assign page content by page number
		// Verify page number in paragraph included in bodyText.xml
		// and allocate data to paragraph included in PDF text object.
		Text aText;
		aText.setObjectNumber(counter<Object>::objects_alive);

		// Get line segment collection in text object
		vector<LineSeg>& tempInPageLineSegs = aText.getInPageLineSegs();

		for (unsigned int j = 1; j <= tempLineSegs.size(); j++)
		{
			if (tempLineSegs[j - 1].getPageNo() == i)
			{
				// When page number between text object and exposed line segment matches
				// copy line segment content to text object
				tempInPageLineSegs.push_back(tempLineSegs[j - 1]);
			}
		}

		// Analize what fonts the text object have.
		// InPageParagraph & InPageFont
		// The Font name must be identical.
		vector<string> inPageFonts;
		size_t textLength = 0;

		for (vector<LineSeg>::iterator IterPos = tempInPageLineSegs.begin();
			IterPos != tempInPageLineSegs.end();
			++IterPos)
		{
			// Count up fonts
			inPageFonts.push_back(IterPos->getFont());
			textLength += IterPos->getLength();
		}

		// Set text length for cross reference table
		aText.setLength(textLength);

		// Count number of fonts as unique
		vector<string>::iterator it;
		it = unique(inPageFonts.begin(), inPageFonts.end());
		inPageFonts.resize(distance(inPageFonts.begin(), it));

		for (unsigned int i = 1; i <= inPageFonts.size(); i++) {
			// Used Resource(font number) setting 
			// Represented in PDF as '/F1 /F2 /F3 /F4 ...'
			aText.setFontNumber(i);
		}

		texts.push_back(aText);

		// Count up all the fonts in document
		for (vector<string>::iterator dfIterPos = inPageFonts.begin();
			dfIterPos != inPageFonts.end();
			++dfIterPos)
		{
			documentFonts.push_back(*dfIterPos);

			// Count number of fonts as unique
			vector<string>::iterator documentIt;
			documentIt = unique(documentFonts.begin(), documentFonts.end());
			
			// Resize collection
			documentFonts.resize(distance(documentFonts.begin(), documentIt));
		}
	}

	/* Initialize new pdf objects */
	cout << "Initialize new pdf objects" << endl;

	initializeNewPdfObject(bodyTextInfo, tempPageSizes, documentFonts, texts, outputFileName, pdfCode);
	return true;

}


// Fuction : InitializeNewPdfObject
// Description : for use case 'Initialize new PDF objects'
void ProducePdf::initializeNewPdfObject(BodyTextInfo bodyTextInfo, 
	vector<PageSize>& tempPageSizes, 
	vector<string> documentFonts, 
	vector<Text> texts, 
	char* outputFileName, 
	ofstream& pdfCode)
{
	// File header
	Header header;
	header.setPdfVersion("%PDF-1.7");

	// Catalog looks parent page
	Catalog catalog;
	catalog.setObjectNumber(counter<Object>::objects_alive);

	// Root of page collection
	Pages parentPage;
	parentPage.setObjectNumber(counter<Object>::objects_alive);

	// Multiple pages
	vector<Page> pages;

	unsigned int pageIndex = 0;
	for (unsigned int i = 0; i < bodyTextInfo.getPageNo(); i++)
	{
		// Page object - set object number int PDF code
		Page aPage;
		aPage.setObjectNumber(counter<Object>::objects_alive);
		aPage.referenceObjectNo(parentPage.getObjectNo());

		// Set mediabox
		if (tempPageSizes.size() == 1)
		{
			aPage.setMediaBox(
				tempPageSizes[pageIndex].getMbox0(),
				tempPageSizes[pageIndex].getMbox1(),
				tempPageSizes[pageIndex].getMbox2(),
				tempPageSizes[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				tempPageSizes[pageIndex].getMbox0(),
				tempPageSizes[pageIndex].getMbox1(),
				tempPageSizes[pageIndex].getMbox2(),
				tempPageSizes[pageIndex].getMbox3()
				);
		}
		else
		{
			pageIndex++;
			aPage.setMediaBox(
				tempPageSizes[pageIndex].getMbox0(),
				tempPageSizes[pageIndex].getMbox1(),
				tempPageSizes[pageIndex].getMbox2(),
				tempPageSizes[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				tempPageSizes[pageIndex].getMbox0(),
				tempPageSizes[pageIndex].getMbox1(),
				tempPageSizes[pageIndex].getMbox2(),
				tempPageSizes[pageIndex].getMbox3()
				);
		}
		// push page to collection
		pages.push_back(aPage);
	}

	// Initialize font collection
	vector<PrimaryFont> primaryFonts;
	vector<SubFont> subFonts;
	vector<FontDescriptor> fontDescriptors;

	for (vector<string>::iterator fIterPos = documentFonts.begin();
		fIterPos != documentFonts.end();
		++fIterPos)
	{
		// Font saving in whole document scope
		// Set primary font
		PrimaryFont aFont;
		aFont.setObjectNumber(counter<Object>::objects_alive);

		if (!fIterPos->compare("NanumGothicCoding"))
		{
			// When meets font Nanum Gothic Coding
			// Set subfont and font descriptor
			SubFont aSubfont;
			aSubfont.setObjectNumber(counter<Object>::objects_alive);
			FontDescriptor aFontDescriptor;
			aFontDescriptor.setObjectNumber(counter<Object>::objects_alive);

			aFont.setSubtype("/Type0");
			aFont.setBaseFont(fIterPos->data());
			aFont.setEncoding("/UniKS-UCS2-H");

			aSubfont.setSubtype("/CIDFontType2");
			aSubfont.setBaseFont(fIterPos->data());
			//aSubfont.setFontDescriptor("")
			aSubfont.setW(0, 95, 500);
			aSubfont.setDw(1000);
			aSubfont.setCidSystemInfo("<<\n/Supplement 1\n/Ordering(Korea1)\n/Registry(Adobe)\n>>");

			// Primary font refereneces descendant fonts
			aFont.referenceObjectNo(aSubfont.getObjectNo());

			aFontDescriptor.setFontName(fIterPos->data());
			aFontDescriptor.setFontFamily(fIterPos->data());
			aFontDescriptor.setFlags(34);
			aFontDescriptor.setFontBBox(0, -200, 1000, 801);
			aFontDescriptor.setItalicAngle(0);
			aFontDescriptor.setAscent(801);
			aFontDescriptor.setDescent(-200);
			aFontDescriptor.setCapHeight(699);
			aFontDescriptor.setStemV(72);
			aFontDescriptor.setStemH(72);
			aFontDescriptor.setFontStretch("/Normal");
			aFontDescriptor.setFontWeight(400);
			aFontDescriptor.setXHeight(458);
			aSubfont.referenceObjectNo(aFontDescriptor.getObjectNo()); // subfont references font descriptor

			// save CID subfont and font descriptor to each collection class
			subFonts.push_back(aSubfont);
			fontDescriptors.push_back(aFontDescriptor);
		}
		else if (!fIterPos->compare("TimesNewRoman"))
		{
			// if there isn't first font for english, add font Times New Roman.
			aFont.setSubtype("Type1");
			aFont.setBaseFont(fIterPos->data());
		}

		// save primary font
		primaryFonts.push_back(aFont);
	}

	// Catalog looks at parentPage
	catalog.referenceObjectNo(parentPage.getObjectNo());

	// Multiple pages and parent page
	// Page and font reference setting
	for (vector<Page>::iterator pIterPos = pages.begin();
		pIterPos != pages.end();
		++pIterPos)
	{
		// Child page reference setting in Parent page
		// Represented as 'Kids [ x 0 R y 0 R z 0 R ... ]'
		parentPage.referenceObjectNo(pIterPos->getObjectNo());
		pIterPos->referenceObjectNo(parentPage.getObjectNo());
		parentPage.setCount();

		// Primary font reference setting in child page
		for (vector<PrimaryFont>::iterator fIterPos = primaryFonts.begin();
			fIterPos != primaryFonts.end();
			++fIterPos)
		{
			pIterPos->setResources(fIterPos->getObjectNo());
		}
	}

	// Initialize Cross reference table
	XrefTable xrefTable;

	// Initialize File Trailer 
	Trailer trailer;
	trailer.setRoot(catalog.getObjectNo());
	trailer.setSize(counter<Object>::objects_alive);

	/* Connect page content to page */
	cout << "Connect page content to page" << endl;
	this->connectPageContentToPage(texts, pages);

	/* Generate a new pdf code */
	cout << "Generate a new pdf code" << endl;

	// Initialize file name pointer
	char* outputFileNameWithExtension = NULL;
	outputFileNameWithExtension = new char[strlen((char*)outputFileName) + 5];

	// Get original file name
	int fileLength = strncpy_s(outputFileNameWithExtension, 
		strlen((char*) outputFileName) + 5, (char*) outputFileName, 
		strlen((char*) outputFileName));

	if (fileLength != 0)
	{
		cout << "file name(strncpy_s) error" << endl;
		exit(1);

		// Concatenate file extension ".pdf"
		int pdfNameLength = strncat_s(outputFileNameWithExtension,
			strlen((char*)outputFileName) + 5, ".pdf", 4);

		if (pdfNameLength != 0) {
			cout << "file name(strncat_s) error" << endl;
			exit(1);
		}
		else
		{
			cout << "processing : " << outputFileName << " -> " << outputFileNameWithExtension << endl;
		}
	}

	// Output file processing
	pdfCode.open(outputFileNameWithExtension, (ios::out | ios::binary));

	// Move pointer to begin of file
	pdfCode.seekp(0, ios::beg);

	// Generate final PDF code
	this->generateNewPdfCode(header, catalog, parentPage, pages, primaryFonts,
		texts, subFonts, fontDescriptors, trailer, xrefTable, pdfCode);

	// Move pointer to end of file
	pdfCode.seekp(0, ios::end);
	
	// Get file size
	size_t length = pdfCode.tellp();

	// Close file
	pdfCode.close();
}


// Fuction : connectPageContentToPage
// Description : for use case 'Connect page content to page'
void ProducePdf::connectPageContentToPage(vector<Text>& texts, 
	vector<Page>& pages)
{
	vector<Text>::iterator tIterPos = texts.begin();
	vector<Page>::iterator pIterPos = pages.begin();

	while ((tIterPos != texts.end()) && (pIterPos != pages.end()))
	{
		pIterPos->setContents(tIterPos->getObjectNo());
		tIterPos++;
		pIterPos++;
	}
}


// Function : generateANewPdfCode
// Description : for use case 'Generate a new pdf code'
void ProducePdf::generateNewPdfCode(Header& header, Catalog& catalog,
	Pages& parentPage, vector<Page>& pages,	vector<PrimaryFont>& primaryFonts,
	vector<Text>& texts, vector<SubFont>& subFonts,
	vector<FontDescriptor>& fontDescriptors, Trailer& trailer, XrefTable& xrefTable,
	ofstream& pdfCode)
{
	cout << "\nCross reference table element setting start..." << endl;

	// 1. File header
	pdfCode << header.generateCode();
	string tempCode = header.getCode();
	xrefTable.setOffset(0, tempCode.size());

	// 2. Catalog (root)
	pdfCode << catalog.generateCode();
	tempCode = catalog.getCode();	// code byte measuring
	xrefTable.setOffset(catalog.getObjectNo(), tempCode.size());

	// 3. Parent page (pages)
	pdfCode << parentPage.generateCode();
	tempCode.clear();	// code byte measuring
	tempCode = parentPage.getCode();
	xrefTable.setOffset(parentPage.getObjectNo(), tempCode.size());

	// 4. Child pages (page)
	for (vector<Page>::iterator pIterPos = pages.begin();
		pIterPos != pages.end();
		++pIterPos)
	{
		pdfCode << pIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = pIterPos->getCode();
		xrefTable.setOffset(pIterPos->getObjectNo(), tempCode.size());
	}

	// 5. Font (primary font)
	for (vector<PrimaryFont>::iterator fIterPos = primaryFonts.begin();
		fIterPos != primaryFonts.end();
		++fIterPos)
	{
		pdfCode << fIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = fIterPos->getCode();
		xrefTable.setOffset(fIterPos->getObjectNo(), tempCode.size());
	}

	// 6. Text stream
	// UniKS-UCS2-H Adobe Korea1-1
	for (vector<Text>::iterator tIterPos = texts.begin();
		tIterPos != texts.end();
		++tIterPos)
	{
		pdfCode << tIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = tIterPos->getCode();
		xrefTable.setOffset(tIterPos->getObjectNo(), tempCode.size());
	}

	// 7. Subfont (descendant font)
	for (vector<SubFont>::iterator sfIterPos = subFonts.begin();
		sfIterPos != subFonts.end();
		++sfIterPos)
	{
		pdfCode << sfIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = sfIterPos->getCode();
		xrefTable.setOffset(sfIterPos->getObjectNo(), tempCode.size());
	}

	// 8. Font descriptor
	for (vector<FontDescriptor>::iterator fdIterPos = fontDescriptors.begin();
		fdIterPos != fontDescriptors.end();
		++fdIterPos)
	{
		pdfCode << fdIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = fdIterPos->getCode();
		xrefTable.setOffset(fdIterPos->getObjectNo(), tempCode.size());
	}

	// 9. Cross reference table
	pdfCode << xrefTable.generateCode();
	vector<std::pair<int, size_t>>& tempOffsets = xrefTable.getOffsets();

	// 10. Trailer
	pdfCode << trailer.generateCode(tempOffsets.back().second);
}

// Function : readFileBytes
// Description : for image file processing
char* readFileBytes(const char *fileName)
{
	ifstream inputFile(fileName);
	inputFile.seekg(0, ios::end);
	size_t length = inputFile.tellg();
	char *returnAddress = new char[length];
	inputFile.seekg(0, ios::beg);
	inputFile.read(returnAddress, length);
	inputFile.close();
	return returnAddress;
}