#include "classes.h"
#include<iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>
#include<vector>
using namespace std;

double normalCDF(double x) 
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

double simStandardNormal()
{
	static std::mt19937 mt_rand(1234500);
	static std::normal_distribution<double> dis_normal(0,1);

	return dis_normal(mt_rand);
}

Portfolio::Portfolio(double percentEquity, double percentBond, double ret, double vol )
{
	PercentEquity = percentEquity;
	PercentBond = percentBond;
	Ret = ret;
	Vol = vol;
	PercentCash = 1.0 - PercentEquity - PercentBond;
}

Market::Market(double corrEquityBond, double equityReturn, double equityVol, double bondReturn, double bondVol, double riskFreeRate, int numSimulations )
{
	CorrEquityBond = corrEquityBond;
	EquityReturn = equityReturn;
	EquityVol = equityVol;
	BondReturn = bondReturn;
	BondVol = bondVol;
	RiskFreeRate = riskFreeRate;
	NumSimulations = numSimulations;

	double Z_equity, Z_bond;
	r_bond.clear();
	r_equity.clear();
	
	for( int j=0;j <NumSimulations; j++)
	{
		//double Z_equity = simStandardNormal();
		Z_equity = simStandardNormal();
		Z_bond = corrEquityBond * Z_equity + sqrt(1.0 - corrEquityBond * corrEquityBond) * simStandardNormal();
		//double Z_bond = corrEquityBond * Z_equity + sqrt(1.0 - corrEquityBond * corrEquityBond) * simStandardNormal();
		r_equity.push_back((equityReturn - (equityVol * equityVol * 0.5)) + equityVol * Z_equity);
		r_bond.push_back((bondReturn - (bondVol * bondVol * 0.5)) + bondVol * Z_bond);
	}
}

Portfolio Market::analyzePortfolio(double percentEquity, double percentBond)
{
	double sum = 0.0;
	std::vector<double> ret;

	for (int i = 0; i < NumSimulations; i++){
		ret.push_back( - 1.0 + percentEquity * exp(r_equity[i]) + percentBond * exp(r_bond[i]) + (1.0 - percentEquity - percentBond) * exp(RiskFreeRate) );
	}

	for (int i = 0; i < NumSimulations; i++){
		sum += ret[i];
	}

	double mean = sum/NumSimulations;
	sum = 0.0;
	for (int i = 0; i < NumSimulations; i++){
		sum += ( ret[i] - mean ) * (ret[i] - mean);
	}

	double vol = sqrt(sum / (NumSimulations-1)); 
	Portfolio p(percentEquity, percentBond, mean, vol);
	return(p);
}

double Market::calculateTangent(double percentEquity)
{
	Portfolio Up=analyzePortfolio(percentEquity + 0.01, 0.99 - percentEquity);
	Portfolio Down=analyzePortfolio(percentEquity - 0.01, 1.01 - percentEquity);
	return((Up.Ret - Down.Ret) / (Up.Vol - Down.Vol));
}	

Portfolio Market::findTangencyPortfolio()
{
	double maxSlope = 0.0, calcSlope, percentEquity=0.01;
	for (double k = 0.01; k <= 0.99; k = k + 0.0025)
	{
		calcSlope = calculateTangent(k);
		if (calcSlope > maxSlope)
		{
			maxSlope = calcSlope;
			percentEquity = k;
		}
	}

	double percentBond = 1.0 - percentEquity;
	Portfolio pNew = analyzePortfolio(percentEquity, 1.0 - percentEquity);
	
	return(pNew);
}

#define EPSILON 0.01
Portfolio Market::findPortfolioForVol(double vol)
{
	Portfolio p = findTangencyPortfolio();
	double percentEquity, percentBond;
	if (vol > p.Vol)
	{
		double upper = 1.0, lower = p.PercentEquity;
		while (upper - lower >= EPSILON)
		{
			Portfolio pTemp = analyzePortfolio((upper + lower) *0.5, 1.0 - (upper + lower)*0.5);
			if (pTemp.Vol == vol)
				break;
			else
			{
				if (pTemp.PercentEquity > (upper + lower) / 2.0)
					lower = pTemp.PercentEquity;
				else
					upper = pTemp.PercentEquity;
			}
		}

		percentEquity = (upper + lower) / 2.0;
		percentBond = 1.0 - percentEquity;

	}
	else if(vol < p.Vol)
	{
		double vol_tp = p.Vol;
		vol = (1.0 - p.PercentCash)* vol_tp;
		percentEquity = (1.0 - p.PercentCash) * p.PercentEquity;
		percentBond = (1.0 - p.PercentCash) * p.PercentBond;

	}
	else
	{
		percentBond = p.PercentBond;
		percentEquity = p.PercentEquity;
	}

	Portfolio pNew = analyzePortfolio(percentEquity, percentBond);
	return(pNew);
}
