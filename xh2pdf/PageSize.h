#pragma once

/**
 * Class : PageSize
 * Description : saving page size & orientation
 */
class PageSize
{

public:
	double getMbox0();
	double getMbox1();
	double getMbox2();
	double getMbox3();
	unsigned int getAppliedPageNo();
	void setMbox(double mbox0, double mbox1, double mbox2, double mbox3);
	void setAppliedPageNo(unsigned int appliedPageNumber);

private:
	double mbox0;
	double mbox1;
	double mbox2;
	double mbox3;
	unsigned int appliedPageNo;
};
