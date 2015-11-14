#pragma once

#include "stdafx.h"
#include "ProducePdf.h"
#include "tinyxml.h"
#include "UTF8Conv.h"
#include <algorithm>
#include <iostream>

using namespace std;

/**
 * Class : Header, Catalog, Pages, Page, PrimaryFont, SubFont, FontDescriptor, 
 *  crossReferenceTable, Trailer
 * Description : All member functions in Pdf structure classes
 *
 * Function Overrided :
 *  void generateCode(string& target),
 *  void referenceObjectNumber(int objectNum)
 *
 * Description :
 *  Implementations of Functions that generate Pdf structure code
 *
 * Parameters :
 *  stringstream& target - the target stream of string for Pdf structure code,
 *  int objectNum - the number of object that is referenced by
 *  Return Value : none
 *
 * Revisions :
 *  1. When : 140606
 *   What : getnerateCode(string& target) argument changed to string* ... failed.
 *   If so, "target << " code is unavailable
 *   because << operator need to be additionally overloaded!! ( POINTER << VARIABLE )
 *  
 *  2. When : 140607
 *   What : implementation error correction
 *   two functions must be implemented by all derived subclasses.
 *  
 *  3. When : 140616
 *   What : stringstream type inspection
 *   runtime-error cannot be conquered...
 *  
 *  4. When : 140705
 *   What : changed generateCode()
 *   using string variable
 *   added function string.append() and string.push_back()
 *  
 *  5. When : 140803
 *   What : ProducePdf()
 *   use case realization
 */

ProducePdf::ProducePdf()
{
}

ProducePdf::~ProducePdf()
{
}

