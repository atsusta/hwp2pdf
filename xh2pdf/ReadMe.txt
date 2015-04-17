========================================================================
    콘솔 응용 프로그램 : xh2pdf 프로젝트 개요
========================================================================

응용 프로그램 마법사에서 이 xh2pdf 응용 프로그램을 만들었습니다.

xh2pdf 응용 프로그램을 구성하는 각 파일에 대한
요약 설명이 포함되어 있습니다.


xh2pdf.vcxproj
    응용 프로그램 마법사를 사용하여 생성한 VC++ 프로젝트의 기본 프로젝트 파일입니다. 파일을 생성한 Visual C++ 버전에 대한 정보와 응용 프로그램 마법사를 사용하여 선택한 플랫폼, 구성 및 프로젝트 기능에 대한 정보가 포함되어 있습니다.

xh2pdf.vcxproj.filters
    응용 프로그램 마법사를 사용하여 생성된 VC++ 프로젝트의 필터 파일입니다. 이 파일에는 프로젝트의 파일과 필터 간의 연결 정보가 들어 있습니다. 이러한 연결은 특정 노드에서 유사한 확장명으로 그룹화된 파일을 표시하기 위해 IDE에서 사용됩니다. 예를 들어 ".cpp" 파일은 "소스 파일" 필터와 연결되어 있습니다.

xh2pdf.cpp
    기본 응용 프로그램 소스 파일입니다.

HwpXmlInflation.h, HwpXmlInflation.cpp
	pyhwp에서 XML로 파싱된 HWP파일 중 BodyText.xml의 내용을 Object화 하여 저장하는 클래스 정의.

ProducePdf.h, ProducePdf.cpp
	BodyText.xml의 내용을 저장하는 과정, 저장된 정보를 PDF 파일 구조에 맞게 재조합하여 PDF파일을 만드는 클래스의 정의.

Header.h, Header.cpp
	PDF 파일 구조에서 PDF 버전 정보를 저장하는 클래스 정의.

Object.h, Object.cpp
	PDF 파일 구조에서 본문 내용의 정보를 저장하고 PDF 코드를 생성하는 핵심 클래스(PDF object)들의 최상위 부모 클래스(추상 클래스) 정의.

Dictionary.h, Dictionary.cpp
	PDF 파일 구조에서 PDF object가 참고하거나 참조되는 리소스(폰트, 이미지 필터) 정보를 저장하고 PDF 코드를 생성하는 클래스들의 부모 클래스(추상 클래스) 정의.

Stream.h, Stream.cpp
	PDF 파일 구조에서 PDF object 중 바이너리 정보(이미지, 텍스트)를 저장하고 PDF 코드를 생성하는 클래스 정의.

Font.h, Font.cpp
	PDF 파일 구조에서 PDF object 중 폰트 정보를 저장하고 PDF 코드를 생성하는 클래스 정의.

Image.h, Image.cpp
	PDF 파일 구조에서 PDF object 중 이미지 정보를 저장하는 클래스 정의(실제 코드는 없음, 차후 개선이 필요).

CrossReferenceTable.h, CrossReferenceTable.cpp
	PDF 파일 구조에서 PDF 뷰어가 해당 PDF 파일 내의 object에 접근할 수 있도록 byte offset을 기록해주는 'Cross reference table'의 정보를 기록하고 PDF 코드를 생성하는 클래스 정의.

Trailer.h, Trailer.cpp
	PDF 파일 구조에서 전체 문서의 제목, PDF object 트리 구조의 최상위 object 정보, Cross reference table의 byte offset을 저장하고 PDF 코드를 생성하는 클래스 정의.
/////////////////////////////////////////////////////////////////////////////
기타 표준 파일:

StdAfx.h, StdAfx.cpp
    이 파일은 미리 컴파일된 헤더(PCH) 파일 xh2pdf.pch와 미리 컴파일된 형식(PCT) 파일 StdAfx.obj를 빌드하는 데 사용됩니다.

/////////////////////////////////////////////////////////////////////////////
기타 참고:

응용 프로그램 마법사에서 사용하는 "TODO:" 주석은 사용자가 추가하거나 사용자 지정해야 하는 소스 코드 부분을 나타냅니다.

/////////////////////////////////////////////////////////////////////////////
