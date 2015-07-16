//-------------------------------------------------------------------------------------
// Calendar-Class:      HEADER
//-------------------------------------------------------------------------------------

#ifndef CALEDIT_CALCLASS_H
#define CALEDIT_CALCLASS_H

#include <list>
#include <vector>
#include <string>

#include "eveclass.h"

using namespace std;

class C_calendar{
	public:
		//no void here! (execu wont work otherwise)
		C_calendar();
		~C_calendar();
		bool is_ics();
		bool is_changed();
		bool is_selected();
		int size();
		bool show(); //show everything
		bool show_selected(); //show selected events
		std::string show_status(); //return status for statusbar and shit
		int select(int todo,std::string str1,std::string str2); //select events
		bool rmsel(); //clear selection
		int remove(); //remove selected events
		int change(bool todo,std::string str1,std::string str2); //edit selected events
		bool save_as_ics(std::string fileadresse);
		std::list<C_event>* pntr_list(); //REMOVE THAT SHIT!!!!
		std::list<std::list<C_event>::iterator>* pntr_selected();
	protected:
		int finish_load();
		static bool vergleich(C_event event1,C_event event2);
		bool isics;
		std::list<C_event> events;
		std::list<std::list<C_event>::iterator> selected; //selected events (should never be 0 and listsize if nothing is selected)
		std::vector<std::string> removed; //removed events
		std::list<std::list<C_event>::iterator> changed; //changed events
		std::vector<std::string> begin; //begin of ics-file (used for saving in subclasses)
		std::vector<std::string> end; //end of ics-file (used for saving in subclasses)
};

//-------------------------------------------------------------------------------------
// Subclass for CalDav:
//-------------------------------------------------------------------------------------

class C_caldav : public C_calendar{
	public:
		C_caldav();
		~C_caldav();
		int load(std::string calname, std::string pid);
		bool save();
	private:
		std::string url;
		std::string tmpdir,icsdir;
		std::string delfile; //this file saves the commands for cadaver
};

//-------------------------------------------------------------------------------------
//Subclass for ICS:
//-------------------------------------------------------------------------------------

class C_ics : public C_calendar{
	public:
		C_ics();
		~C_ics();
		bool save();
		int load(std::string filename, std::string pid);
	private:
		std::string adress;
};

//-------------------------------------------------------------------------------------
//Create Calender and return pointer:
//-------------------------------------------------------------------------------------
C_calendar* createcal(string calname);
//-------------------------------------------------------------------------------------
//Check calendar-type of pointer, type-cast pointer to right type and execute command:
//-------------------------------------------------------------------------------------
#define EXECU(retu,kalender,ausfu) \
        if((*kalender).is_ics()==1){   \
                C_ics* calf = static_cast<C_ics*>(kalender);  \
                retu = (*calf).ausfu;  \
        }   \
        else{   \
                C_caldav* calf = static_cast<C_caldav*>(kalender);   \
                retu = (*calf).ausfu;   \
        } \


#endif //CALEDIT_CALCLASS_H
