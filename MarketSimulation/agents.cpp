#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>
using namespace std;

#include "agents.h"

double simUniform()
{
	static std::mt19937 mt_rand(12345);
	static std::uniform_real_distribution<double> dist(0, 1);

	return dist(mt_rand);
}

double simStandardNormal()
{
	static std::mt19937 mt_rand(12345);
	static std::normal_distribution<double> dis_normal(0, 1);

	return dis_normal(mt_rand);
}

Agent::Agent(double tradeProb, double tradeScale) {
	TradeProb = tradeProb;
	TradeScale = tradeScale;
}

Dealer::Dealer(double priceScale) {
	PriceScale = priceScale;
}

void Dealer::addAgent(Agent* a) {
	agents.push_back(a);
}

double Dealer::getPrice(double net) {
	return(exp(PriceScale * net));
}

void Dealer::runSimulation(int numSimulations, int numPeriods, vector<Result>& simResults) {
	simResults.clear();
	double price, net;
	int period;
	for (int simNum = 1; simNum <= numSimulations; simNum ++) {
		for (int j = 0; j < agents.size(); j++) {
			agents[j]->reset();	//reset agents
		}
		price = 1.0;
		net = 0.0;
		Result r1(simNum, 0, 1.0);
		simResults.push_back(r1);
		for (period = 1; period <= numPeriods; period++) 
		{
			for (int j = 0; j < agents.size(); j++)
			{
				double u = simUniform();
				if (u < agents[j]->TradeProb) {
					net = net + agents[j]->tick(price);	
				}
			}
			price = getPrice(net);
			Result r2(simNum, period, price);
			simResults.push_back(r2);
		}
	}
}

double ValueAgent::tick(double price) {
	double netToTrade = TradeScale * (-0.5 + (1.0 / (2.0 + log(price))));
	return(netToTrade);
}

double TrendAgent::tick(double price) {
	double netToTrade = TradeScale * (log(price) - log(prevPrice));
	prevPrice = price;
	return(netToTrade);
}

void TrendAgent::reset() {
	Agent::reset();
	prevPrice = 1;
}

double NoiseAgent::tick(double price) {
	double netToTrade = simStandardNormal()*TradeScale;
	return(netToTrade);

}
