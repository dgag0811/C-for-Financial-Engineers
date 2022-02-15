#include <iostream>
#include <vector>


#ifndef Option_H
#define Option_H


class Option 
{
protected:
	double Strike;
	double Sigma;
	double T;
	double RiskFreeRate;
public:
	Option(double strike, double sigma, double t, double riskFreeRate);
	virtual double getValue(double spot) = 0;
	virtual double getIntrinsicValue(double spot) = 0;
	virtual double calcBlackScholesValue(double spot) = 0;
	double getDelta(double spot);
};

#endif

#ifndef EuropeanOption_H
#define EuropeanOption_H

class EuropeanOption : public Option
{
public:
	EuropeanOption(double strike, double sigma, double t, double riskFreeRate);
	double getValue(double spot);
};

#endif

#ifndef EuropeanCall_H
#define EuropeanCall_H

class EuropeanCall : public EuropeanOption
{
public:
	EuropeanCall(double strike, double sigma, double t, double riskFreeRate);
	double getIntrinsicValue(double spot);
	double calcBlackScholesValue(double spot);
};

#endif

#ifndef EuropeanPut_H
#define EuropeanPut_H

class EuropeanPut : public EuropeanOption
{
public:
	EuropeanPut(double strike, double sigma, double t, double riskFreeRate);
	double getIntrinsicValue(double spot);
	double calcBlackScholesValue(double spot);
};

#endif

#ifndef Position_H
#define Position_H

class Position 
{
public:
	double weight;
	Option* option;
	Position();
	Position(double w, Option* o);
};

#endif

#ifndef Portfolio_H
#define Portfolio_H

class Portfolio 
{
protected:
	std::vector<Position> Positions;
	std::vector<double> vals;
	
public:
	Portfolio();
	void addPosition(const Position& pos);
	double getValue(double spot);
	double getDelta(double spot);
	double calcDailyVaR(double spot, double sigma, double riskFreeRate, int N);
	double calcDailyExpectedShortfall(double spot, double sigma, double riskFreeRate, int N);




};

#endif




