// Assignment8.cpp : 
// There is no need to update this file.

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>


/// YOU MUST CREATE agents.h



#include "agents.h"
using namespace std;


double normalCDF(double x) // Phi(-infinity, x) aka N(x)
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

double bs_call(double spot, double strike, double sigma, double T, double r) {
	double d1 = (log(spot / strike) + (r + sigma * sigma / 2.0) * T) / sigma / sqrt(T);
	double d2 = d1 - sigma * sqrt(T);
	double v = spot * normalCDF(d1) - strike * exp(-r * T) * normalCDF(d2);
	return v;
}


double bs_put(double spot, double strike, double sigma, double T, double r) {
	double d1 = (log(spot / strike) + (r + sigma * sigma / 2.0) * T) / sigma / sqrt(T);
	double d2 = d1 - sigma * sqrt(T);
	double v = strike * normalCDF(-d2) * exp(-r * T) - spot * normalCDF(-d1);
	return v;
}

class SimulationAnalysis {
public:
	double otmOptionValue(const vector<Result>& results, double strike, int period) {

		int count_results = 0;
		double sum_mark = 0;
		for (auto r : results) {
			if (r.period == period) {
				count_results++;
				if (strike < 1.0) {
					// put
					sum_mark += max(0.0, strike - r.price);
				}
				else {
					// call
					sum_mark += max(0.0, r.price - strike);
				}
			}
		}
		return sum_mark / count_results;
	}

	double calc_price(double strike, double v, double T, double r) {
		double bs_price = 0;
		if (strike < 1.0) {
			bs_price = bs_put(1.0, strike, v, T, r);
		}
		else {
			bs_price = bs_call(1.0, strike, v, T, r);
		}

		return bs_price;

	}
	double findBlackScholesVol(double p, double strike, double T, double r) {

		double minVol = 0.01;
		double maxVol = 5;
		double v = 1;



		double diff = 1;
		double bs_price;
		int count = 0;
		while (abs(diff) > 1e-6 && count < 1000) {
			v = 0.5 * (minVol + maxVol);
			bs_price = calc_price(strike, v, T, r);

			if (bs_price > p) {
				maxVol = v;
			}
			else {
				minVol = v;
			}
			diff = p - bs_price;
			count++;
		}

		return v;
	}
};




// Run a simulation for a dealer
void runSimulation(Dealer& dealer, int num_sims, const string& outputFileName="") {
	vector<Result> simResults;

	dealer.runSimulation(num_sims, 100, simResults);

	ofstream outFile;
	outFile.open(outputFileName);
	outFile << "sim" ;
	for (int p = 0; p <= 100; p++) {
		outFile << ",p" << p;
	}
	outFile.precision(5);

	int prevSim = -1;
	for (Result r : simResults) {
		if (r.sim != prevSim) {
			outFile << endl;
			outFile << r.sim;
		}
		outFile  << ',' << r.price ;
		prevSim = r.sim;
	}
	outFile << endl;

	outFile.close();

}


class VolCurve {
public:
	double T;
	double Price;
	double Vol;
};

vector<VolCurve> runVolSurface(Dealer& dealer, int num_sims) {
	vector<Result> simResults;

	dealer.runSimulation(num_sims, 100, simResults);
	vector <VolCurve> curve;

	double strike = 1.0;
	SimulationAnalysis sim_anls;
	for (int p = 10; p <= 100; p += 10) {
		double opt_val = sim_anls.otmOptionValue(simResults, strike, p);
		double vol = sim_anls.findBlackScholesVol(opt_val, strike, p / 100.0, 0.0);
		VolCurve vc = { p / 100.0, opt_val, vol };
		curve.push_back(vc);
	}

	return curve;

}



