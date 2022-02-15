#include "options.h"
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>

using namespace std;

double normalCDF(double x) // Phi(-infinity, x) aka N(x)
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

double simStandardNormal() 
{
	static std::mt19937 mt_rand(12345);
	static std::normal_distribution<double> dis_normal(0.0, 1.0);

	return dis_normal(mt_rand);
}

Option::Option(double strike, double sigma, double t, double riskFreeRate)
{
	Strike = strike;
	Sigma = sigma;
	T = t;
	RiskFreeRate = riskFreeRate;
}

double Option::getDelta(double spot)
{
	double new_spot;
	double new_OptionValue;
	double old_OptionValue;
	new_spot = spot * 1.01;
	old_OptionValue = getValue(spot);
	new_OptionValue = getValue(new_spot);

	return(new_OptionValue - old_OptionValue);
}

EuropeanOption::EuropeanOption(double strike, double sigma, double t, double riskFreeRate): Option(strike, sigma, t, riskFreeRate)
{
	Strike = strike;
	Sigma = sigma;
	T = t;
	RiskFreeRate = riskFreeRate;
}

double EuropeanOption::getValue(double spot)
{
	return(calcBlackScholesValue(spot));
}

EuropeanCall::EuropeanCall(double strike, double sigma, double t, double riskFreeRate) : EuropeanOption(strike, sigma, t, riskFreeRate)
{
	Strike = strike;
	Sigma = sigma;
	T = t;
	RiskFreeRate = riskFreeRate;
}

double EuropeanCall::getIntrinsicValue(double spot)
{
	return(max(spot - Strike, double(0))); 
}

double EuropeanCall::calcBlackScholesValue(double spot)
{
	double d1, d2, K = Strike, r = RiskFreeRate;
	d1 = (std::log(spot / K) + (r + Sigma * Sigma * 0.5) * T) / (Sigma * std::sqrt(T));
	d2 = d1 - Sigma * std::sqrt(T);

	return(spot * normalCDF(d1) - K * std::exp(-1 * r * T) * normalCDF(d2));
}

EuropeanPut::EuropeanPut(double strike, double sigma, double t, double riskFreeRate) : EuropeanOption(strike, sigma, t, riskFreeRate)
{
	Strike = strike;
	Sigma = sigma;
	T = t;
	RiskFreeRate = riskFreeRate;
}

double EuropeanPut::getIntrinsicValue(double spot)
{
	return(max(Strike-spot, double(0)));
}

double EuropeanPut::calcBlackScholesValue(double spot) {
	double d1, d2, K = Strike, r = RiskFreeRate;
	d1 = (log(spot / K) + (r + Sigma * Sigma * 0.5) * T) / (Sigma * sqrt(T));
	d2 = d1 - Sigma * sqrt(T);

	return(normalCDF(-1 * d2) * K * exp(-1 * r * T) - normalCDF(-1 * d1) * spot);
}

Position::Position()
{
	weight = 0;
	option = NULL;
}

Position::Position(double w, Option* o)
{
	weight = w;
	option = o;
}


Portfolio::Portfolio()
{
	Positions.clear();
	vals.clear();
}

void Portfolio::addPosition(const Position& pos)
{
	Positions.push_back(pos);
}

double Portfolio::getValue(double spot)
{
	double sum=0; 
	for (Position pos : Positions) {
		sum += pos.weight * pos.option->getValue(spot);
	}
	return(sum);

// positions = [ [w0, f0], [w1, f1],...]]
}

double Portfolio::getDelta(double spot)
{
	double sum = 0;
	for (Position pos : Positions) {
		sum += pos.weight * pos.option->getDelta(spot);
	}
	return(sum);
}

double Portfolio::calcDailyVaR(double spot, double sigma, double riskFreeRate, int N)
{
	double VaR, dt, vol, drift, spot_new, portfolio_new;
	
	vals.clear();
	dt = 1.0 / 252.0;
	vol = sqrt(dt) * sigma;
	drift = dt * (riskFreeRate - 0.5 * sigma * sigma);
	for (int i = 0; i < N; i++)
	{
		spot_new = spot * exp(drift + vol * simStandardNormal());
		portfolio_new = getValue(spot_new);
		vals.push_back(portfolio_new);
	}

	sort(vals.begin(), vals.end());

	VaR = getValue(spot) - vals[0.05 * N - 1];
	return(VaR);
}

double Portfolio::calcDailyExpectedShortfall(double spot, double sigma, double riskFreeRate, int N)
{
	double expectedShortFall, Sum=0.0;
	double VaR, dt, vol, drift, spot_new, portfolio_new;

	vals.clear();
	dt = 1.0 / 252.0;
	vol = sqrt(dt) * sigma;
	drift = dt * (riskFreeRate - 0.5 * sigma * sigma);
	for (int i = 0; i < N; i++)
	{
		spot_new = spot * exp(drift + vol * simStandardNormal());
		portfolio_new = getValue(spot_new);
		vals.push_back(portfolio_new);
	}

	sort(vals.begin(), vals.end());


		// for(double x: vals){
		// 	cout<<x<<"\n";	
		// }
			
	for (int i = 0; i <= 0.05 * N - 1; i++)
	{
		Sum += vals[i];
	}
	
	expectedShortFall = getValue(spot) - (Sum)/(N*0.05);
	return(expectedShortFall);
}
