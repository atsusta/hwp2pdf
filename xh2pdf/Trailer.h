#pragma once

#include <string>
using namespace std;

/**
 * Class : Trailer
 * Description : tail of cross reference table
 * It sets root in 
 *    1. file body structure 
 *    2. file information(this is optional) 
 *    3. offset of cross reference table
 */
class Trailer
{

public:
	void setSize(int size);
	void setRoot(int root);
	string generateCode();
	string generateCode(size_t XrefTableOffset);

private:
	int size;
	int root;
	string code;

};
