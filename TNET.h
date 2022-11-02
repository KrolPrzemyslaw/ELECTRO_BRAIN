#ifndef TNET_h
#define TNET_h
//SŁAWA WELESOWI!
#include<iostream>
#include<random>
using namespace std;
#include "TFILTER.h"
//Constants
constexpr double pi(){return 3.14159265358979;}
constexpr double e(){return 2.718281828459;}
///STRUCTURE: NEURON
struct TNeuron
{
	///Constructors*****************************************************
	TNeuron(int input_number, string type, bool bias=false)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0.0, 0.10);
		
		Bias=bias;
		for(int i=0; i<input_number; i++)
		{
			Weights.push_back(dis(gen));
		}
		if(bias){Weights.push_back(dis(gen));}
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
	TNeuron(int input_number, string type, vector<double> weights, bool bias=false)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0.0, 0.10);
		
		Bias=bias;
		for(int i=0; i<input_number; i++)
		{
			Weights.push_back(dis(gen));
		}
		if(bias){Weights.push_back(dis(gen));}
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
	///Neuron response functions****************************************
	double resp_lin(double X){return X;}
	double resp_slin(double X){return 2*(X-0.5);}
	double resp_hlin(double X){double Y=X; if(fabs(Y)>1){Y/=fabs(Y);}return Y;}
	double resp_kwa(double X){return X*X;}
	double resp_moc(double X){return X*X*X;}
	double resp_sig(double X){return 1.0/(1.0+pow(e(), -X));}
	double resp_tgh(double X){return tanh(X*pi());;}
	double resp_sin(double X){return sin(X*pi()*0.5);}
	double resp_cos(double X){return cos(X*pi()*0.5);}
	///Neuron response to input signal vector***************************
	double response(vector<double> input_signals)
	{
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
	///Derivative of neuron response to input signal vector*************
	double derivative(vector<double> input_signals)
	{
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
	///Assigning input signal filters to the neuron*********************
	void add_filters(TFilter_yes filter_yes)
	{
		Filters_yes.push_back(filter_yes);
	}
	void add_filters(TFilter_high filter_high)
	{
		Filters_high.push_back(filter_high);
	}
	///Releasing input signal filters***********************************
	void free_filters()
	{
		Filters_yes.clear();
		Filters_high.clear();
	}
	///Boolean function for input signal acceptance*********************
	bool allow_signal(vector<double> input_signals)
	{
		bool allow=true;
		for(auto &f:Filters_yes)
		{
			if(!f(input_signals))
			{
				allow=false;
				break;
			}
		}
		for(auto &f:Filters_high)
		{
			if(!f(input_signals))
			{
				allow=false;
				break;
			}
		}
		return allow;
	}
	///Show input signal weights****************************************
	void show_weights()const
	{
		cout<<"(";
		for(unsigned int w=0; w<Weights.size(); w++)
		{
			cout<<Weights[w];
			if(w<Weights.size()-1){cout<<", ";}
		}
		cout<<")<"<<Type; if(Bias){cout<<", biased";}cout<<">";
	}
	///Neuron learning**************************************************
	double unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization)
	{
		double Y=response(input_signals);
		for(unsigned int inp=0; inp<Weights.size()-Bias; inp++)
		{	
			Weights[inp]+=input_signals[inp]*Y*learn_rate;
		}
		if(Bias){Weights.back()+=1.0*Y*learn_rate;}
		if(normalization)
		{	
			double WM=0;
			for(auto &w:Weights)
			{
				WM+=pow(w,2);
			}
			for(auto &w:Weights)
			{
				w/=sqrt(WM);
			}
		}
		return Y;
	}
	//void forced_unsupervised_learning(vector<double> input_signals, double response_pattern, 
	//double learn_rate, bool normalization)
	//{
		////double Y=response(input_signals);
		////double sw[Weights.size()];
		//for(unsigned int inp=0; inp<Weights.size(); inp++)
		//{	
			////sw[inp]=Weights[inp];
			//Weights[inp]+=learn_rate*(response_pattern-Weights[inp]);//+0.6*learn_rate*(Weights[inp]-sw[inp]);
		//}
		////if(Bias){Weights.back()+=1.0*Y*learn_rate;}
		////normalization wag wejść neuronów
		//if(normalization)
		//{	
			//double WM=0;
			//for(auto &w:Weights)
			//{
				//WM+=pow(w,2);
			//}
			//for(auto &w:Weights)
			//{
				//w/=sqrt(WM);
			//}
		//}
	//}
	//void supervised_learning(vector<double> input_signals, double error, double learn_rate)
	//{
		//double Y=response(input_signals);
		//double Y1=derivative(input_signals);
		//for(unsigned int inp=0; inp<Weights.size(); inp++)
		//{
			//Weights[inp]+=
			//input_signals[inp]*Y*Y1*error*learn_rate;
		//}
	//}
	///private data*****************************************************
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
///STRUCTURE: NEURON LAYER**********************************************
struct TLayer
{
	///Constructor******************************************************
	TLayer(int neuron_number, int input_number, string type, bool bias)
	{
		if(type!="amp"&&type!="sel"&&type!="pol")
		{
			for(int i=0; i<neuron_number; i++)
			{
				Neurons.push_back(TNeuron(input_number, type, bias));
			}
		}
		if(type=="amp")
		{
			for(int i=0; i<neuron_number; i++)
			{
				if(i%2==0){Neurons.push_back(TNeuron(input_number, "sin", bias));}
				if(i%2==1){Neurons.push_back(TNeuron(input_number, "cos", bias));}
			}
		}
	}
	///Constructor******************************************************
	TLayer(vector<TNeuron> neurons)
	{
		Neurons=neurons;
	}
	///Print input signal weights for entire layer**********************
	void show_weights()const
	{
		for(auto &n:Neurons)
		{
			n.show_weights();
			cout<<"   ";
		}
	}
	///Layer responses vector for given input vector********************
	vector<double> responses(vector<double> input_signals)
	{
		vector<double> resp;
		for(auto &n:Neurons)
		{
			resp.push_back(n.response(input_signals));
		}
		return resp;
	}
	///Derivatives of layer responses for given input vector************
	vector<double> derivatives(vector<double> input_signals)
	{
		vector<double> drv;
		for(unsigned int neu=0; neu<Neurons.size(); neu++)
		{
			drv.push_back(Neurons[neu].derivative(input_signals));
		}
		return drv;
	}
	///Unsupervised learning, returning response vector*****************
	vector<double> unsupervised_learning(vector<double> input_signals, 
	double learn_rate, bool normalization)
	{
		vector<double> resp;
		for(auto &n:Neurons)
		{
			resp.push_back(n.unsupervised_learning(input_signals, learn_rate, normalization));
		}
		return resp;
	}
	//void forced_unsupervised_learning(
	//vector<double> input_signals, vector<double> response_patterns, 
	//double learn_rate, bool normalization)
	//{
		////vector<double> resp;
		//for(unsigned int neu=0; neu<Neurons.size(); neu++)
		//{
			////resp.push_back(
			//Neurons[neu].forced_unsupervised_learning(input_signals, response_patterns[neu], learn_rate, normalization);
			////);
		//}
	//}
	//vector<double> unsupervised_learning(int neuron, vector<double> input_signals, 
	//double learn_rate, bool normalization, bool bias)
	//{
		//double resp=Neurons[neuron].unsupervised_learning(input_signals, learn_rate, stalosc_wag, normalization, bias);
		//return {resp};
	//}
	///Return number of neurons in layer********************************
	int neuron_number()const{return Neurons.size();}
	///Return number of inputs per neuron*******************************
	int input_number()const{return Neurons[0].Weights.size();}
	///Adding neuron to the layer***************************************
	void operator +=(TNeuron neuron)
	{
		Neurons.push_back(neuron);
	}
	///Private data*****************************************************
	private:
	vector<TNeuron> Neurons;
	friend struct TNet;
};
///STRUCTURE: NEURON NET************************************************
struct TNet
{
	///Constructors*****************************************************
	TNet(){}
	TNet(int layer_number, int neuron_number, int input_number, string structure, string type, bool bias)
	{
		vector<int> neurons_in_layer;
		for(int lay=0; lay<layer_number; lay++)
		{
			if(structure=="dec")
			{
				neurons_in_layer.push_back(layer_number-lay);
			}
			if(structure=="tri")
			{
				if(lay<=layer_number/2)
				{
					neuron_number=(lay+1);
				}
				if(lay>layer_number/2)
				{
					neuron_number=layer_number-lay;
				}
				neurons_in_layer.push_back(neuron_number);
			}
			if(structure=="con")
			{
				neurons_in_layer.push_back(neuron_number);
			}
			if(structure=="conW")
			{
				if(lay<layer_number-1){neurons_in_layer.push_back(neuron_number);}
				if(lay==layer_number-1){neurons_in_layer.push_back(1);}
			}
		}
		if(type=="log")
		{
			Layers.push_back(TLayer(neurons_in_layer[0],input_number, "amp", bias));
			//Layers.push_back(TLayer(neurons_in_layer[1],input_number, "prod"));
			for(int lay=1; lay<layer_number; lay++)
			{
				Layers.push_back(TLayer(neurons_in_layer[lay], Layers[lay-1].neuron_number(), "sig", bias));
			}
			//Layers.push_back(TLayer(neurons_in_layer[layer_number-1], Layers.back().neuron_number(), "tgh", Bias));	
		}
		if(type!="log")
		{
			Layers.push_back(TLayer(neurons_in_layer[0], input_number, type, bias));
			for(int lay=1; lay<layer_number; lay++)
			{
				Layers.push_back(TLayer(neurons_in_layer[lay], Layers[lay-1].neuron_number(), type, bias));
			}	
		}
	}
	TNet(string saved_net)
	{
		int number_of_layers;
		ifstream NET{saved_net};
		NET>>number_of_layers;
		int number_of_inputs;
		int number_of_neurons;
		string type;
		bool bias;
		for(int lay=0; lay<number_of_layers; lay++)
		{
			NET>>number_of_inputs>>number_of_neurons;
			vector<TNeuron> neurons;
			for(int neu=0; neu<number_of_neurons; neu++)
			{
				NET>>type>>bias;
				vector<double> weights;
				for(int wgh=0; wgh<number_of_inputs; wgh++)
				{
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
	///Assign yes filter to target neuron in layer**********************
	void assign_filter(int layer, int neuron, TFilter_yes filter_yes)
	{
		Layers[layer].Neurons[neuron].add_filters(filter_yes);
	}
	///Assign high filter to target neuron in layer*********************
	void assign_filter(int layer, int neuron, TFilter_high filter_high)
	{
		Layers[layer].Neurons[neuron].add_filters(filter_high);
	}
	///Release all filters on net***************************************
	void free_filters()
	{
		for(uint lay=0; lay<Layers.size(); lay++)
		{
			for(uint neu=0; neu<Layers[lay].Neurons.size(); neu++)
			{
				Layers[lay].Neurons[neu].free_filters();
			}
		}
	}
	///Checks if net is empty*******************************************
	bool is_empty()
	{
		bool empty=true;
		for(const auto &lay:Layers)
		{
			for(const auto &neu:lay.Neurons)
			{
				empty=false; break;
			}
		}
		return empty;
	}
	///Return number of neurons in given layer**************************
	int neuron_number(int lay)const
	{
		return Layers[lay].neuron_number();
	}
	///Return number of layers******************************************
	int layer_number()const
	{
		return Layers.size();
	}
	///Return number of inputs in target layer (for frint by default)***
	int input_number(int lay=0)const
	{
		return Layers[lay].input_number();
	}
	///Return number of neurons in last layer***************************
	int output_number()const
	{
		return Layers.back().neuron_number();
	}
	///Return number of neurons in target layer*************************
	int output_number(int lay)const
	{
		return Layers[lay].neuron_number();
	}
	///Prints input weights for all neurons in net**********************
	void show_weights()const
	{
		for(auto &w:Layers)
		{
			cout<<"\n";
			w.show_weights();
		}
	}
	///Returns weight at target input of target neuron******************
	double weight(int lay, int neu, int inp)const
	{
		return Layers[lay].Neurons[neu].Weights[inp];
	}
	///Return weight of target input, normalized by highest weight in
	///target neuron's layer
	double weight_n(int lay, int neu, int inp)const
	{
		double norm=0;
		for(uint inp=0; inp<Layers[lay].Neurons[neu].Weights.size(); inp++)
		{
			if(fabs(Layers[lay].Neurons[neu].Weights[inp])>norm)
			{
				norm=Layers[lay].Neurons[neu].Weights[inp];
			}
		}	
		return Layers[lay].Neurons[neu].Weights[inp]/norm;
	}
	///Assignes prescribed value to target input************************
	void assign_weight(int layer, int neuron, int input, double weight)
	{
		Layers[layer].Neurons[neuron].Weights[input]=weight;
	}
	///Changes target weight by gived value*****************************
	void change_weight(int layer, int neuron, int input, double dw)
	{
		Layers[layer].Neurons[neuron].Weights[input]+=dw;
	}
	///Returns net response to given sample*****************************
	vector<double> responses(TSample sample)
	{
		vector<double> signals=Layers[0].responses(*sample.signals());
		for(unsigned int lay=1; lay<Layers.size(); lay++)
		{
			signals=Layers[lay].responses(signals);
		}
		return signals;
	}
	//vector<double> responses(vector<double> input_signals,
	 //vector<vector<double>> &Y, vector<vector<double>> &Y1)
	//{
		//vector<double> signals=Layers[0].responses(input_signals);
		//Y.push_back(signals);
		//Y1.push_back(Layers[0].derivatives(input_signals));
		//for(unsigned int lay=1; lay<Layers.size(); lay++)
		//{
			//Y1.push_back(Layers[lay].derivatives(signals));
		
			//signals=Layers[lay].responses(signals);
			//Y.push_back(signals);
		//}
		//return signals;
	//}
	///Net unsupervied learning, return void****************************
	void unsupervised_learning(TSample &sample, double learn_rate, 
	vector<double> &weights, bool normalization, bool forced=false)
	{
		vector<double> signals=Layers[0].unsupervised_learning(*sample.signals(), learn_rate, normalization);
		for(unsigned int lay=1; lay<Layers.size(); lay++)
		{
			signals=Layers[lay].unsupervised_learning(signals, learn_rate, normalization);
		}
		if(forced)
		{
			cout<<"\nFeature not implemented yet! Learning without forced mode";
			//vector<double> response_patterns=*sample.response_patterns();
			//Layers.back().forced_unsupervised_learning(signals, response_patterns, learn_rate, normalization);
		}
		weights.clear();
		for(uint lay=0; lay<Layers.size(); lay++)
		{
			for(uint neu=0; neu<Layers[lay].Neurons.size(); neu++)
			{
				for(uint inp=0; inp<Layers[lay].Neurons[neu].Weights.size(); inp++)
				{
					weights.push_back(Layers[lay].Neurons[neu].Weights[inp]);
				}
			}
		}
	}
	//void uczenie_nienadzorowane_z_konkurencja(vector<double> input_signals, double learn_rate, bool normalization)
	//{
		//vector<double> signals=Layers[0].responses(input_signals, Bias);
		//double y=0;
		//int m;
		//for(uint s=0; s<signals.size(); s++)
		//{
			//if(fabs(signals[s])>fabs(y))
			//{
				//y=signals[s];
				//m=s;
			//}
		//}
		//cout<<"\nUczę neuron "<<m;
		//Layers[0].unsupervised_learning(m, signals, learn_rate, normalization, Bias);
		
		//for(unsigned int lay=1; lay<Layers.size(); lay++)
		//{
			//signals=Layers[lay].responses(signals, Bias);
			
			//y=0;
			//for(uint s=0; s<signals.size(); s++)
			//{
				//if(fabs(signals[s])>fabs(y))
				//{
					//y=signals[s];
					//m=s;
				//}
			//}
			//Layers[lay].uczenie_nienadzorowane_z_kontrastem(m, signals, learn_rate, normalization, Bias);
		//}
	//}
	//vector<double> bledy(vector<double> input_signals, vector<double> response_patterns,
	 //vector<vector<double>> &Y, vector<vector<double>> &Y1)
	//{
		///*uwaga - przy uczeniu nadzorozwanym
		 //* jeżeli wzorców może być mniej niż sygnałów wyjściowych
		 //* wtedy zaczną się po wyczerpaniu powtarzać od początku
		 //* jeżeli wzorców będzie więcej to załąpią się tylko pierwsze
		//*/
		//vector<double> bledy=responses(input_signals, Y, Y1);
		//unsigned int wz=0;
		//for(unsigned int ble=0; ble<bledy.size(); ble++)
		//{
			//bledy[ble]=response_patterns[wz]-bledy[ble];
			//wz++; if(wz==response_patterns.size()-1){wz=0;}
		//}
		//return bledy;
	//}
	//void supervised_learning(vector<double> input_signals, vector<double> response_patterns, double learn_rate)
	//{
		//vector<vector<double>> Y, Y1;
		//vector<double> delta=bledy(input_signals, response_patterns, Y, Y1);
		////cout<<"\nX:";
		////for(auto &x:input_signals)
		////{
			////cout<<x<<" ";
		////}
		////cout<<"\n\tWartości sygnałów";
		////for(unsigned int y=0; y<Y.size(); y++)
		////{
			////cout<<"\n\t";
			////for(unsigned int yy=0; yy<Y[y].size(); yy++)
			////{
				////cout<<Y[y][yy]<<"\t";
			////}
		////}
		////cout<<"\n\tWartości pochodnych";
		////for(unsigned int y=0; y<Y.size(); y++)
		////{
			////cout<<"\n\t";
			////for(unsigned int yy=0; yy<Y[y].size(); yy++)
			////{
				////cout<<Y1[y][yy]<<"\t";
			////}
		////}
		
	//}
	///Save net to ASCII file*******************************************
	void save_net(string name)const
	{	
		cout<<"\nNet saving as "<<name;
		ofstream NET{name};		
		NET<<fixed<<layer_number();
		for(const auto &lay:Layers)
		{
			NET<<"\n\t"<<lay.input_number()<<" "<<lay.neuron_number()<<"\n";
			for(const auto &neu:lay.Neurons)
			{
				NET<<"\t\t"<<neu.Type<<" "<<setprecision(0)<<neu.Bias<<setprecision(6);
				for(const auto &wgh:neu.Weights)
				{
					NET<<"\n\t\t\t"<<wgh;
				}
				NET<<"\n";
			}
			NET<<"\n";
		}
	}

	///Adding layer to the net******************************************
	void operator +=(TLayer layer)
	{
		Layers.push_back(layer);
	}
	///Private data*****************************************************
	private:
	vector<TLayer> Layers;
};
///*********************************************************************
#endif