// Function : getPageContentInformation
// Description : for use case 'Get page content information'
bool ProducePdf::getPageContentInformation(char* inputFileName, 
    char* outputFileName, ofstream& pdfCode/*, char* imageFileName*/)
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
	vector<PageSize>& documentPageSizes = bodyTextInfo.getPageSizes();

	// Get reference of lineSegs in BodyText object
	vector<LineSeg>& documentLineSegs = bodyTextInfo.getLineSegs();

	// Image file to byte array
	char* imageFileToByteArray = NULL;

	// Depth 1 : <SectionDef> analysis
	for (TiXmlElement* pElem = pRoot->FirstChildElement("SectionDef");
		pElem;
		pElem = pRoot->NextSiblingElement())
	{
		// Find new <SectionDef> tag : new page orientation
		bodyTextInfo.setNextPage();

		// Tabstop setting
		int defaultTabStops = atoi(pElem->Attribute("defaultTabStops"));
		bodyTextInfo.setDefaultTabStops(defaultTabStops);

		// Depth 2 : 
		// <PageDef>, <FootNoteShape>, <PageBorderFill>, 
        // <Paragraph> tag analysis
		// findPageContents(pElem, bodyTextInfo);
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
		int lowerLineSegY			= 0;
		bool isPageChangedInLineSeg = false;

		for (TiXmlElement* pChild = pElem->FirstChildElement();
			pChild;
			pChild = pChild->NextSiblingElement()) 
		{
			// Fetch each existing children
			// (PageDef, FootnoteShpe, PageBorderFill, Paragraph)
			// findPageInformations(pChild, bodyTextInfo)
			bool hasPageDef = !memcmp(pChild->Value(), 
                "PageDef", sizeof("PageDef"));
			bool hasFootNoteShape = !memcmp(pChild->Value(),
                "FootNoteShape", sizeof("FootNoteShape"));
			bool hasPageBorderFill = !memcmp(pChild->Value(), 
                "PageBorderFill", sizeof("PageBorderFill"));
			bool hasParagraph = !memcmp(pChild->Value(), 
                "Paragraph", sizeof("Paragraph"));
			if (hasPageDef)
			{
				// Find and analyze <PageDef> tags,
				// Set page sizes
				{
					// Set page orientation (/MediaBox in PDF)
					PageSize pageSize;

					bool isPortrait = !memcmp(pChild->Attribute("orientation"), 
                        "portrait", sizeof("portrait"));
					if (isPortrait)
					{ 
						// Portrait orientation
						mbox2 = (double)atoi(pChild->Attribute("width"));
						mbox3 = (double)atoi(pChild->Attribute("height"));
						pageSize.setMbox(mbox0, mbox1, mbox2 / 100.0, mbox3 / 100.0);
						cout << "MediaBox : " << mbox0 << " " << mbox1 << 
                            " " << mbox2 << " " << mbox3 << endl;
					}
					else
					{ 
						// Landscape orientation
						mbox2 = (double)atoi(pChild->Attribute("height"));
						mbox3 = (double)atoi(pChild->Attribute("width"));
						pageSize.setMbox(mbox0, mbox1, mbox2 / 100.0, mbox3 / 100.0);
					}

                    // Collect media boxes(page sizes) per page
					// N-th page is n-th element
					documentPageSizes.push_back(pageSize);
					pageSize.setAppliedPageNo(bodyTextInfo.getPageNo());
				} // end PAGE SCOPE

				// PARAGRAPH SCOPE : analyze page margin (/Tm in PDF)
				tm4 = (double) atoi(pChild->Attribute("left-offset"));
				tm5 = (double) ( atoi(pChild->Attribute("height")) 
					- atoi(pChild->Attribute("top-offset")) 
					- atoi(pChild->Attribute("header-offset")) );
			}

			if (hasFootNoteShape)
			{
				// <FootNoteShape> analysis
				// blank
			}

			if (hasPageBorderFill)
			{
				// <PageBorderFill> analysis
				// blank
			}

			if (hasParagraph)
			{
				// Find and analyze <Paragraph> tags
				// One <Paragraph> tag has many <LineSeg> tags
				// findLineSegs(pChild, bodyTextInfo);
				for (TiXmlElement* pChild1 = pChild->FirstChildElement();
					pChild1;
					pChild1 = pChild1->NextSiblingElement())
				{
					// Find and analyze <LineSeg> tags
					bool hasLineSeg = !memcmp(pChild1->Value(), "LineSeg", 
                        sizeof("LineSeg"));
					if (hasLineSeg)
					{
						LineSeg aLineSeg;

						// Tm setting - font size
                        // (e.g. 10 0 0 10 566.9 785.19 matrix)
						tm0 = (double) atoi(pChild1->Attribute("height2"));
						tm3 = (double) atoi(pChild1->Attribute("height"));

						cout << "Text Matrix : \t" << tm0 << " " << tm1 << 
							" " << tm2 << " " << tm3 << " " << tm4 << " " << 
							tm5 << endl;

						aLineSeg.setTm(tm0 / 100.0, tm1 / 100.0, tm2 / 100.0, 
							tm3 / 100.0, tm4 / 100.0, tm5 / 100.0);

						// Page number setting
						lineSegX = atoi(pChild1->Attribute("x"));
						lowerLineSegY = atoi(pChild1->Attribute("y"));

						if (upperLineSegY > lowerLineSegY) 
						{
							// Offset of line segment got over in Paragraph scope
							bodyTextInfo.setNextPage();

							// Each paragraph has page number
							aLineSeg.setPageNo(bodyTextInfo.getPageNo());
						}
						else 
						{ 
							// Current y postion is smaller than previous
                            // New page has been occured
							aLineSeg.setPageNo(bodyTextInfo.getPageNo());
						}

						// Td setting for line height and indentation
						td0 = lineSegX / 1000.0;
						td1 = (lowerLineSegY / 1000.0) * (1000.0 / tm0);
						aLineSeg.setTd(td0, td1);

						upperLineSegY = lowerLineSegY;

						// Set font of line segment
						// In this version, all PDF documents has the font 
                        // 'Nanum Gothic Coding' only...
						aLineSeg.setFont("NanumGothicCoding");

						// findLineSegContents(aLineSeg, bodyTextInfo)

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
									// Re-encode UTF8 string from the XML file
										
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

									//aLineSeg.setSeg(strMultibyte);
									//aLineSegs.push_back(aLineSeg); // each paragraph has line one or more segments


									// 2. UTF8Conv macro functions
									UTF8_CONVERSION;

									// ANSI string for console display
									strAnsi = UTF82A(strUtf8);

									// UNICODE string for file output
									wstring strUnicodeSegment = UTF82W(strUtf8);
										
									// Unit test - display console
									cout << strAnsi;
									cout << '\n';

									// Accumulate line segment
									strUnicode += strUnicodeSegment;
								}
							}
							
                            if (hasImage)
							{
								// When document has an image object
								//imageFileToByteArray = readFileBytes(imageFileName);
								//aImages.push_back(imageFileToByteArray);
								//cout << "image file test" << endl;
							}
						} // End text(string) setting

						// Set line segment content (TJ element in PDF)
						aLineSeg.setSeg(strUnicode);

						// Set line segment length for crossReferenceTable
						aLineSeg.setLength(sizeof(strUnicode));
							
						// Add to whole LineSeg collection
						documentLineSegs.push_back(aLineSeg);
					} 
				} // End loop for <LineSeg> setting (depth 3)
			} 
		} // End loop for <PageDef>, <Paragraph> (depth 2)
	} // End loop for <SectionDef> (depth 1)

	// Paragraph assigned by text objects ordered by page number
	vector<Text> texts;

	// 전체 문서에 있는 폰트의 종류
	vector<string> documentFonts;

	for (unsigned int i = 1; i <= bodyTextInfo.getPageNo(); i++)
	{
		// Assign page content by page number
		// Verify page number in paragraph included in bodyText.xml
		// and allocate attribute value to text object.
		Text aText;
		aText.setObjectNo(counter<Object>::objects_alive);

		// Get line segment collection in text object
		vector<LineSeg>& textLineSegs = aText.getLineSegs();

		for (unsigned int j = 1; j <= documentLineSegs.size(); j++)
		{
			if (documentLineSegs[j - 1].getPageNo() == i)
			{
				// When text object has a same page number of line segment
				// Collect line segment contents to text object
				textLineSegs.push_back(documentLineSegs[j - 1]);
			}
		}

		// Analize what fonts the text object has.
		// Font names must be identical.
		vector<string> pageScopeFonts;
		size_t textLength = 0;

		for (vector<LineSeg>::iterator i = textLineSegs.begin();
			i != textLineSegs.end();
			++i)
		{
			// Count up fonts
			pageScopeFonts.push_back(i->getFont());
			textLength += i->getLength();
		}

		// Set text length for cross reference table
		aText.setLength(textLength);

		// Count number of fonts
		vector<string>::iterator i;
		i = unique(pageScopeFonts.begin(), pageScopeFonts.end());
		pageScopeFonts.resize(distance(pageScopeFonts.begin(), i));

		for (unsigned int i = 1; i <= pageScopeFonts.size(); i++) {
			// Set font numbers 
			// Represented in PDF as '/F1 /F2 /F3 /F4 ...'
			aText.setFontNo(i);
		}

		texts.push_back(aText);

		// Count up all the fonts in document
		for (vector<string>::iterator i = pageScopeFonts.begin();
			i != pageScopeFonts.end();
			++i)
		{
			documentFonts.push_back(*i);

			// Count number of fonts as unique
			vector<string>::iterator j;
			j = unique(documentFonts.begin(), documentFonts.end());
			
			// Resize collection
			documentFonts.resize(distance(documentFonts.begin(), j));
		}
	}

	/* Initialize new pdf objects */
	cout << "Initialize new pdf objects" << endl;

	initializeNewPdfObject(bodyTextInfo, documentPageSizes, documentFonts, 
        texts, outputFileName, pdfCode);
	return true;

}

