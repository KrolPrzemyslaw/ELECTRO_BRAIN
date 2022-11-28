#ifndef TCONF_H
#define TCONF_H
#include<iostream>
#include<string.h>
using namespace std;
//SŁAWA WELESOWI!
struct TConfiguration_input{

	TConfiguration_input(){
		Normalization=true;
	}
	void assign_learn_cycles(int learn_cycles){
		Learn_cycles=learn_cycles;
	}
	void assign_learn_mode(string learn_mode){
		Learn_mode=learn_mode;
	}
	void decide_on_normalization(bool normalization){
		Normalization=normalization;
	}
	void assign_output_file_name(string outfile){
		Output_file_name=outfile;
	}
	bool normalization()const{
		return Normalization;
	}
	int learn_cycles()const{
		return Learn_cycles;
	}
	string learn_mode()const{
		return Learn_mode;
	}
	string output_file_name()const{
		return Output_file_name;
	}
	private:
	int Learn_cycles;
	string Learn_mode;
	bool Normalization;
	string Output_file_name;
};
#endif
