#include "stdafx.h"
#include "ProducePdf.h"


ProducePdf::ProducePdf()
{
}


ProducePdf::~ProducePdf()
{
}


// Function : getPageContentInformation
// Description :  for use case 'Get page content information'
bool ProducePdf::getPageContentInformation(char* inputFileName, char* outputFileName, ofstream& pdfCode/*, char* imageFileName*/)
{
	cout << "saving bodytext.xml analysis" << endl;

	// Saving BodyText.xml analysis
	BodyTextInfo bodyTextInfo;

	// Unmarshalling XML file IO using TinyXML.
	TiXmlDocument document;
	document.LoadFile(inputFileName);

	// root node : <BodyText> setting
	TiXmlElement* pRoot = document.FirstChildElement("BodyText");
	if (pRoot == NULL)
	{
		cout << "file does not exists" << '\n';
		return false;
	}

	// page number, size, and orientation setting ready
	vector<PageSize>& tempPageSizeCollection = bodyTextInfo.getPageSizeCollection();

	// get reference of lineSegCollection in BodyText object
	vector<LineSeg>& tempLineSegCollection = bodyTextInfo.getLineSegCollection();

	// image file to byte array
	char* imageFileToByteArray = NULL;

	// depth 1 : <SectionDef> analysis
	for (TiXmlElement* pElem = pRoot->FirstChildElement("SectionDef");
		pElem;
		pElem = pRoot->NextSiblingElement())
	{
		if (pElem)
		{
			// new SectionDef : new page orientation
			bodyTextInfo.setNextPage();

			// tabstop setting
			bodyTextInfo.setDefaultTabStops(atoi(pElem->Attribute("defaultTabStops")));
			//cout << "defaultTabStops : " << pElem->Attribute("defaultTabStops") << endl;

			// depth 2 : 
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
			int downLineSegY			= 0;
			bool isPageChangedInLineSeg = false;

			for (TiXmlElement* pChild = pElem->FirstChildElement();
				pChild;
				pChild = pChild->NextSiblingElement()) 
			{
				// fetch each existing children
				// (PageDef, FootnoteShpe, PageBorderFill, Paragraph)

				// <PageDef> exists, analyze
				if (!memcmp(pChild->Value(), "PageDef", sizeof("PageDef")))
				{
					// PAGE SCOPE : page size setting
					// page orientation alternation (/MediaBox in PDF)
					{
						PageSize tempPageSize;

						if (!memcmp(pChild->Attribute("orientation"), "portrait", sizeof("portrait")))
						{ 
							// portrait orientation
							mbox2 = (double)atoi(pChild->Attribute("width"));
							mbox3 = (double)atoi(pChild->Attribute("height"));
							tempPageSize.setMbox(mbox0, mbox1, mbox2 / 100.0, mbox3 / 100.0);
							cout << "MediaBox : " << mbox0 << " " << mbox1 << 
								" " << mbox2 << 
								" " << mbox3 << endl;
						}
						else
						{ 
							// landscape orientation
							mbox2 = (double)atoi(pChild->Attribute("height"));
							mbox3 = (double)atoi(pChild->Attribute("width"));
							tempPageSize.setMbox(mbox0, mbox1, 
								mbox2 / 100.0, 
								mbox3 / 100.0);
						}
						// page1 - first element, page2 - second element, ...
						tempPageSizeCollection.push_back(tempPageSize);
						tempPageSize.setAppliedPageNumber(bodyTextInfo.getPageNumber());
					} // PAGE SCOPE end

					// PARAGRAPH SCOPE : page margin analyze (/Tm in PDF)
					tm4 = (double)atoi(pChild->Attribute("left-offset"));
					tm5 = (double)(atoi(pChild->Attribute("height")) 
						- atoi(pChild->Attribute("top-offset")) 
						- atoi(pChild->Attribute("header-offset")));
				}

				// <FootNoteShape> exists, analyze
				else if (!memcmp(pChild->Value(), "FootNoteShape", 
					sizeof("FootNoteShape")))
				{
				}

				// <PageBorderFill> exists, analyze
				else if (!memcmp(pChild->Value(), "PageBorderFill", 
					sizeof("PageBorderFill")))
				{
				}

				// *** <Paragraph> exists, analyze ***
				else if (!memcmp(pChild->Value(), "Paragraph", 
					sizeof("Paragraph")))
				{
					// ParagraphCollection -1--owns--*- Paragraph
					//									Paragraph -1--has--1- LineSegCollection
					//														  LineSegCollection -1--owns--*- LineSeg

					// *** <LineSeg> exists, analysis ***
					for (TiXmlElement* pChild1 = pChild->FirstChildElement();
						pChild1;
						pChild1 = pChild1->NextSiblingElement()) // fetch each children (LineSeg)
					{
						// depth 3 : <LineSeg> setting
						if (!memcmp(pChild1->Value(), "LineSeg", sizeof("LineSeg")))
						{
							LineSeg tempLineSeg;

							// Tm setting - font size (e.g. 10 0 0 10 566.9 785.19 matrix)
							tm0 = (double)atoi(pChild1->Attribute("height2"));
							tm3 = (double)atoi(pChild1->Attribute("height"));

							cout << "Text Matrix : \t" << tm0 << " " << 
								tm1 << " " << tm2 << " " << tm3 << " " << 
								tm4 << " " << tm5 << endl;

							tempLineSeg.setTm(tm0 / 100.0, tm1 / 100.0, 
								tm2 / 100.0, tm3 / 100.0, 
								tm4 / 100.0, tm5 / 100.0);

							// page number setting
							lineSegX = atoi(pChild1->Attribute("x"));
							downLineSegY = atoi(pChild1->Attribute("y"));

							if (upperLineSegY > downLineSegY) 
							{
								// paragraph 내에서 페이지가 바뀌었다.
								bodyTextInfo.setNextPage();

								// each paragraph has page number
								tempLineSeg.setPageNumber(bodyTextInfo.getPageNumber());
							}
							else 
							{ 
								// next y postion is smaller than previous y position -> next page
								tempLineSeg.setPageNumber(bodyTextInfo.getPageNumber());
							}

							// Td setting - 줄간격, 들여쓰기/내어쓰기
							td0 = lineSegX / 1000.0;
							td1 = (downLineSegY / 1000.0) * (1000.0 / tm0);
							tempLineSeg.setTd(td0, td1);

							upperLineSegY = downLineSegY;

							// font setting - temporary preference
							tempLineSeg.setFont("NanumGothicCoding");

							// text(string) setting
							string strAnsi = "";
							wstring strUnicode = L"";

							// <Text> / <GShapeObjectControl> analysis
							for (TiXmlElement* pChild2 = pChild1->FirstChildElement();
								pChild2;
								pChild2 = pChild2->NextSiblingElement())
							{
								pChild2->SetCondenseWhiteSpace(false);

								if (!memcmp(pChild2->Value(), "Text", 
									sizeof("Text")))
								{
									// text가 없는 경우를 생각해야함
									const char* strUtf8 = pChild2->GetText();
									if (strUtf8 == 0) // text가 없는 경우
									{
										cout << "no text : insert '\\n'" << endl;
										//tempLineSeg.setSeg(L"\n");
										//tempLineSegCollection.push_back(tempLineSeg);
										//tempParagraph.getLineSegCollection().push_back(tempLineSeg);
									}

									else // text가 있는 경우
									{ 
										// select converting method to re-encode UTF8 string from the XML file
										
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
										//tempLineSegCollection.push_back(tempLineSeg); // each paragraph has line one or more segments


										// 2. UTF8Conv macro functions
										UTF8_CONVERSION;
										strAnsi = UTF82A(strUtf8); // ANSI string for console display
										wstring strUnicodeSegment = UTF82W(strUtf8); // UNICODE string for pdf code generating
										cout << strAnsi;
										cout << '\n';

										strUnicode += strUnicodeSegment;
									}
								}
								else if (!memcmp(pChild1->Value(), "GShapeObjectControl", 
									sizeof("GShapeObjectControl")))
								{
									//imageFileToByteArray = readFileBytes(imageFileName);
									//tempImageCollection.push_back(imageFileToByteArray);
									//cout << "image file test" << endl;
								}
							} // text(string) setting ends
							tempLineSeg.setSeg(strUnicode); // set line segment (TJ element)
							tempLineSeg.setLength(sizeof(strUnicode)); // set line segment length for xrefTable
							tempLineSegCollection.push_back(tempLineSeg);
						} // <LineSeg> setting (depth 3) ends
					}
				} // PageDef, Paragraph (depth 2) ends
			} 
		}
	} // SectionDef (depth 1) ends


	// assign paragraph to text objects ordered by page number
	vector<Text> textCollection;

	// 전체 문서에 있는 폰트의 종류
	vector<string> documentFontCollection;

	for (unsigned int i = 1; 
		i <= bodyTextInfo.getPageNumber(); 
		i++)
	{
		// assign page content by page number
		// verify page number in paragraph included in bodyText.xml
		// and allocate data to paragraph included in PDF text object.
		Text aText;
		aText.setObjectNumber(counter<Object>::objects_alive);

		// Text object의 line segment collection을 reference에 연결
		vector<LineSeg>& tempInPageLineSegCollection = aText.getInPageLineSegCollection();

		for (unsigned int j = 1; 
			j <= tempLineSegCollection.size(); 
			j++)
		{
			if (tempLineSegCollection[j - 1].getPageNumber() == i)
			{
				tempInPageLineSegCollection.push_back(tempLineSegCollection[j - 1]);
			}
		}

		// 한 Page에 한 Text가 있는데 그 안에 총 font의 개수가 몇 개인지 어떻게 파악할 것인가
		// InPageParagraph & InPageFont
		// 한 문서에 존재하는 특정 font의 개수는 유일해야 한다.
		vector<string> inPageFontCollection;

		size_t textLength = 0;

		for (vector<LineSeg>::iterator IterPos = tempInPageLineSegCollection.begin();
			IterPos != tempInPageLineSegCollection.end();
			++IterPos)
		{
			// 폰트의 종류 수 세기
			inPageFontCollection.push_back(IterPos->getFont());
			textLength += IterPos->getLength();
		}
		aText.setLength(textLength); // set text length for cross reference table

		// 폰트 종류의 중복 없애기 - unique 함수 사용
		vector<string>::iterator it;
		it = unique(inPageFontCollection.begin(), inPageFontCollection.end());
		inPageFontCollection.resize(distance(inPageFontCollection.begin(), it));

		// used Resource(font number) setting 
		for (unsigned int i = 1; i <= inPageFontCollection.size(); i++)
			aText.setFontNumber(i); // /F1 /F2 /F3 /F4 ...

		textCollection.push_back(aText);
		//cout << "objects created : " << counter<Object>::objects_alive << endl;

		// 전체 문서에 존재하는 폰트 파악
		for (vector<string>::iterator dfIterPos = inPageFontCollection.begin();
			dfIterPos != inPageFontCollection.end();
			++dfIterPos)
		{
			documentFontCollection.push_back(*dfIterPos);
			// 중복 없애기
			vector<string>::iterator documentIt;
			documentIt = unique(documentFontCollection.begin(), documentFontCollection.end());
			documentFontCollection.resize(distance(documentFontCollection.begin(), documentIt));
		}

	}

	// =-=-=-=-=-=-=-=-= Initialize new pdf objects =-=-=-=-=-=-=-=-=
	cout << "Initialize new pdf objects" << endl;

	initializeNewPdfObject(bodyTextInfo, tempPageSizeCollection, documentFontCollection, textCollection, outputFileName, pdfCode);
	return true;

}


