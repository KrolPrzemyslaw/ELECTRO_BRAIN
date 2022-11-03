#ifndef TFILTER_h
#define TFILTER_h
///STRUCTURE: BASIC FILTER(abstract class)******************************
struct TFilter_base
{
	///Constructor******************************************************
	TFilter_base(vector<uint> channels)
	{
		Channels=channels;
	}
	///Filtering function***********************************************
	virtual bool operator ()(vector<double>signal)=0;
	///Protected data***************************************************
	protected:
	vector<uint> Channels;
};
///STRUCTURE: YES/NO FILTER*********************************************
struct TFilter_yes : public TFilter_base
{
	///Constructor******************************************************
	TFilter_yes(vector<uint> channels, bool yes) : TFilter_base(channels)
	{
		Yes=yes;
	}
	///Filtering function***********************************************
	bool operator()(vector<double> signal) override
	{
		bool allow=true;
		for(auto &k:Channels)
		{
			if(bool(signal[k])!=Yes)
			{
				allow=false;
				break;
			}
		}
		return allow;
	}
	///Private data*****************************************************
	private:
	bool Yes;
};
///STRUCTURE: HIGHER/LOWER FILTER***************************************
struct TFilter_high : public TFilter_base
{
	///Constructor******************************************************
	TFilter_high(vector<uint> channels, vector<double> thresholds, bool high_pass) : TFilter_base(channels)
	{
		Thresholds=thresholds;
		High_pass=high_pass;
	}
	///Filtering function***********************************************
	bool operator()(vector<double> signal) override
	{
		bool allow=true;
		if(High_pass)
		{
			for(uint kan=0; kan<Channels.size(); kan++)
			{
				if(signal[Channels[kan]]<=Thresholds[kan])
				{
					allow=false;
					break;
				}
			}
		}
		if(!High_pass)
		{
			for(uint kan=0; kan<Channels.size(); kan++)
			{
				if(signal[Channels[kan]]>=Thresholds[kan])
				{
					allow=false;
					break;
				}
			}
		}
		return allow;
	}
	void normalize_thresholds(vector<double> a, vector<double> b)
	{
		for(uint prog=0; prog<Thresholds.size(); prog++)
		{
			Thresholds[prog]=a[Channels[prog]]*Thresholds[prog]+b[Channels[prog]];
		}
	}
	///Private data*****************************************************
	private:
	vector<double> Thresholds;
	bool High_pass;
};
///*********************************************************************
#endif
