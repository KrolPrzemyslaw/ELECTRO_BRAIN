//SŁAWA WELESOWI!
#include<iostream>
#include<fstream>
#include<iomanip>
#include<string>
#include<vector>
#include<algorithm>
#include<random>
#include"TSAMPLE.h"
#include"TNET.h"
#include"TCONF.h"
#include"ELECTRO_BRAIN.h"
using namespace std;
void read_samples(string infile, vector<TSample> &Samples);
void configuration(TConfiguration_input &Conf, TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes);
void net_learning(const TConfiguration_input &Conf, TNet &Brain, vector<TSample> Samples);
void analysis(TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes, string outfile);
void check_major_exceptions(const vector<TSample> &Samples, const vector<TSample> &Analytes, const TNet &Brain);
int main(){

	cout<<"\nWelcome in ELECATRO_BRAIN, version "<<ELECTRO_BRAIN_VERSION_MAJOR<<"."<<ELECTRO_BRAIN_VERSION_MINOR<<
	"\nCompiled in cpp standard "<<__cplusplus;
	//Basic data declaration
	TConfiguration_input Conf;
	vector<TSample> Samples;
	vector<TSample> Analytes;
	TNet Brain;
	//Data reading and validity control
	configuration(Conf, Brain, Samples, Analytes);
	check_major_exceptions(Samples, Analytes, Brain);
	//Net learning
	net_learning(Conf, Brain, Samples);
	Brain.save_net();
	//Analysis
	Brain.free_filters();
	analysis(Brain, Samples, Analytes, Conf.output_file_name());
	cout<<"\nELECTRO_BRAIN - job finished\n";
}
void read_samples(string infile, vector<TSample> &Samples){
	//Read labels
	ifstream DATA{infile};{
		vector<string> labels;
		string dat;
		while(DATA>>dat&&dat!="EoE")
		{
			labels.push_back(dat);
		}
		TSample::assign_labels(labels);
	}
	//Read samples
	vector<double> signals;
	vector<double> response_patterns;
	double s=0;
	while(s!=-666){
		signals.clear(); 
		response_patterns.clear();
		for(int sig=0; sig<TSample::label_number(); sig++){
			DATA>>s; 
			signals.push_back(s);
		}
		DATA>>s; 
		response_patterns.push_back(s);
		Samples.push_back(TSample(signals, response_patterns));
	}
	Samples.pop_back();
	DATA.close();
}
bool filter_is_defined_in(const string &dat){
	bool filter=false;
	if(dat.find("filter")<dat.length()){
		filter=true;
	}
	return filter;
}
void check_input_file_existence(const ifstream &CONF, const string &dat){
	try{if(!CONF){
		throw runtime_error("Configuration file "+dat+" not found");}
	}
	catch(exception &ex){
		cout<<endl<<ex.what();exit(-1);
	}
}
void configuration(TConfiguration_input &Conf, TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes){
	bool bias=false;
	string net_architecture;
	string neuron_type;
	string sample_file;
	int input_number;
	int neuron_number;
	int layer_number;
	vector<TFilter_high> Filters_trshd;
	string dat="CONF";
	cout<<"\nConfiguration input file: "<<dat;
	ifstream CONF{dat};
	check_input_file_existence(CONF, dat);
	while(dat!="EOF"){
		CONF>>dat;
		if(dat=="sample"){
			CONF>>sample_file; 
			cout<<"\n\tReading samples for learning from file: "<<sample_file;
			read_samples(sample_file, Samples); 
			input_number=Samples[0].signal_number();
		}
		if(dat=="analys"){
			CONF>>sample_file; 
			cout<<"\n\tReading samples for analysis from file: "<<sample_file;
			read_samples(sample_file, Analytes);
		}
		if(dat=="LAY"){
			CONF>>layer_number;
		}
		if(dat=="NEU"){
			CONF>>neuron_number;
		}
		if(dat=="bias"){
			bias=true;
		}
		if(dat=="nnor"){
			Conf.decide_on_normalization(false);
		}
		if(dat=="struct"){
			string net_name;
			CONF>>net_architecture;
			if(net_architecture=="read_from_file"){
				string saved_net;
				CONF>>saved_net;
				Brain=TNet(saved_net);
			}
			if(net_architecture=="manual"){
				CONF>>net_name;
				Brain.assign_name(net_name);
				string response_function;
				for(int lay=0; lay<layer_number; lay++)
				{
					if(lay>0){input_number=Brain.neuron_number(lay-1);}
					
					vector<TNeuron> Neurons;
					bias=false;
					CONF>>response_function;
					do{
						if(response_function=="bias"){bias=true;}
						else{Neurons.push_back(TNeuron(input_number, response_function, bias));}
						CONF>>response_function;
						
					}while(response_function!="eoN");
					Brain+=TLayer(Neurons);
					Neurons.clear();
				}
			}
			if(net_architecture!="manual"&&net_architecture!="read_from_file"){
				Brain=TNet(net_name, layer_number, neuron_number, input_number, 
				net_architecture, neuron_type, bias);
			}
		}
		if(dat=="active"){
			CONF>>neuron_type;
		}
		if(dat=="learn"){
			string learn_mode;
			CONF>>learn_mode;
			Conf.assign_learn_mode(learn_mode);
		}
		if(dat=="cycle"){
			int learn_cycles;
			CONF>>learn_cycles;
			Conf.assign_learn_cycles(learn_cycles);
		}
		if(filter_is_defined_in(dat)){
			//Brain existence control
			try{if(Brain.is_empty()){throw runtime_error("Brain lobotomized - nowhere to assign filter!");}}
			catch(exception &ex){cout<<endl<<ex.what();exit(-1);}
			
			bool yes=true;
			if(dat=="filter_no"){
				yes=false;
			}
			int layer;
			CONF>>layer;
			while(layer>=0){
				int neuron;
				CONF>>neuron;
				string label;
				vector<uint> channels;
				CONF>>label;
				while(label!="eoF"){
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
		if(dat=="save"){
			string outfile;
			CONF>>outfile;
			Conf.assign_output_file_name(outfile);
		}
	}
	CONF.close();
	if(Conf.normalization()){
		normalize(Samples); 
		normalize(Analytes);
	}	
	cout<<"\n\tSAMPLES "<<Samples.size()<<"\tANALYTES "<<Analytes.size()<<"\tSIGNALS "<<Samples[0].signal_number();
}
void net_learning(const TConfiguration_input &Conf, TNet &Brain, vector<TSample> Samples)
{
	if(Conf.learn_mode()=="none"){return;}
	double learn_rate=0.6;
	bool forced=false;
	if(Conf.learn_mode()=="swob_fors"){
		forced=true;
		if(Brain.output_number()<Samples[0].pattern_number()){
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
	if(Conf.learn_mode()=="unsu"){
		for(int cycle=0; cycle<Conf.learn_cycles(); cycle++){
			vector<double> weights;
			learn_rate=0.3*(1.0-1.0*cycle/Conf.learn_cycles());
			for(unsigned int sample=0; sample<Samples.size(); sample++){
				Brain.unsupervised_learning(Samples[sample],  learn_rate, weights, Conf.normalization(), forced);
			}
			random_shuffle(Samples.begin(), Samples.end());
		}
	}
}
void analysis(TNet &Brain, vector<TSample> &Samples, vector<TSample> &Analytes, string outfile){
	//Merge Samples and Analytes vectors 
	move(Analytes.begin(), Analytes.end(), back_inserter(Samples));
	Analytes.clear();
	//Prototype header
	uint m=0;
	for(int label=0; label<TSample::label_number(); label++){
		if(TSample::label(label).length()>m){
			m=TSample::label(label).length();
		}
	}
	ofstream RES{outfile+".txt"};
	cout<<"\nSaving results to file "<<outfile+".txt";
	RES<<fixed<<setprecision(3);
	for(int lay=0; lay<Brain.layer_number(); lay++){
		///Column headers as neuron 'out' of layer 'lay': Nout/lay
		RES<<setw(m)<<" ";
		for(int out=0; out<Brain.output_number(lay); out++){
			RES<<"\tN"<<out<<"/"<<lay;
		}
		RES<<"\n";
		///Weights assigned by neuron to its input
		for(int inp=0; inp<Brain.input_number(); inp++){
			///Left margin column: input names as sample labels
			if(lay==0){
				if(inp<Samples[0].signal_number()){RES<<setw(m)<<Samples[0].label(inp);}
				if(inp==Samples[0].signal_number()){RES<<setw(m)<<"BIAS";}
			}
			///Left margin column: input names as preceding neuron numbers
			if(lay>=1){
				if(inp<Brain.neuron_number(lay-1)){RES<<setw(m)<<"N"<<inp<<"/"<<lay-1;}
				if(inp==Brain.neuron_number(lay-1)){RES<<setw(m)<<"BIAS";}
			}
			///Print weight
			for(int neu=0; neu<Brain.output_number(lay); neu++){
				RES<<"\t"<<Brain.weight_n(lay, neu, inp);
			}
			RES<<"\n";
		}
		RES<<"\n";
	}
	//Sample analysis report
	//Header with sample labels, net-assigned group, signam labels names and output neuron responses
	RES<<"\nLABEL\tGROUP";
	for(int i=0; i<Samples[0].signal_number(); i++){
		RES<<"\t"<<Samples[0].label(i).substr(0,7);
	}
	for(int i=0; i<Brain.output_number(); i++){
		RES<<"\tRES_"<<noshowpos<<i;
	}
	vector<vector<double>> Responses;
	for(uint pro=0; pro<Samples.size(); pro++){
		Responses.push_back(Brain.responses(Samples[pro]));
	}
	Responses.shrink_to_fit();
	///Table with values
	for(uint sample=0; sample<Samples.size(); sample++)
	{
		///Response patterns predefined in sample definition
		vector<double>Response_patterns=*Samples[sample].response_patterns();
		RES<<"\n";
		for(const auto &w:Response_patterns){
			RES<<w<<"\t";
		}
		///Selection of most activated output neuron
		double RESP=0;
		int m;
		for(uint resp=0; resp<Responses[sample].size(); resp++){
			if((Responses[sample][resp])>RESP){RESP=Responses[sample][resp]; m=resp;}
		}
		RES<<"PRO_"<<noshowpos<<m<<"\t";
		///Sample signals predefined in sample definition
		for(int s=0; s<Samples[sample].signal_number(); s++){
			RES<<Samples[sample].signal(s);
			if(s<Samples[sample].signal_number()-1){RES<<"\t";}
		}
		///Outpu neurons responses
		for(uint resp=0; resp<Responses[sample].size(); resp++){
			RES<<"\t"<<Responses[sample][resp];
		}
	}
	RES.close();	
}
void check_major_exceptions(const vector<TSample> &Samples, const vector<TSample> &Analytes, const TNet &Brain)
{
	try{if(Samples.empty()){
		throw runtime_error("Samples vector empty - nothing to learn from!");
		}
	}
	catch(exception &ex){
		cout<<endl<<ex.what();exit(-1);
	}

	try{if(Brain.is_empty()){
		throw runtime_error("Brain lobotomized - empty neural net!");
		}
	}
	catch(exception &ex){
		cout<<endl<<ex.what();exit(-1);
	}

	try{if(Analytes.empty()){
		throw runtime_error("Analytes vector empty - nothing to analyze!");
		}
	}
	catch(exception &ex){
		cout<<endl<<ex.what();exit(-1);
	}
}