// Fuction : InitializeNewPdfObject
// Description : for use case 'Initialize new PDF objects'
void ProducePdf::initializeNewPdfObject(BodyTextInfo bodyTextInfo, 
	vector<PageSize>& tempPageSizeCollection, 
	vector<string> documentFontCollection, 
	vector<Text> textCollection, 
	char* outputFileName, 
	ofstream& pdfCode)
{
	Header header; // file header
	header.setPDFVersion("%PDF-1.7");

	Catalog catalog; // looks parent page
	catalog.setObjectNumber(counter<Object>::objects_alive);

	Pages parentPage; // root of page collection
	parentPage.setObjectNumber(counter<Object>::objects_alive);

	vector<Page> pageCollection; // multiple pages

	unsigned int pageIndex = 0;
	for (unsigned int i = 0; i < bodyTextInfo.getPageNumber(); i++)
	{
		// page object - set object number int PDF code
		Page aPage;
		aPage.setObjectNumber(counter<Object>::objects_alive);
		aPage.referenceObjectNumber(parentPage.getObjectNumber());

		// set mediabox
		if (tempPageSizeCollection.size() == 1)
		{
			aPage.setMediaBox(
				tempPageSizeCollection[pageIndex].getMbox0(),
				tempPageSizeCollection[pageIndex].getMbox1(),
				tempPageSizeCollection[pageIndex].getMbox2(),
				tempPageSizeCollection[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				tempPageSizeCollection[pageIndex].getMbox0(),
				tempPageSizeCollection[pageIndex].getMbox1(),
				tempPageSizeCollection[pageIndex].getMbox2(),
				tempPageSizeCollection[pageIndex].getMbox3()
				);
		}
		else
		{
			pageIndex++;
			aPage.setMediaBox(
				tempPageSizeCollection[pageIndex].getMbox0(),
				tempPageSizeCollection[pageIndex].getMbox1(),
				tempPageSizeCollection[pageIndex].getMbox2(),
				tempPageSizeCollection[pageIndex].getMbox3()
				);
			aPage.setCropBox(
				tempPageSizeCollection[pageIndex].getMbox0(),
				tempPageSizeCollection[pageIndex].getMbox1(),
				tempPageSizeCollection[pageIndex].getMbox2(),
				tempPageSizeCollection[pageIndex].getMbox3()
				);
		}
		// push page to collection
		pageCollection.push_back(aPage);
	}

	// initialize font collection
	vector<PrimaryFont> primaryFontCollection;
	vector<SubFont> subFontCollection;
	vector<FontDescriptor> fontDescriptorCollection;

	// font saving in whole document scope
	for (vector<string>::iterator fIterPos = documentFontCollection.begin();
		fIterPos != documentFontCollection.end();
		++fIterPos)
	{
		// set primary font
		PrimaryFont aFont;
		aFont.setObjectNumber(counter<Object>::objects_alive);

		if (!fIterPos->compare("NanumGothicCoding"))
		{
			// set subfont and font descriptor
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
			aSubfont.setDW(1000);
			aSubfont.setCIDSystemInfo("<<\n/Supplement 1\n/Ordering(Korea1)\n/Registry(Adobe)\n>>");
			aFont.referenceObjectNumber(aSubfont.getObjectNumber()); // refereneces descendant font

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
			aSubfont.referenceObjectNumber(aFontDescriptor.getObjectNumber()); // subfont references font descriptor

			// save CID subfont and font descriptor to each collection class
			subFontCollection.push_back(aSubfont);
			fontDescriptorCollection.push_back(aFontDescriptor);
		} // if there isn't first font for korean, add font Nanum Gothic Coding.
		else if (!fIterPos->compare("TimesNewRoman"))
		{
			aFont.setSubtype("Type1");
			aFont.setBaseFont(fIterPos->data());
		} // if there isn't first font for english, add font Times New Roman.

		// save primary font
		primaryFontCollection.push_back(aFont);
	}

	// catalog looks at parentPage
	catalog.referenceObjectNumber(parentPage.getObjectNumber());

	// multiple pages(collection) and parent page
	// page and font reference setting
	for (vector<Page>::iterator pIterPos = pageCollection.begin();
		pIterPos != pageCollection.end();
		++pIterPos)
	{
		// parent page - child page reference setting
		// Kids [ x 0 R y 0 R z 0 R ... ]
		parentPage.referenceObjectNumber(pIterPos->getObjectNumber());
		pIterPos->referenceObjectNumber(parentPage.getObjectNumber());
		parentPage.setCount();

		// page - primary font reference setting
		for (vector<PrimaryFont>::iterator fIterPos = primaryFontCollection.begin();
			fIterPos != primaryFontCollection.end();
			++fIterPos)
		{
			pIterPos->setResources(fIterPos->getObjectNumber());
		}
	}

	// Initialize Cross reference table
	XrefTable xrefTable;

	// Initialize File Trailer 
	Trailer trailer;
	trailer.setRoot(catalog.getObjectNumber());
	trailer.setSize(counter<Object>::objects_alive);

	// =-=-=-=-=-=-=-=-= Connect page content to page =-=-=-=-=-=-=-=-=
	cout << "Connect page content to page" << endl;
	this->connectPageContentToPage(textCollection, pageCollection);

	// =-=-=-=-=-=-=-=-= Generate a new pdf code =-=-=-=-=-=-=-=-=
	cout << "Generate a new pdf code" << endl;

	char* outputFileNameWithExtension = NULL;
	outputFileNameWithExtension = new char[strlen((char*)outputFileName) + 5];

	// get original file name
	int hasFileName = strncpy_s(outputFileNameWithExtension,
		strlen((char*)outputFileName) + 5,
		(char*)outputFileName,
		strlen((char*)outputFileName));

	if (hasFileName != 0) {
		cout << "file name(strncpy_s) error" << endl;
		exit(1);

		// concatenate file extension "xxx.pdf"
		int hasPdfName = strncat_s(outputFileNameWithExtension,
			strlen((char*)outputFileName) + 5, ".pdf", 4);

		if (hasPdfName != 0) {
			cout << "file name(strncat_s) error" << endl;
			exit(1);
		}
		else
		{
			cout << "processing : " << outputFileName << " -> " << outputFileNameWithExtension << endl;
		}
	}


	// output file processing
	pdfCode.open(outputFileNameWithExtension, (ios::out | ios::binary));
	pdfCode.seekp(0, ios::beg); // 다시 시작으로 갖다놓기

	this->generateANewPdfCode(
		header,
		catalog,
		parentPage,
		pageCollection,
		primaryFontCollection,
		textCollection,
		subFontCollection,
		fontDescriptorCollection,
		trailer,
		xrefTable,
		pdfCode
		);

	pdfCode.seekp(0, ios::end); // 끝 위치 이동
	size_t length = pdfCode.tellp(); // 파일 사이즈 구하기

	pdfCode.close();
}


// Fuction : connectPageContentToPage
// Description : for use case 'Connect page content to page'
void ProducePdf::connectPageContentToPage(vector<Text>& textCollection, vector<Page>& pageCollection)
{
	vector<Text>::iterator tIterPos = textCollection.begin();
	vector<Page>::iterator pIterPos = pageCollection.begin();

	while (tIterPos != textCollection.end() && pIterPos != pageCollection.end())
	{
		pIterPos->setContents(tIterPos->getObjectNumber());
		tIterPos++;
		pIterPos++;
	}
}


// Function : generateANewPdfCode
// Description : for use case 'Generate a new pdf code'
void ProducePdf::generateANewPdfCode(
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
	ofstream& pdfCode
	)
{
	cout << "\ncross reference table element setting start..." << endl;

	// 1. file header
	pdfCode << header.generateCode();
	string tempCode = header.getCode();
	xrefTable.setOffset(0, tempCode.size());

	// 2. catalog (root)
	pdfCode << catalog.generateCode();
	tempCode = catalog.getCode();	// code byte measuring
	xrefTable.setOffset(catalog.getObjectNumber(), tempCode.size());

	// 3. parent page (pages)
	pdfCode << parentPage.generateCode();
	tempCode.clear();	// code byte measuring
	tempCode = parentPage.getCode();
	xrefTable.setOffset(parentPage.getObjectNumber(), tempCode.size());

	// 4. child pages (page)
	for (vector<Page>::iterator pIterPos = pageCollection.begin();
		pIterPos != pageCollection.end();
		++pIterPos)
	{
		pdfCode << pIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = pIterPos->getCode();
		xrefTable.setOffset(pIterPos->getObjectNumber(), tempCode.size());
	}

	// 5. font (primary font)
	for (vector<PrimaryFont>::iterator fIterPos = primaryFontCollection.begin();
		fIterPos != primaryFontCollection.end();
		++fIterPos)
	{
		pdfCode << fIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = fIterPos->getCode();
		xrefTable.setOffset(fIterPos->getObjectNumber(), tempCode.size());
	}

	// 6. text stream
	// UniKS-UCS2-H Adobe Korea1-1
	for (vector<Text>::iterator tIterPos = textCollection.begin();
		tIterPos != textCollection.end();
		++tIterPos)
	{
		pdfCode << tIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = tIterPos->getCode();
		xrefTable.setOffset(tIterPos->getObjectNumber(), tempCode.size());
	}

	// 7. subfont (descendant font)
	for (vector<SubFont>::iterator sfIterPos = subFontCollection.begin();
		sfIterPos != subFontCollection.end();
		++sfIterPos)
	{
		pdfCode << sfIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = sfIterPos->getCode();
		xrefTable.setOffset(sfIterPos->getObjectNumber(), tempCode.size());
	}

	// 8. font descriptor
	for (vector<FontDescriptor>::iterator fdIterPos = fontDescriptorCollection.begin();
		fdIterPos != fontDescriptorCollection.end();
		++fdIterPos)
	{
		pdfCode << fdIterPos->generateCode();
		tempCode.clear();	// code byte measuring
		tempCode = fdIterPos->getCode();
		xrefTable.setOffset(fdIterPos->getObjectNumber(), tempCode.size());
	}

	// 9. cross reference table
	pdfCode << xrefTable.generateCode();
	vector<std::pair<int, size_t>>& tempOffsetCollection = xrefTable.getOffsetCollection();

	// 10. trailer
	pdfCode << trailer.generateCode(tempOffsetCollection.back().second);
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