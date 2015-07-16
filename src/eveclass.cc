//-------------------------------------------------------------------------------------
//   EVECLASS               MAIN
//-------------------------------------------------------------------------------------

#include <iostream>
#include "eveclass.h"

using namespace std;

//regular expressions:
regex reg2 (":([0-9]*)[A-Z]([0-9]*)"); // $1=Date, $2=Time

//Creator:
C_event::C_event(){}

//Destructor:
C_event::~C_event(){}

//Output some stuff:
vector<string>* C_event::pntr_vector(){return &lines;}
string C_event::cmpstart_time(){return start_time;}
string C_event::cmpend_time(){return end_time;}
string C_event::cmpuuid(){return uuid;}
string C_event::cmpsummary(){return summary;}
string C_event::showname(){return name;}


void C_event::setname(string sname){
	if(sname=="uuid")name=uuid;
	else name=sname;
}

void C_event::add(string line){
	lines.push_back(line);
	checkstring(line);
}

void C_event::checkstring(string line){
        string anfang(line,0,6);
        if(anfang=="SUMMAR"){
                summary=line;
                summary.erase(0,8);
        }
        if(anfang=="DTSTAR"){
                start_date=regex_replace(line,reg2,"$1",regex_constants::format_no_copy);
                start_time=regex_replace(line,reg2,"$2",regex_constants::format_no_copy);
        }
        anfang.pop_back();
        if(anfang=="DTEND"){
                end_date=regex_replace(line,reg2,"$1",regex_constants::format_no_copy);
                end_time=regex_replace(line,reg2,"$2",regex_constants::format_no_copy);
        }
        anfang.pop_back();
        if(anfang=="UID:"){
                uuid=line;
                uuid.erase(0,4);
        }

}

//show event:
void C_event::show(){
	cout << "------------------------------------------" << endl;
	cout << "Summary: "<<summary<<endl;
	cout << "Filename: "<<name<<endl;
	cout << "Start-Date: "<<start_date<<endl;
	cout << "Start-Time: "<<start_time<<endl;
	cout << "End-Date: "<<end_date<<endl;
	cout << "End-Time: "<<end_time<<endl;
}

//return-values for comparison:
string C_event::cmpstart(){
	return start_date+start_time;
}
string C_event::cmpend(){
	return end_date+end_time;
}

//matching
bool C_event::match(regex reg){
	for(int i=0;i<lines.size();i++){
		if(regex_search(lines[i],reg))return 1;
	}
	return 0;
}

//changing
bool C_event::change(regex reg,string ersstr){
	for(int i=0;i<lines.size();i++){
		if(regex_search(lines[i],reg)){
			lines[i]=regex_replace(lines[i],reg,ersstr);
			checkstring(lines[i]);
		}
	}
	return 0;
}
