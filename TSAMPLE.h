#ifndef TSAMPLE_h
#define TSAMPLE_h
//SŁAWA WELESOWI!
#include<iostream>
using namespace std;
struct TSample
{
	static void assign_labels(vector<string> labels){Labels=labels;}
	TSample(vector<double> &signals, vector<double> &response_patterns)
	{
		Signals=signals; Response_patterns=response_patterns;
	}
	void show_signal() const
	{
		cout<<"\n";
		for(unsigned int s=0; s<Signals.size(); s++){cout<<Signals[s]<<" ";}
		cout<<"|\t";
		for(unsigned int w=0; w<Response_patterns.size(); w++){cout<<Response_patterns[w]<<" ";}
	}
	static int label_number(){return Labels.size();}
	static string label(int sig){return Labels[sig];}
	int signal_number()const{return Signals.size();}
	double signal(int s)const{return Signals[s];}
	vector<double>* signals(){return &Signals;}
	///Return number of response patterns signal repr. the sample*******
	int pattern_number()const{return Response_patterns.size();}
	double response_pattern(int w)const{return Response_patterns[w];}
	vector<double>* response_patterns(){return &Response_patterns;}
	private:
	static vector<string> Labels;
	vector<double> Signals;
	vector<double> Response_patterns;
	friend void normalize(vector<TSample> &samples);
};
vector<string> TSample::Labels={""};
void normalize(vector<TSample> &samples)
{
	static bool done=false;
	static vector<double> a;
	static vector<double> b;
	static double signal_norm;
	if(!done)
	{
		for(uint sig=0; sig<samples[0].Signals.size(); sig++)
		{
			double min=0, max=0;
			for(auto &p:samples)
			{
				if(p.Signals[sig]>max){max=p.Signals[sig];}
				if(p.Signals[sig]<min){min=p.Signals[sig];}
			}
			a.push_back(1.0/(max-min));
			b.push_back(-a.back()*min);
		}
		for(auto &p:samples)
		{
			signal_norm=0;
			for(auto &s:p.Signals){signal_norm+=pow(s,2);}
		}
		done=true;
	}
	for(uint sig=0; sig<samples[0].Signals.size(); sig++)
	{
		for(auto &p:samples)
		{
			p.Signals[sig]=b[sig]+a[sig]*p.Signals[sig];
		}
	}
	for(auto &p:samples)
	{
		for(auto &s:p.Signals){s/=sqrt(signal_norm);}
	}
}
#endif
