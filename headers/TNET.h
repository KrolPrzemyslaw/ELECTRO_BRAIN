#ifndef TNET_h
#define TNET_h
//SŁAWA WELESOWI!
#include<iostream>
#include<random>
#include<iomanip>
#include<fstream>
#include "TFILTER.h"
using namespace std;
constexpr double pi(){return 3.14159265358979;}
constexpr double e(){return 2.718281828459;}
struct TNeuron
{
	TNeuron(int input_number, string type, bool bias=false){
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0.0, 0.10);
		
		Bias=bias;
		for(int i=0; i<input_number; i++){
			Weights.push_back(dis(gen));
		}
		if(bias){
			Weights.push_back(dis(gen));
		}
		Filters_yes.clear();
		Filters_high.clear();
		Type=type;
		if(Type=="lin"){Resp_ptr=&TNeuron::resp_lin;}
		if(Type=="slin"){Resp_ptr=&TNeuron::resp_slin;}
		if(Type=="hlin"){Resp_ptr=&TNeuron::resp_hlin;}
		if(Type=="kwa"){Resp_ptr=&TNeuron::resp_kwa;}
		if(Type=="moc"){Resp_ptr=&TNeuron::resp_moc;}
		if(Type=="sig"){Resp_ptr=&TNeuron::resp_sig;}
		if(Type=="tgh"){Resp_ptr=&TNeuron::resp_tgh;}
		if(Type=="sin"){Resp_ptr=&TNeuron::resp_sin;}
		if(Type=="cos"){Resp_ptr=&TNeuron::resp_cos;}
	}
	TNeuron(int input_number, string type, vector<double> weights, bool bias=false){
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0.0, 0.10);
		
		Bias=bias;
		for(int i=0; i<input_number; i++){
			Weights.push_back(dis(gen));
		}
		if(bias){
			Weights.push_back(dis(gen));
		}
		Filters_yes.clear();
		Filters_high.clear();
		Type=type;
		if(Type=="lin"){Resp_ptr=&TNeuron::resp_lin;}
		if(Type=="slin"){Resp_ptr=&TNeuron::resp_slin;}
		if(Type=="hlin"){Resp_ptr=&TNeuron::resp_hlin;}
		if(Type=="kwa"){Resp_ptr=&TNeuron::resp_kwa;}
		if(Type=="moc"){Resp_ptr=&TNeuron::resp_moc;}
		if(Type=="sig"){Resp_ptr=&TNeuron::resp_sig;}
		if(Type=="tgh"){Resp_ptr=&TNeuron::resp_tgh;}
		if(Type=="sin"){Resp_ptr=&TNeuron::resp_sin;}
		if(Type=="cos"){Resp_ptr=&TNeuron::resp_cos;}
		Weights=weights;
	}
	double resp_lin(double X){return X;}
	double resp_slin(double X){return 2*(X-0.5);}
	double resp_hlin(double X){double Y=X; if(fabs(Y)>1){Y/=fabs(Y);}return Y;}
	double resp_kwa(double X){return X*X;}
	double resp_moc(double X){return X*X*X;}
	double resp_sig(double X){return 1.0/(1.0+pow(e(), -X));}
	double resp_tgh(double X){return tanh(X*pi());;}
	double resp_sin(double X){return sin(X*pi()*0.5);}
	double resp_cos(double X){return cos(X*pi()*0.5);}

	double response(vector<double> input_signals){
		double X=0, Y=0;
		if(allow_signal(input_signals)==true)
		{
			for(unsigned int inp=0; inp<Weights.size()-Bias; inp++)
			{
				X+=input_signals[inp]*Weights[inp];
			}
			if(Bias){X+=Weights.back();}
			Y=(this->*Resp_ptr)(X);
		}
		return Y;
	}
	double derivative(vector<double> input_signals){
		double X=0;
		for(unsigned int inp=0; inp<Weights.size()-1; inp++)
		{
			X+=input_signals[inp]*Weights[inp];
		}
		X+=Weights.back();
		double Y, Y1;
		if(Type=="lin"){Y1=1;}
		if(Type=="sig"){Y=1.0/(1.0+pow(e(), -X)); Y1=Y*(1-Y);}
		if(Type=="tgh"){Y=tanh(X*pi()); Y1=(1+Y)*(1-Y);}
		return Y1;
	}
	void add_filters(TFilter_yes filter_yes){
		Filters_yes.push_back(filter_yes);
	}
	void add_filters(TFilter_high filter_high){
		Filters_high.push_back(filter_high);
	}
	void free_filters(){
		Filters_yes.clear();
		Filters_high.clear();
	}
	bool allow_signal(vector<double> input_signals){
		bool allow=true;
		for(auto &f:Filters_yes)
		{
			if(!f(input_signals))
			{
				allow=false;
				break;
			}
		}
		for(auto &f:Filters_high){
			if(!f(input_signals)){
				allow=false;
				break;
			}
		}
		return allow;
	}
	void show_weights()const{
		cout<<"(";
		for(unsigned int w=0; w<Weights.size(); w++){
			cout<<Weights[w];
			if(w<Weights.size()-1){cout<<", ";}
		}
		cout<<")<"<<Type; if(Bias){cout<<", biased";}cout<<">";
	}
	double unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization){
		double Y=response(input_signals);
		for(unsigned int inp=0; inp<Weights.size()-Bias; inp++){	
			Weights[inp]+=input_signals[inp]*Y*learn_rate;
		}
		if(Bias){
			Weights.back()+=1.0*Y*learn_rate;
		}
		if(normalization){	
			double WM=0;
			for(auto &w:Weights){
				WM+=pow(w,2);
			}
			for(auto &w:Weights){
				w/=sqrt(WM);
			}
		}
		return Y;
	}
	private:
	vector<double> Weights;
	string Type;
	bool Bias;
	double(TNeuron::*Resp_ptr)(double);	
	vector<TFilter_yes> Filters_yes;
	vector<TFilter_high> Filters_high;
	
	friend struct TLayer;
	friend struct TNet;
};
struct TLayer
{
	TLayer(int neuron_number, int input_number, string type, bool bias){
		if(type!="amp"&&type!="sel"&&type!="pol"){
			for(int i=0; i<neuron_number; i++){
				Neurons.push_back(TNeuron(input_number, type, bias));
			}
		}
		if(type=="amp"){
			for(int i=0; i<neuron_number; i++){
				if(i%2==0){
					Neurons.push_back(TNeuron(input_number, "sin", bias));
				}
				if(i%2==1){
					Neurons.push_back(TNeuron(input_number, "cos", bias));
				}
			}
		}
	}
	TLayer(vector<TNeuron> neurons){
		Neurons=neurons;
	}
	void show_weights()const{
		for(auto &n:Neurons){
			n.show_weights();
			cout<<"   ";
		}
	}
	vector<double> responses(vector<double> input_signals){
		vector<double> resp;
		for(auto &n:Neurons){
			resp.push_back(n.response(input_signals));
		}
		return resp;
	}
	vector<double> derivatives(vector<double> input_signals){
		vector<double> drv;
		for(unsigned int neu=0; neu<Neurons.size(); neu++){
			drv.push_back(Neurons[neu].derivative(input_signals));
		}
		return drv;
	}
	vector<double> unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization){
		vector<double> resp;
		for(auto &n:Neurons){
			resp.push_back(n.unsupervised_learning(input_signals, learn_rate, normalization));
		}
		return resp;
	}
	int neuron_number()const{
		return Neurons.size();
	}
	int input_number()const{
		return Neurons[0].Weights.size();
	}
	void operator +=(TNeuron neuron){
		Neurons.push_back(neuron);
	}
	private:
	vector<TNeuron> Neurons;
	friend struct TNet;
};
struct TNet
{
	TNet(){}
	TNet(string name, int layer_number, int neuron_number, int input_number, string structure, string type, bool bias){
		Name=name;
		vector<int> neurons_in_layer;
		for(int lay=0; lay<layer_number; lay++)
		{
			if(structure=="dec"){
				neurons_in_layer.push_back(layer_number-lay);
			}
			if(structure=="tri"){
				if(lay<=layer_number/2){
					neuron_number=(lay+1);
				}
				if(lay>layer_number/2){
					neuron_number=layer_number-lay;
				}
				neurons_in_layer.push_back(neuron_number);
			}
			if(structure=="con"){
				neurons_in_layer.push_back(neuron_number);
			}
			if(structure=="conW"){
				if(lay<layer_number-1){
					neurons_in_layer.push_back(neuron_number);
				}
				if(lay==layer_number-1){
					neurons_in_layer.push_back(1);
				}
			}
		}
		if(type=="log"){
			Layers.push_back(TLayer(neurons_in_layer[0],input_number, "amp", bias));
			for(int lay=1; lay<layer_number; lay++){
				Layers.push_back(TLayer(neurons_in_layer[lay], Layers[lay-1].neuron_number(), "sig", bias));
			}
		}
		if(type!="log"){
			Layers.push_back(TLayer(neurons_in_layer[0], input_number, type, bias));
			for(int lay=1; lay<layer_number; lay++){
				Layers.push_back(TLayer(neurons_in_layer[lay], Layers[lay-1].neuron_number(), type, bias));
			}	
		}
	}
	TNet(string saved_net){
		Name=saved_net;
		int number_of_layers;
		ifstream NET{saved_net};
		NET>>number_of_layers;
		int number_of_inputs;
		int number_of_neurons;
		string type;
		bool bias;
		for(int lay=0; lay<number_of_layers; lay++){
			NET>>number_of_inputs>>number_of_neurons;
			vector<TNeuron> neurons;
			for(int neu=0; neu<number_of_neurons; neu++){
				NET>>type>>bias;
				vector<double> weights;
				for(int wgh=0; wgh<number_of_inputs; wgh++){
					double w; NET>>w; weights.push_back(w);
				}
				neurons.push_back(TNeuron(number_of_inputs, type, weights, bias));
				weights.clear();
			}
			*this+=TLayer(neurons);
			neurons.clear();
		}
		NET.close();
	}
	void assign_name(string name){
		Name=name;
	}
	void assign_filter(int layer, int neuron, TFilter_yes filter_yes){
		Layers[layer].Neurons[neuron].add_filters(filter_yes);
	}
	void assign_filter(int layer, int neuron, TFilter_high filter_high){
		Layers[layer].Neurons[neuron].add_filters(filter_high);
	}
	void free_filters(){
		for(uint lay=0; lay<Layers.size(); lay++){
			for(uint neu=0; neu<Layers[lay].Neurons.size(); neu++){
				Layers[lay].Neurons[neu].free_filters();
			}
		}
	}
	bool is_empty() const{
		bool empty=true;
		for(const auto &lay:Layers){
			for(const auto &neu:lay.Neurons){
				empty=false; break; neu.show_weights();
			}
		}
		return empty;
	}
	int neuron_number(int lay)const{
		return Layers[lay].neuron_number();
	}
	int layer_number()const{
		return Layers.size();
	}
	int input_number(int lay=0)const{
		return Layers[lay].input_number();
	}
	int output_number()const{
		return Layers.back().neuron_number();
	}
	int output_number(int lay)const{
		return Layers[lay].neuron_number();
	}
	void show_weights()const{
		cout<<fixed<<setprecision(3);
		for(auto &w:Layers){
			cout<<"\n";
			w.show_weights();
		}
		cout<<endl;
	}
	double weight(int lay, int neu, int inp)const{
		return Layers[lay].Neurons[neu].Weights[inp];
	}
	double weight_n(int lay, int neu, int inp)const{
		double norm=0;
		for(uint inp=0; inp<Layers[lay].Neurons[neu].Weights.size(); inp++){
			if(fabs(Layers[lay].Neurons[neu].Weights[inp])>norm){
				norm=Layers[lay].Neurons[neu].Weights[inp];
			}
		}	
		return Layers[lay].Neurons[neu].Weights[inp]/norm;
	}
	void assign_weight(int layer, int neuron, int input, double weight){
		Layers[layer].Neurons[neuron].Weights[input]=weight;
	}
	void change_weight(int layer, int neuron, int input, double dw){
		Layers[layer].Neurons[neuron].Weights[input]+=dw;
	}
	vector<double> responses(TSample sample){
		vector<double> signals=Layers[0].responses(*sample.signals());
		for(unsigned int lay=1; lay<Layers.size(); lay++){
			signals=Layers[lay].responses(signals);
		}
		return signals;
	}
	void unsupervised_learning(TSample &sample, double learn_rate, vector<double> &weights, bool normalization, bool forced=false){
		vector<double> signals=Layers[0].unsupervised_learning(*sample.signals(), learn_rate, normalization);
		for(unsigned int lay=1; lay<Layers.size(); lay++){
			signals=Layers[lay].unsupervised_learning(signals, learn_rate, normalization);
		}
		if(forced){
			cout<<"\nFeature not implemented yet! Learning without forced mode";
		}
		weights.clear();
		for(uint lay=0; lay<Layers.size(); lay++){
			for(uint neu=0; neu<Layers[lay].Neurons.size(); neu++){
				for(uint inp=0; inp<Layers[lay].Neurons[neu].Weights.size(); inp++){
					weights.push_back(Layers[lay].Neurons[neu].Weights[inp]);
				}
			}
		}
	}
	void save_net(){	
		cout<<"\nNet saving as "<<Name;
		ofstream NET{Name};		
		NET<<fixed<<layer_number();
		for(const auto &lay:Layers){
			NET<<"\n\t"<<lay.input_number()<<" "<<lay.neuron_number()<<"\n";
			for(const auto &neu:lay.Neurons){
				NET<<"\t\t"<<neu.Type<<" "<<setprecision(0)<<neu.Bias<<setprecision(6);
				for(const auto &wgh:neu.Weights){
					NET<<"\n\t\t\t"<<wgh;
				}
				NET<<"\n";
			}
			NET<<"\n";
		}
	}
	void operator +=(TLayer layer){
		Layers.push_back(layer);
	}
	private:
	vector<TLayer> Layers;
	string Name;
};
#endif
