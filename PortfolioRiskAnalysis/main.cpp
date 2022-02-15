/**
* MFE C++ 2022 - Assignment 7
*
*
*/
#include <iostream>
#include "classes.h"
#include <fstream>
using namespace std;

int main()
{
	//Part B: Efficient Frontier
	Market m2(-0.3, 0.1, 0.2, 0.03, 0.07, 0.01, 100000);
	double percentBond;

    fstream fout;
	fout.open("PartB.csv", ios::out);
	fout << "PercentEquity" << "," << "PercentBond" << "," << "Ret" << "," << "Vol" << "\n";
   
	for (double percentEquity = 0.0; percentEquity <= 1.0; percentEquity = percentEquity + 0.01)
	{
		percentBond = 1.0 - percentEquity;
		Portfolio p2 = m2.analyzePortfolio(percentEquity, percentBond);
		fout << p2.PercentEquity << "," << p2.PercentBond << "," << p2.Ret << "," << p2.Vol << "\n";
	}

	fout.close();

	//Part C: Tangency Portfolio
	Market m3(-0.1, 0.09, 0.20, 0.03, 0.02, 0.02, 100000);
	Portfolio p = m3.analyzePortfolio(1.0, 0.0);
	double maxVol = p.Vol;
	cout << "maxVol " << maxVol << "\n";

	fout.open("PartC.csv", ios::out);
	fout << "PercentEquity" << "," << "PercentBond" << "," << "PercentCash" << "," << "Ret" << "," << "Vol" << "\n";
	
	for (double i = 0.0; i <= maxVol; i = i + 0.0025)
	{
		cout << i << "\n";
		Portfolio p3 = m3.findPortfolioForVol(i);
		fout << p3.PercentEquity << "," << p3.PercentBond << "," << p3.PercentCash << "," << p3.Ret << "," << p3.Vol <<"\n";
		cout << p3.PercentEquity << "," << p3.PercentBond << "," << p3.PercentCash << "," << p3.Ret << "," << p3.Vol << "\n";
	}

	fout.close(); 
	

}
