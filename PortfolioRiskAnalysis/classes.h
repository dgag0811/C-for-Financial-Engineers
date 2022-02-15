#include<iostream>
#include<vector>
using namespace std;

#ifndef Portfolio_H
#define Portfolio_H

class Portfolio
{
public:	
	Portfolio(double percentEquity, double percentBond, double ret, double vol);

	double PercentEquity;
	double PercentBond;
	double Ret;
	double Vol;
	double PercentCash;
};

#endif

#ifndef Market_H
#define Market_H

class Market
{
protected:
	double calculateTangent(double percentEquity);

public:	
	Market(double corrEquityBond, double equityReturn, double equityVol, double bondReturn, double bondVol, double riskFreeRate, int numSimulations);

	double CorrEquityBond;
	double EquityReturn;
	double EquityVol;
	double BondReturn;
	double BondVol;
	double RiskFreeRate;
	int NumSimulations;
	std::vector<double> r_bond;
	std::vector<double> r_equity;
	
	Portfolio analyzePortfolio(double percentEquity, double percentBond);
	Portfolio findTangencyPortfolio();
	Portfolio findPortfolioForVol(double vol);
};

#endif

