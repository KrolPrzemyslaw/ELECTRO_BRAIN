#include"TFILTER.h"
using namespace std;
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