// Fuction : InitializeNewPdfObject
// Description : for use case 'Initialize new PDF objects'
void ProducePdf::initializeNewPdfObject(BodyTextInfo bodyTextInfo, 
	vector<PageSize>& pageSizes, vector<string> documentFonts, 
	vector<Text> texts, char* outputFileName, ofstream& pdfCode)
{
	// Initialize File header
	Header header;
	header.setPdfVersion("%PDF-1.7");

	// Initialize Catalog looks parent page
	Catalog catalog;
	catalog.setObjectNo(counter<Object>::objects_alive);

	// Initialize Root of page collection
	Pages parentPage;
	parentPage.setObjectNo(counter<Object>::objects_alive);

	// Initialize page collection
	vector<Page> pages;
	unsigned int pageIndex = 0;
	for (unsigned int i = 0; i < bodyTextInfo.getPageNo(); i++)
	{
		// Page object - set object number int PDF code
		Page aPage;
		aPage.setObjectNo(counter<Object>::objects_alive);
		aPage.referenceObjectNo(parentPage.getObjectNo());

		// Set mediabox
		if (pageSizes.size() == 1)
		{
			aPage.setMediaBox(
				pageSizes[pageIndex].getMbox0(),
				pageSizes[pageIndex].getMbox1(),
				pageSizes[pageIndex].getMbox2(),
				pageSizes[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				pageSizes[pageIndex].getMbox0(),
				pageSizes[pageIndex].getMbox1(),
				pageSizes[pageIndex].getMbox2(),
				pageSizes[pageIndex].getMbox3()
				);
		}
		else
		{
			pageIndex++;
			aPage.setMediaBox(
				pageSizes[pageIndex].getMbox0(),
				pageSizes[pageIndex].getMbox1(),
				pageSizes[pageIndex].getMbox2(),
				pageSizes[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				pageSizes[pageIndex].getMbox0(),
				pageSizes[pageIndex].getMbox1(),
				pageSizes[pageIndex].getMbox2(),
				pageSizes[pageIndex].getMbox3()
				);
		}

		// Add page object to collection
		pages.push_back(aPage);
	}

	// Initialize font collections
	vector<PrimaryFont> primaryFonts;
	vector<SubFont> subFonts;
	vector<FontDescriptor> fontDescriptors;
	for (vector<string>::iterator i = documentFonts.begin();
		i != documentFonts.end();
		++i)
	{
		// Font saving in whole document scope
		// Set primary font
		PrimaryFont aFont;
		aFont.setObjectNo(counter<Object>::objects_alive);

		if (!i->compare("NanumGothicCoding"))
		{
			// When meets font (e.g. Nanum Gothic Coding), set subfont and 
            // font descriptor
			SubFont aSubfont;
			aSubfont.setObjectNo(counter<Object>::objects_alive);
			FontDescriptor aFontDescriptor;
			aFontDescriptor.setObjectNo(counter<Object>::objects_alive);

			aFont.setSubtype("/Type0");
			aFont.setBaseFont(i->data());
			aFont.setEncoding("/UniKS-UCS2-H");

			aSubfont.setSubtype("/CIDFontType2");
			aSubfont.setBaseFont(i->data());
			//aSubfont.setFontDescriptor("")
			aSubfont.setW(0, 95, 500);
			aSubfont.setDw(1000);
			aSubfont.setCidSystemInfo(
				"<<\n/Supplement 1\n/Ordering(Korea1)\n/Registry(Adobe)\n>>");

			// Primary font refereneces descendant fonts
			aFont.referenceObjectNo(aSubfont.getObjectNo());

            // Set font design attributes
			aFontDescriptor.setFontName(i->data());
			aFontDescriptor.setFontFamily(i->data());
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

			// Subfont references font descriptor
			aSubfont.referenceObjectNo(aFontDescriptor.getObjectNo());

			// Save CID subfont and font descriptor to each collection
			subFonts.push_back(aSubfont);
			fontDescriptors.push_back(aFontDescriptor);
		}
		else if (!i->compare("TimesNewRoman"))
		{
			// If there isn't first font for english, add font Times New Roman.
			aFont.setSubtype("Type1");
			aFont.setBaseFont(i->data());
		}

		// Save the primary font
		primaryFonts.push_back(aFont);
	}
	
	// Catalog looks at parentPage
	catalog.referenceObjectNo(parentPage.getObjectNo());
	cout << "// Catalog looks at parentPage" << endl;

	// Multiple pages and parent page
	// Page and font reference setting
	for (vector<Page>::iterator i = pages.begin();
		i != pages.end();
		++i)
	{
		// Child page reference setting in Parent page
		// Represented as 'Kids [ x 0 R y 0 R z 0 R ... ]'
		parentPage.referenceObjectNo(i->getObjectNo());
		i->referenceObjectNo(parentPage.getObjectNo());
		parentPage.setCount();

		// Primary font reference setting in child page
		for (vector<PrimaryFont>::iterator j = primaryFonts.begin();
			j != primaryFonts.end();
			++j)
		{
			i->setResources(j->getObjectNo());
		}
	}

	// Initialize Cross reference table
	CrossReferenceTable crossReferenceTable;
	cout << "// Initialize Cross reference table" << endl;

	// Initialize File Trailer 
	Trailer trailer;
	trailer.setRoot(catalog.getObjectNo());
	trailer.setSize(counter<Object>::objects_alive);

	/* Connect page content to page */
	this->connectPageContentToPage(texts, pages);
	cout << "Connect page content to page" << endl;

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
			cout << "processing : " << outputFileName << " -> " << 
                outputFileNameWithExtension << endl;
		}
	}

	// Output file processing
	pdfCode.open(outputFileNameWithExtension, (ios::out | ios::binary));

	// Move pointer to begin of file
	pdfCode.seekp(0, ios::beg);

	// Generate final PDF code
	this->generateNewPdfCode(header, catalog, parentPage, pages, primaryFonts,
		texts, subFonts, fontDescriptors, trailer, crossReferenceTable, pdfCode);

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
	vector<Text>::iterator i = texts.begin();
	vector<Page>::iterator j = pages.begin();

	while ((i != texts.end()) && (j != pages.end()))
	{
		// Connect content in text(LineSegs) to page
		j->setContents(i->getObjectNo());

		// Go to next content
		i++;
		j++;
	}
}

