#ifndef TSAMPLE_h
#define TSAMPLE_h
//SŁAWA WELESOWI!
#include<iostream>
#include<vector>
#include<math.h>
using namespace std;
struct TSample
{
	static void assign_labels(vector<string> labels){
		Labels=labels;
	}
	TSample(vector<double> &signals, vector<double> &response_patterns){
		Signals=signals; Response_patterns=response_patterns;
	}
	void show_signal() const;
	static int label_number(){
		return Labels.size();
	}
	static string label(int sig){
		return Labels[sig];
	}
	int signal_number()const{
		return Signals.size();
	}
	double signal(int s)const{
		return Signals[s];
	}
	vector<double>* signals(){
		return &Signals;
	}
	int pattern_number()const{
		return Response_patterns.size();}
	double response_pattern(int w)const{
		return Response_patterns[w];
	}
	vector<double>* response_patterns(){
		return &Response_patterns;
	}
	private:
	static vector<string> Labels;
	vector<double> Signals;
	vector<double> Response_patterns;
	friend void normalize(vector<TSample> &samples);
};
#endif