int main()
{
	auto ms_start = std::chrono::system_clock::now();

	default_random_engine generator = default_random_engine((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	auto dist = normal_distribution<double>(0, 1);
	auto u_dist = uniform_real_distribution<double>(0.1, 0.5);

	double noiseTradeScale = 0.2 / sqrt(100.0) ;

	vector<Agent*> noiseAgents;
	for (int i = 0; i < 1; i++) {
		Agent* a = new NoiseAgent(noiseTradeScale);
		noiseAgents.push_back(a);
	}


	Dealer dealer(1.0);
	for (Agent* a : noiseAgents) {
		dealer.addAgent(a);
	}

	// run the simulation with just a noise trader
	cout << "Running noise" << endl;
	runSimulation(dealer, 200, "sim_just_noise.csv");



	double valueTradeScale = 0.02 / sqrt(75.0);
	vector<Agent*> valueAgents;
	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		
		Agent* a = new ValueAgent(trade_prob, valueTradeScale);

		valueAgents.push_back(a);
	}

	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new ValueAgent(trade_prob, valueTradeScale*2.0);
		valueAgents.push_back(a);
	}

	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new ValueAgent(trade_prob, valueTradeScale*0.5);
		valueAgents.push_back(a);
	}


	Dealer value_dealer(1.0);
	for (Agent* a : noiseAgents) {
		value_dealer.addAgent(a);
	}

	for (Agent* a : valueAgents) {
		value_dealer.addAgent(a);
	}
	
	cout << "Running value" << endl;
	runSimulation(value_dealer, 200, "sim_noise_and_value.csv");


	double trendTradeScale = 0.25/ sqrt(50.0) ;
	vector<Agent*> trendAgents;


	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new TrendAgent(trade_prob, trendTradeScale);
		trendAgents.push_back(a);
	}

	Dealer trend_dealer(1.0);
	for (Agent* a : noiseAgents) {
		trend_dealer.addAgent(a);
	}

	for (Agent* a : trendAgents) {
		trend_dealer.addAgent(a);
	}
	cout << "Running trend" << endl;
	runSimulation(trend_dealer, 200, "sim_noise_and_trend.csv");


	Dealer all_dealer(1.0);
	for (Agent* a : noiseAgents) {
		all_dealer.addAgent(a);
	}

	for (Agent* a : trendAgents) {
		all_dealer.addAgent(a);
	}

	for (Agent* a : valueAgents) {
		all_dealer.addAgent(a);
	}

	cout << "Running all agents" << endl;
	runSimulation(all_dealer, 200, "sim_all_agents.csv");





	auto vc_jn = runVolSurface(dealer, 10000);
	auto vc_value = runVolSurface(value_dealer, 10000);
	auto vc_trend = runVolSurface(trend_dealer, 10000);
	auto vc_all_agents = runVolSurface(all_dealer, 10000);


	ofstream outFilePrices;
	outFilePrices.open("agent_prices.csv");
	outFilePrices << "T,Noise,Value,Trend,All" << endl;
	for (int off = 0; off < vc_jn.size(); off++) {
		outFilePrices << vc_jn[off].T
			<< "," << vc_jn[off].Price
			<< "," << vc_value[off].Price
			<< "," << vc_trend[off].Price
			<< "," << vc_all_agents[off].Price
			<< endl;
	}
	outFilePrices.close();

	ofstream outFileVols;
	outFileVols.open("agent_implied_vols.csv");
	outFileVols << "T,Noise,Value,Trend,All" << endl;
	for (int off = 0; off < vc_jn.size(); off++) {
		outFileVols << vc_jn[off].T
			<< "," << vc_jn[off].Vol
			<< "," << vc_value[off].Vol
			<< "," << vc_trend[off].Vol
			<< "," << vc_all_agents[off].Vol
			<< endl;
	}
	outFileVols.close();


	// clean up
	for (Agent* a : noiseAgents) {
		delete a;
	}

	for (Agent* a : trendAgents) {
		delete a;
	}

	for (Agent* a : valueAgents) {
		delete a;
	}



	auto ms_end = std::chrono::system_clock::now();

	std::chrono::duration<double> diff = ms_end - ms_start;
	cout << "Elapsed " << diff.count() << endl;
}

