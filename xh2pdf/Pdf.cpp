#include "stdafx.h"
#include "Pdf.h"
#include <iomanip>
using namespace std;

/**
 * Class : Header, Catalog, Pages, Page, PrimaryFont, SubFont, FontDescriptor, xrefTable, Trailer
 * Description : All member functions in Pdf structure classes
 *
 * Function : void generateCode(string& target), void referenceObjectNumber(int objectNum)
 * Description : Implementations of Functions that generate Pdf structure code
 * Parameters : stringstream& target - the target stream of string for Pdf structure code, int objectNum - the number of object that is referenced by
 * Return Value : none
 *
 * Revisions :
 * 1. When : 140606
 * 	What : getnerateCode(string& target) argument changed to string* ... failed.
 *	If so, "target << " code is unavailable because << operator need to be additionally overloaded!! ( POINTER << STUFFS )
 *
 * 2. When : 140607
 *	What : implementation error correction
 *	two functions must be implemented by all derived subclasses.
 *
 * 3. When : 140616
 * What : stringstream type inspection
 * runtime-error cannot be conquered...
 *
 * 4. When : 140705
 * What : generateCode() change
 * using string variable / string.append(), string.push_back() function
 *
 * 5. When : 140803
 * What : ProducePdf()
 * use case realization 
 */

