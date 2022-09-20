//SŁAWA WELESOWI!
#include<iostream>
#include<fstream>
#include<iomanip>
#include<string>
#include <vector>
#include <algorithm>
#include <random>
#include "/home/przemek/MojeProgramy/BIBLIOTEKI2/MAT.h"
#include "/home/przemek/MojeProgramy/MÓZG_ELEKTRONOWY/4Git/TSAMPLE.h"
#include "/home/przemek/MojeProgramy/MÓZG_ELEKTRONOWY/4Git/TNET.h"
using namespace std;
void read_samples(string infile, vector<TSample> &Samples, bool normalization);
void configuration(bool &bias, bool &normalization, int &learn_cycles, string &learn_mode, string &outfile, string &net_name, 
                  TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes);
void net_learning(string learn_mode, int learn_cycles, bool normalization, TNet &Brain, vector<TSample> Samples);
void analysis(TNet Brain, vector<TSample> Samples, vector<TSample> Analytes, string outfile);
/////////////////////////////////////////////////////////////////////////
int main()
{
srand( time( NULL ) );
vector<TSample> Samples;
vector<TSample> Analytes;
bool bias=false;
bool normalization=true;
int learn_cycles;
string learn_mode;
string outfile;
string net_name;
TNet Brain;
configuration(bias, normalization, learn_cycles, learn_mode, outfile, net_name, Brain, Samples, Analytes);
net_learning(learn_mode, learn_cycles, normalization, Brain, Samples);
//Brain.save_net(net_name);
Brain.show_weights();
Brain.free_filters();
analysis(Brain, Samples, Analytes, outfile);
cout<<
"\nFunkcja normalizująca próbki powinno równocześnie normalizować"
"\nprogi filtrów!";

////////////////////////////////////////////////////////////////////////
}
void read_samples(string infile, vector<TSample> &Samples, bool normalization)
{
	ifstream DATA{infile};
	int types_number;
	vector<string> labels;
	vector<double> signals;
	vector<double> response_patterns;
	string dat;
	do{
		DATA>>dat;
		labels.push_back(dat);
	}while(dat!="EoE");
	labels.pop_back();
	for(auto e:labels)
	{
		//cout<<"\n"<<e;
	}
	double s;
	for(unsigned int i=0; i<labels.size(); i++)
	{
		DATA>>s;
		signals.push_back(s);
	}
	DATA>>s; response_patterns.push_back(s);
	Samples.push_back(TSample(labels, signals, response_patterns));
	double w=Samples.back().response_pattern(0);
	while(s!=-666)
	{
		signals.clear(); 
		response_patterns.clear();
		for(unsigned int sig=0; sig<labels.size(); sig++)
		{
			DATA>>s; if(s==-666){break;} signals.push_back(s);
		}
		DATA>>s; response_patterns.push_back(s);
		Samples.push_back(TSample(labels, signals, response_patterns));
		if(w!=Samples.back().response_pattern(0)){w=Samples.back().response_pattern(0); types_number++;}
	}
	Samples.pop_back();
	DATA.close();
	for(auto &p:Samples)
	{
		p.show_signal();
	}
	cout<<"\nSample types_number "<<types_number;
}
void configuration(bool &bias, bool &normalization, int &learn_cycles, string &learn_mode, string &outfile, string &net_name,
                  TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes)
{
	string net_architecture;
	string neuron_type;
	string sample_file;
	int input_number;
	int neuron_number;
	int layer_number;

	vector<TFilter_high> Filtry_progu;
	string dat;
	ifstream CONF{"CONF"};
	while(dat!="EOF")
	{
		CONF>>dat;
		if(dat=="sample"){CONF>>sample_file; read_samples(sample_file, Samples, normalization); input_number=Samples[0].signal_number();}
		if(dat=="analys"){CONF>>sample_file; read_samples(sample_file, Analytes, normalization);}
		if(dat=="LAY"){CONF>>layer_number;}
		if(dat=="NEU"){CONF>>neuron_number;}
		if(dat=="bias"){bias=true;}
		if(dat=="nnor"){normalization=false;}
		if(dat=="struct")
		{
			CONF>>net_architecture;
			if(net_architecture=="manual")
			{
				string istr;
				for(int lay=0; lay<layer_number; lay++)
				{
					if(lay>0){input_number=Brain.neuron_number(lay-1);}
					
					vector<TNeuron> Neurons;
					bias=false;
					CONF>>istr;
					do{
						if(istr=="bias"){bias=true;}
						else{Neurons.push_back(TNeuron(input_number, istr, bias));}
						CONF>>istr;
						
					}while(istr!="eoN");
					Brain+=TLayer(Neurons);
					Neurons.clear();
				}
			}
			if(net_architecture!="manual")
			{
				Brain=TNet(layer_number, neuron_number, input_number, 
				net_architecture, neuron_type, bias);
			}
		}
		if(dat=="active"){CONF>>neuron_type;}
		if(dat=="learn"){CONF>>learn_mode;}
		if(dat=="filter_yes"||dat=="filter_no")
		{
			bool yes=true;
			if(dat=="filter_no"){yes=false;}
			int layer;
			CONF>>layer;
			while(layer>=0)
			{
				int neuron;
				CONF>>neuron;
				string label;
				vector<uint> channels;
				CONF>>label;
				while(label!="eoF")
				{
					///sprawdzenie któremu kanałowi odpowiada label
					for(int chan=0; chan<Samples[0].signal_number(); chan++)
					{
						if(label==Samples[0].label(chan))
						{
							channels.push_back(chan);
							break;
						}
					}
					CONF>>label;
				}
				Brain.assign_filter(layer, neuron, TFilter_yes(channels, yes));
				CONF>>layer;
			}
		}
		if(dat=="filtr_gorny"||dat=="filtr_dolny")
		{
			bool high_pass=true;
			if(dat=="filtr_dolny"){high_pass=false;}
			int layer;
			CONF>>layer;
			while(layer>=0)
			{
				int neuron;
				CONF>>neuron;
				string label;
				vector<uint> channels;
				vector<double> thresholds;
				
				CONF>>label;
				while(label!="eoF")
				{
					double prog;
					CONF>>prog;
					thresholds.push_back(prog);
					///sprawdzenie któremu kanałowi odpowiada label
					for(int chan=0; chan<Samples[0].signal_number(); chan++)
					{
						if(label==Samples[0].label(chan))
						{
							channels.push_back(chan);
							break;
						}
					}
					CONF>>label;
				}
				Filtry_progu.push_back(TFilter_high(channels, thresholds, high_pass));
				CONF>>layer;
			}
		}
		if(dat=="save"){CONF>>outfile;}
		if(dat=="savenet"){CONF>>net_name;}
		if(dat=="cycle"){CONF>>learn_cycles;}
	}
	CONF.close();
	if(normalization){normalize(Samples); normalize(Analytes);}	
	cout<<"SAMPLES "<<Samples.size()<<"\tSIGNALS "<<Samples[0].signal_number()<<fixed<<setprecision(3)<<showpos;
	
	//if(Brain.output_number()<types_number)
	//{
		//cout<<"\nSieć ma mniej WYJŚĆ niż yes typów w zbiorze danych. Czy "
		//"kontynuować pracę? 1 - tak, 0 - nie\n";
		//bool praca;
		//cin>>praca;
		//if(praca==0){return 0;}
	//}
}
void net_learning(string learn_mode, int learn_cycles, bool normalization, TNet &Brain, vector<TSample> Samples)
{
	double learn_rate=0.6;
	bool forced=false;
	if(learn_mode=="swob_fors")
	{
		forced=true;
		learn_mode="unsu";
		if(Brain.output_number()<Samples[0].pattern_number())
		{
			cout<<"\nLess net outputs than response patterns!"
			"\nContinue?"
			"\n1 - yes"
			"\n0 - no";
			bool con;
			cin>>con;
			if(!con){return;}
		}
	}
	//if(learn_mode=="swob_grup")
	//{
		//cout<<"\nPerceptron";
		//int type=Samples.back().response_pattern(0);
		//int neuron=0;
		////vector<TSample> podprobki_tmp;
		//vector<vector<TSample>> podprobki;
		
		//podprobki.push_back(vector<TSample>());
		//for(unsigned int pro=0; pro<Samples.size(); pro++)
		//{
			//if(Samples[pro].response_pattern(0)!=type){type=Samples[pro].response_pattern(0); neuron++; podprobki.push_back(vector<TSample>());}
			
			//podprobki[neuron-1].push_back(Samples[pro]);
		//}
		//for(int cycle=0; cycle<learn_cycles; cycle++)
		//{
			//for(int neu=0; neu<Brain.output_number(); neu++)
			//{
				//learn_rate=0.3*(1.0-1.0*cycle/learn_cycles);
				//for(unsigned int sample=0; sample<podprobki[neu].size(); sample++)
				//{
					////cout<<"\nneu "<<neu;
					//Brain.unsupervised_learning(neu, *podprobki[neu][sample].signals(),  learn_rate, stalosc_wag, normalization);
					////Brain.show_weights();cout<<"\t"<<cycle<<"\t"<<learn_rate;
				//}
				//random_shuffle(podprobki[neu].begin(), podprobki[neu].end());
			//}
		//}
	//}
	if(learn_mode=="unsu")
	{
		for(int cycle=0; cycle<learn_cycles; cycle++)
		{
			vector<double> weights;
			//learn_rate=.3;
			learn_rate=0.3*(1.0-1.0*cycle/learn_cycles);
			//learn_rate=1-3.9*pow(learn_rate-0.5,2);
			//cout<<"\n"<<cycle<<"\t"<<learn_rate;
			for(unsigned int sample=0; sample<Samples.size(); sample++)
			{
				Brain.unsupervised_learning(Samples[sample],  learn_rate, weights, normalization, forced);
			}
			//for(auto &w:weights){cout<<w<<"\t";}
			random_shuffle(Samples.begin(), Samples.end());
		}
	}
	
	//if(learn_mode=="swob_kon")
	//{
		//for(int cycle=0; cycle<learn_cycles; cycle++)
		//{
			//learn_rate=0.3*(1.0-1.0*cycle/learn_cycles);
			//for(unsigned int sample=0; sample<Samples.size(); sample++)
			//{
				//Brain.uczenie_nienadzorowane_z_konkurencja(*Samples[sample].signals(),  learn_rate, normalization);
				////Brain.show_weights();cout<<"\t"<<cycle<<"\t"<<learn_rate;
			//}
			//random_shuffle(Samples.begin(), Samples.end());
		//}
	//}
}
void analysis(TNet Brain, vector<TSample> Samples, vector<TSample> Analytes, string outfile)
{
	//Obliczenie responses na wszystkie próbki
	for(auto &a:Analytes)
	{
		Samples.push_back(a);
	}
	vector<vector<double>> Responses;
	for(uint pro=0; pro<Samples.size(); pro++)
	{
		Responses.push_back(Brain.responses(Samples[pro]));
	}
	Responses.shrink_to_fit();
	//Nagłówek prototypów
	///Znalezienie najdłuższego stringu
	uint m=0;
	for(int label=0; label<Samples[0].signal_number(); label++)
	{
		if(Samples[0].label(label).length()>m){m=Samples[0].label(label).length();}
	}
	///Otwarcie pliku
	ofstream RES{outfile+".txt"};
	RES<<fixed<<setprecision(3);
	for(int lay=0; lay<Brain.layer_number(); lay++)
	{
		RES<<setw(m)<<" ";
		for(int out=0; out<Brain.output_number(lay); out++)
		{
			RES<<"\tN"<<out<<"/"<<lay;
		}
		RES<<"\n";//<<showpos;
		int input_number;
		{input_number=Brain.input_number(lay);}
		
		for(int inp=0; inp<input_number; inp++)
		{
			if(lay==0)
			{
				if(inp<Samples[0].signal_number()){RES<<setw(m)<<Samples[0].label(inp);}
				if(inp==Samples[0].signal_number()){RES<<setw(m)<<"BIAS";}
			}
			if(lay>=1)
			{
				if(inp<Brain.neuron_number(lay-1)){RES<<setw(m)<<"N"<<inp<<"/"<<lay-1;}
				if(inp==Brain.neuron_number(lay-1)){RES<<setw(m)<<"BIAS";}
				
			}
			for(int neu=0; neu<Brain.output_number(lay); neu++)
			{
				RES<<"\t"<<Brain.weight_n(lay, neu, inp);
			}
			RES<<"\n";
		}
		RES<<"\n";
	}
	//Raport z analizy próbek
	RES<<"\nLABEL\tGROUP";
	for(int i=0; i<Samples[0].signal_number(); i++)
	{
		RES<<"\t"<<Samples[0].label(i).substr(0,7);
	}
	for(int i=0; i<Brain.output_number(); i++)
	{
		RES<<"\tRES_"<<noshowpos<<i;
	}
	//RES<<showpos;
	for(uint sample=0; sample<Samples.size(); sample++)
	{
		//if(sample>0)
		//{if(Samples[sample].response_pattern(0)!=Samples[sample-1].response_pattern(0)){RES<<"\n";}}
		vector<double>Response_patterns=*Samples[sample].response_patterns();
		RES<<"\n";
		for(auto w:Response_patterns)
		{
			RES<<w<<"\t";
		}
		double RESP=0;
		int m;
		for(uint resp=0; resp<Responses[sample].size(); resp++)
		{
			if((Responses[sample][resp])>RESP){RESP=Responses[sample][resp]; m=resp;}
		}
		RES<<"PRO_"<<noshowpos<<m<<"\t";
		for(int s=0; s<Samples[sample].signal_number(); s++)
		{
			RES<<Samples[sample].sygnal(s);
			if(s<Samples[sample].signal_number()-1){RES<<"\t";}
		}
		for(uint resp=0; resp<Responses[sample].size(); resp++)
		{
			RES<<"\t"<<Responses[sample][resp];
		}
	}
	RES.close();	
}
