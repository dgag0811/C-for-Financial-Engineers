#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

#include "agents.h"
using namespace std;

double simStandardNormal()
{
	static std::mt19937 mt_rand(1234500);
	static std::normal_distribution<double> dis_normal(0, 1);

	return dis_normal(mt_rand);
}

Agent::Agent(double tradeProb, double tradeScale) {
	tradeProb = tradeProb;
	tradeScale = tradeScale;
}

Dealer::Dealer(double priceScale) {
	priceScale = priceScale;
}

void Dealer::addAgent(Agent* a) {
	agents.push_back(a);
}
double Dealer::getPrice(double net) {
	return(exp(priceScale * net));
}

void Dealer::runSimulation(int numSimulations, int numPeriods, vector< Result >& simResults) {
	simResults.clear();
	double price, net;

	for (int simNumber = 1; simNumber <= numSimulations; simNumber++) {
		for (int j = 0; j < sizeof(agents); j++) {
			agents[j]->reset();	//reset agents
		}
		price = 1.0;
		net = 0.0;
		Result r(simNumber, 0, 1.0);
		simResults.push_back(r);

		//Add the starting Result object to simResults
		for (int period = 1; period <= numPeriods; period++) {
			for (int j = 0; j < sizeof(agents); j++) {
				double u = simStandardNormal();
				if (u < agents[j]->tradeProb) {
					net = net + agents[j]->tick(price);
				}
			}
			price = getPrice(net);
			Result r(simNumber, period, price);
			simResults.push_back(r);
		}
	}
}

double ValueAgent::tick(double price) {
	double netToTrade = tradeScale * (-0.5 + 1.0 / (2.0 + log(price)));
	return(netToTrade);
}

double TrendAgent::tick(double price) {
	double netToTrade = tradeScale * log(price) - log(prevPrice);
	prevPrice = price;
	return(netToTrade);
}

void TrendAgent::reset() {
	Agent::reset();
	prevPrice = 1.0;
}

double NoiseAgent::tick(double price) {
	double netToTrade = simStandardNormal() * tradeScale;
	return(netToTrade);
}
