#ifndef TFILTER_h
#define TFILTER_h
struct TFilter_base{
	TFilter_base(vector<uint> channels){
		Channels=channels;
	}
	virtual bool operator ()(vector<double>signal)=0;
	protected:
	vector<uint> Channels;
};
struct TFilter_yes : public TFilter_base{
	TFilter_yes(vector<uint> channels, bool yes) : TFilter_base(channels){
		Yes=yes;
	}
	bool operator()(vector<double> signal) override{
		bool allow=true;
		for(auto &k:Channels){
			if(bool(signal[k])!=Yes){
				allow=false;
				break;
			}
		}
		return allow;
	}
	private:
	bool Yes;
};
struct TFilter_high : public TFilter_base{
	TFilter_high(vector<uint> channels, vector<double> thresholds, bool high_pass) : TFilter_base(channels){
		Thresholds=thresholds;
		High_pass=high_pass;
	}
	bool operator()(vector<double> signal) override{
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
	void normalize_thresholds(vector<double> linear_multiplier, vector<double> linear_base){
		for(uint prog=0; prog<Thresholds.size(); prog++){
			Thresholds[prog]=linear_multiplier[Channels[prog]]*Thresholds[prog]+linear_base[Channels[prog]];
		}
	}
	private:
	vector<double> Thresholds;
	bool High_pass;
};
#endif