// Function : generateNewPdfCode
// Description : for use case 'Generate a new pdf code'
void ProducePdf::generateNewPdfCode(Header& header, Catalog& catalog, 
    Pages& parentPage, 
    vector<Page>& pages,
    vector<PrimaryFont>& primaryFonts,
	vector<Text>& texts,
    vector<SubFont>& subFonts,
	vector<FontDescriptor>& fontDescriptors,
    Trailer& trailer, CrossReferenceTable& crossReferenceTable,
	ofstream& pdfCode)
{
	cout << "\nCross reference table element setting start..." << endl;

	// 1. File header code generating
	// Measure byte offset (Generating cross reference table)
	pdfCode << header.generateCode();
	string code = header.getCode();
	crossReferenceTable.setOffset(0, code.size());

	// 2. Catalog (root) code generating
	// Measure byte offset (Generating cross reference table)
	pdfCode << catalog.generateCode();
	code = catalog.getCode();
	crossReferenceTable.setOffset(catalog.getObjectNo(), code.size());

	// 3. Parent page (Pages) code generating
    // Measure byte offset (Generating cross reference table)
	pdfCode << parentPage.generateCode();
	code.clear();
	code = parentPage.getCode();
	crossReferenceTable.setOffset(parentPage.getObjectNo(), code.size());

	// 4. Child pages (Page) code generating
	for (vector<Page>::iterator i = pages.begin();
		i != pages.end();
		++i)
	{
		pdfCode << i->generateCode();

        // Measure byte offset (Generating cross reference table)
		code.clear();
		code = i->getCode();
		crossReferenceTable.setOffset(i->getObjectNo(), code.size());
	}

	// 5. Font (PrimaryFont) code generating
	for (vector<PrimaryFont>::iterator i = primaryFonts.begin();
		i != primaryFonts.end();
		++i)
	{
		pdfCode << i->generateCode();
		
        // Measure byte offset (Generating cross reference table)
		code.clear();
		code = i->getCode();
		crossReferenceTable.setOffset(i->getObjectNo(), code.size());
	}

	// 6. Text stream code generating
	// UniKS-UCS2-H Adobe Korea1-1
	for (vector<Text>::iterator i = texts.begin();
		i != texts.end();
		++i)
	{
		pdfCode << i->generateCode();

        // Measure byte offset (Generating cross reference table)
		code.clear();
		code = i->getCode();
		crossReferenceTable.setOffset(i->getObjectNo(), code.size());
	}

	// 7. Subfont (descendant font) code generating
	for (vector<SubFont>::iterator i = subFonts.begin();
		i != subFonts.end();
		++i)
	{
		pdfCode << i->generateCode();

        // Measure byte offset (Generating cross reference table)
		code.clear();
		code = i->getCode();
		crossReferenceTable.setOffset(i->getObjectNo(), code.size());
	}

	// 8. Font descriptor code generating
	for (vector<FontDescriptor>::iterator i = fontDescriptors.begin();
		i != fontDescriptors.end();
		++i)
	{
		pdfCode << i->generateCode();
		
        // Measure byte offset (Generating cross reference table)
		code.clear();
		code = i->getCode();
		crossReferenceTable.setOffset(i->getObjectNo(), code.size());
	}

	// 9. Cross reference table code generating
	pdfCode << crossReferenceTable.generateCode();
	vector<pair<int, size_t>>& offsets = crossReferenceTable.getOffsets();

	// 10. Trailer code generating
	pdfCode << trailer.generateCode(offsets.back().second);
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