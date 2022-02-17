#include<iostream>
#include<vector>
using namespace std;

#ifndef Agent_H
#define Agent_H
class Agent {
public:
	// the probability the agent trades during a period
	double tradeProb;
	// a scaling factor for the size of the trade
	double tradeScale;
	Agent(double tradeProb, double tradeScale);
	virtual ~Agent() {};
	virtual double tick(double price) = 0;
	virtual void reset() { 
		tradeProb = 0;
		tradeScale = 0;
	};
};
#endif 

#ifndef Result_H
#define Result_H
class Result{
public:
	int sim;
	int period;
	double price;
	Result() {};
	Result(int sim, int period, double price) : sim(sim), period(period), price(price) {};
};
#endif

#ifndef Dealer_H
#define Dealer_H
class Dealer {
private:
	vector<Agent* > agents;
	double priceScale;
public:	
	Dealer(double priceScale);
	virtual ~Dealer() {};
	void addAgent(Agent* a);
	void runSimulation(int numSimulations, int numPeriods, vector< Result >& simResults);
	double getPrice(double net);
};
#endif

#ifndef ValueAgent_H
#define ValueAgent_H
class ValueAgent : public Agent {
public:
	ValueAgent(double tradeProb, double tradeScale) : Agent(tradeProb, tradeScale) {};
	double tick(double price);
};
#endif

#ifndef TrendAgent_H
#define TrendAgent_H
class TrendAgent : public Agent {
protected:
	double prevPrice = 1.0;
public:
	TrendAgent(double tradeProb, double tradeScale) : Agent(tradeProb, tradeScale) {};
	double tick(double price);
	void reset();
};
#endif

#ifndef NoiseAgent_H
#define NoiseAgent_H
class NoiseAgent : public Agent {
public:
	NoiseAgent(double tradeScale) : Agent(1.0, tradeScale) {}
	double tick(double price);
};

#endif

