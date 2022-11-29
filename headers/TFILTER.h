#ifndef TFILTER_h
#define TFILTER_h
#include<vector>
using namespace std;
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
	bool operator()(vector<double> signal) override;
	private:
	bool Yes;
};
struct TFilter_high : public TFilter_base{
	TFilter_high(vector<uint> channels, vector<double> thresholds, bool high_pass) : TFilter_base(channels){
		Thresholds=thresholds;
		High_pass=high_pass;
	}
	bool operator()(vector<double> signal) override;
	void normalize_thresholds(vector<double> linear_multiplier, vector<double> linear_base);
	private:
	vector<double> Thresholds;
	bool High_pass;
};
#endif
