#ifndef TNET_h
#define TNET_h
//SŁAWA WELESOWI!
#include<iostream>
#include<random>
#include<iomanip>
#include<fstream>
#include<vector>
#include"TFILTER.h"
#include"TSAMPLE.h"
using namespace std;
constexpr double pi(){return 3.14159265358979;}
constexpr double e(){return 2.718281828459;}
struct TNeuron
{
	TNeuron(int input_number, string type, bool bias=false);
	TNeuron(int input_number, string type, vector<double> weights, bool bias=false);
	double resp_lin(double X){
		return X;
	}
	double resp_slin(double X){
		return 2*(X-0.5);
	}
	double resp_hlin(double X){
		double Y=X; if(fabs(Y)>1){Y/=fabs(Y);}return Y;
	}
	double resp_kwa(double X){
		return X*X;
	}
	double resp_moc(double X){
		return X*X*X;
	}
	double resp_sig(double X){
		return 1.0/(1.0+pow(e(), -X));
	}
	double resp_tgh(double X){
		return tanh(X*pi());;
	}
	double resp_sin(double X){
		return sin(X*pi()*0.5);
	}
	double resp_cos(double X){
		return cos(X*pi()*0.5);
	}
	double response(vector<double> input_signals);
	double derivative(vector<double> input_signals);
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
	bool allow_signal(vector<double> input_signals);
	void show_weights()const;
	double unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization);
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
	TLayer(int neuron_number, int input_number, string type, bool bias);
	TLayer(vector<TNeuron> neurons){
		Neurons=neurons;
	}
	void show_weights()const;
	vector<double> responses(vector<double> input_signals);
	vector<double> derivatives(vector<double> input_signals);
	vector<double> unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization);
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
	TNet(string name, int layer_number, int neuron_number, int input_number, string structure, string type, bool bias);
	TNet(string saved_net);
	void assign_name(string name){
		Name=name;
	}
	void assign_filter(int layer, int neuron, TFilter_yes filter_yes){
		Layers[layer].Neurons[neuron].add_filters(filter_yes);
	}
	void assign_filter(int layer, int neuron, TFilter_high filter_high){
		Layers[layer].Neurons[neuron].add_filters(filter_high);
	}
	void free_filters();
	bool is_empty() const;
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
	double weight_n(int lay, int neu, int inp)const;
	void assign_weight(int layer, int neuron, int input, double weight){
		Layers[layer].Neurons[neuron].Weights[input]=weight;
	}
	void change_weight(int layer, int neuron, int input, double dw){
		Layers[layer].Neurons[neuron].Weights[input]+=dw;
	}
	vector<double> responses(TSample sample);
	void unsupervised_learning(TSample &sample, double learn_rate, vector<double> &weights, bool normalization, bool forced=false);
	void save_net();
	void operator +=(TLayer layer){
		Layers.push_back(layer);
	}
	private:
	vector<TLayer> Layers;
	string Name;
};
#endif
