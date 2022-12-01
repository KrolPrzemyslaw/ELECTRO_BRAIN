#include"TNET.h"
bool TFilter_high::operator()(vector<double> signal){
	bool allow=true;
	if(High_pass){
		for(uint kan=0; kan<Channels.size(); kan++){
			if(signal[Channels[kan]]<=Thresholds[kan]){
				allow=false;
				break;
			}
		}
	}
	if(!High_pass){
		for(uint kan=0; kan<Channels.size(); kan++)	{
			if(signal[Channels[kan]]>=Thresholds[kan]){
				allow=false;
				break;
			}
		}
	}
	return allow;
}
void TFilter_high::normalize_thresholds(vector<double> linear_multiplier, vector<double> linear_base){
	for(uint prog=0; prog<Thresholds.size(); prog++){
		Thresholds[prog]=linear_multiplier[Channels[prog]]*Thresholds[prog]+linear_base[Channels[prog]];
	}
}
bool TFilter_yes::operator()(vector<double> signal){
	bool allow=true;
	for(auto &k:Channels){
		if(bool(signal[k])!=Yes){
			allow=false;
			break;
		}
	}
	return allow;
}
TNeuron::TNeuron(int input_number, string type, bool bias){
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
TNeuron::TNeuron(int input_number, string type, vector<double> weights, bool bias){
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
double TNeuron::response(vector<double> input_signals){
	double X=0, Y=0;
	if(allow_signal(input_signals)==true)
	{
		for(unsigned int inp=0; inp<Weights.size()-Bias; inp++){
			X+=input_signals[inp]*Weights[inp];
		}
		if(Bias){
			X+=Weights.back();
		}
		Y=(this->*Resp_ptr)(X);
	}
	return Y;
}
double TNeuron::derivative(vector<double> input_signals){
	double X=0;
	for(unsigned int inp=0; inp<Weights.size()-1; inp++){
		X+=input_signals[inp]*Weights[inp];
	}
	X+=Weights.back();
	double Y, Y1;
	if(Type=="lin"){
		Y1=1;
	}
	if(Type=="sig"){
		Y=1.0/(1.0+pow(e(), -X)); 
		Y1=Y*(1-Y);
	}
	if(Type=="tgh"){
		Y=tanh(X*pi()); 
		Y1=(1+Y)*(1-Y);
	}
	return Y1;
}
bool TNeuron::allow_signal(vector<double> input_signals){
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
void TNeuron::show_weights()const{
	cout<<"(";
	for(unsigned int w=0; w<Weights.size(); w++){
		cout<<Weights[w];
		if(w<Weights.size()-1){cout<<", ";}
	}
	cout<<")<"<<Type; if(Bias){cout<<", biased";}cout<<">";
}
double TNeuron::unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization){
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

TLayer::TLayer(int neuron_number, int input_number, string type, bool bias){
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
void TLayer::show_weights()const{
	for(auto &n:Neurons){
		n.show_weights();
		cout<<"   ";
	}
}
vector<double> TLayer::responses(vector<double> input_signals){
	vector<double> resp;
	for(auto &n:Neurons){
		resp.push_back(n.response(input_signals));
	}
	return resp;
}
vector<double> TLayer::derivatives(vector<double> input_signals){
	vector<double> drv;
	for(unsigned int neu=0; neu<Neurons.size(); neu++){
		drv.push_back(Neurons[neu].derivative(input_signals));
	}
	return drv;
}
vector<double> TLayer::unsupervised_learning(vector<double> input_signals, double learn_rate, bool normalization){
	vector<double> resp;
	for(auto &n:Neurons){
		resp.push_back(n.unsupervised_learning(input_signals, learn_rate, normalization));
	}
	return resp;
}

TNet::TNet(string name, int layer_number, int neuron_number, int input_number, string structure, string type, bool bias){
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
TNet::TNet(string saved_net){
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
bool TNet::is_empty() const{
	bool empty=true;
	for(const auto &lay:Layers){
		for(const auto &neu:lay.Neurons){
			empty=false; break; neu.show_weights();
		}
	}
	return empty;
}
void TNet::free_filters(){
	for(uint lay=0; lay<Layers.size(); lay++){
		for(uint neu=0; neu<Layers[lay].Neurons.size(); neu++){
			Layers[lay].Neurons[neu].free_filters();
		}
	}
}
double TNet::weight_n(int lay, int neu, int inp)const{
	double norm=0;
	for(uint inps=0; inps<Layers[lay].Neurons[neu].Weights.size(); inps++){
		if(fabs(Layers[lay].Neurons[neu].Weights[inps])>norm){
			norm=Layers[lay].Neurons[neu].Weights[inps];
		}
	}	
	return Layers[lay].Neurons[neu].Weights[inp]/norm;
}
vector<double> TNet::responses(TSample sample){
	vector<double> signals=Layers[0].responses(*sample.signals());
	for(unsigned int lay=1; lay<Layers.size(); lay++){
		signals=Layers[lay].responses(signals);
	}
	return signals;
}
void TNet::unsupervised_learning(TSample &sample, double learn_rate, vector<double> &weights, bool normalization, bool forced){
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
void TNet::save_net(){	
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
