#ifndef CALEDIT_EVECLASS_H
#define CALEDIT_EVECLASS_H

//-------------------------------------------------------------------------------------
// EVECLASS                 MAIN
//-------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <regex>

class C_event{
	public:
		C_event();
		~C_event();
		void show();//show details
		std::vector<std::string>* pntr_vector();//return vector
		bool match(std::regex reg);//matching
		bool change(std::regex reg,std::string ersstr);//changing
		std::string cmpstart();		//just
		std::string cmpend();		//output
		std::string cmpstart_time();	//some
		std::string cmpend_time();	//stuff
		std::string cmpuuid();		//
		std::string cmpsummary();	//
		std::string showname();		//
		void add(std::string line);
		void checkstring(std::string line); //check if something important is in line
		void setname(std::string sname);
	private:
		std::string uuid;
		std::string start_date, start_time;
		std::string end_date, end_time;
		std::string summary;
		std::string name;
		std::vector<std::string> lines;
};

#endif //CALEDIT_EVECLASS_H
