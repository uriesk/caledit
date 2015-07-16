//-------------------------------------------------------------------------------------
// Calendar-Class:            MAIN
//-------------------------------------------------------------------------------------

#include <iostream>
#include <algorithm> //for events.sort
#include <regex> //Regular Expressions
#include <dirent.h> //show directory
#include <fstream> //read/write files

#include "functions.h"
#include "calclass.h"

using namespace std;


//Creator
C_calendar::C_calendar(){
	begin.push_back("BEGIN:VCALENDAR");
	begin.push_back("VERSION:2.0");
	begin.push_back("PRODID:-URIESK RULES-");
	begin.push_back("BEGIN:VEVENT");
	end.push_back("END:VEVENT");
	end.push_back("END:VCALENDAR");
}

//Destructor
C_calendar::~C_calendar(){}

//Output some stuff:
bool C_calendar::is_ics(){return isics;}
int C_calendar::size(){return events.size();}
list<C_event>* C_calendar::pntr_list(){return &events;} //REMOVE THAT SHIT!!!!
list<list<C_event>::iterator>* C_calendar::pntr_selected(){return &selected;}

//should be executed after event-list is filled:
int C_calendar::finish_load(){
	if(events.size()==0){
		cout << "Error: No events found!" << endl;
		return 1;
	}
	//cout << "Sort events:"<<endl;
	//events.sort(vergleich);
	rmsel();
	cout << events.size() << " Events loaded!..." << endl;
	return 0;
}

bool C_calendar::vergleich(C_event event1, C_event event2){
	return event1.cmpstart() < event2.cmpstart();
}

//show all Events:
bool C_calendar::show(){
	for(list<C_event>::iterator iter=events.begin();iter!=events.end();++iter)(*iter).show();
}

//show selected Events:
bool C_calendar::show_selected(){
	if(is_selected()==0){cout<<"Nothing selected!"<<endl;return 1;}
	for(list<list<C_event>::iterator>::iterator iter=selected.begin();iter!=selected.end();++iter)(**iter).show();
	return 0;
}

//return some status-string:
string C_calendar::show_status(){
	string status = static_cast<ostringstream*>( &(ostringstream() << "Unsaved Changes: Deleted: "<<removed.size()<<" Changed: "<<changed.size()) )->str();
	return status;
}

//Check if something got changed or removed:
bool C_calendar::is_changed(){
	if((changed.size()!=0)||(removed.size()!=0))return 1;
	return 0;
}

//Check if something is selected:
bool C_calendar::is_selected(){
	if(selected.size()!=events.size())return 1;
	return 0;
}

//"Remove" selection:
// If nothing is selected = EVERYTHING is selected
bool C_calendar::rmsel(){
	selected.clear();
	//selected = iterlist from eventlist
	list<C_event>::iterator iter;
	iter=events.begin();
	while(iter!=events.end()){
		selected.push_back(iter);
		++iter;
	}
	return 0;
}

//Selecting events (out of selected-list):
//todo:    0) general  1)Startdate from 2)Startdate to
//         3)exact Start- and Endtime
int C_calendar::select(int todo,string str1,string str2){
	//Create regular expression (for 1):
	string regstr;
	if(todo==0){
		regstr=str1+"[^:]*:.*"+str2;
		//cout<<"RegEXP: "<<regstr<<endl;
	}
	regex reg(regstr);
	//matched events are stored in iterlist
	list<list<C_event>::iterator> iterlist;
	list<list<C_event>::iterator>::iterator iter;
	iter=selected.begin();
	while(iter!=selected.end()){
		if((todo==0)&&((**iter).match(reg)==1))iterlist.push_back(*iter);
		if((todo==1)&&((**iter).cmpstart()>=(str1+str2)))iterlist.push_back(*iter);
		if((todo==2)&&((**iter).cmpstart()<=(str1+str2)))iterlist.push_back(*iter);
		if(todo==3){
			string start=(**iter).cmpstart_time();
			string end=(**iter).cmpend_time();
			if((start.compare(0,6,str1)==0)&&(end.compare(0,6,str2)==0))iterlist.push_back(*iter);
		}
		++iter;
	}
	//replace selected list with iterlist
	if(iterlist.size()==0){cout<<"No matching event..."<<endl;return 1;}
	selected.clear();
	selected.splice(selected.begin(),iterlist);
	cout<<selected.size()<< " events selected!..."<<endl;
	return 0;
}

//remove selected events:
int C_calendar::remove(){
	if(is_selected()==0){
		cout<<"Nothing selected!"<<endl;
		return 1;
	}
	list<list<C_event>::iterator>::iterator iter;
	iter=selected.begin();
	while(iter!=selected.end()){
		removed.push_back((**iter).showname());
		events.erase(*iter);
		++iter;
	}
	cout << removed.size()<<" Elements removed!..."<<endl;
	rmsel();
	return 0;
}

//edit selected events:
// todo: 0) general  1) times
// example: (0,"SUMM","HALLO"); (1,"DTSTART","123000")
int C_calendar::change(bool todo,string str1,string str2){
	if(str1=="")return 1;
	if(str2=="")return 1;
	cout << "Changing events..."<<endl;
	//Create regular expressions:
	string regstr;
	string ersstr;
	if(todo==0){
		regstr="(.*"+str1+"[^:]*:)(.*)";
		ersstr="$1"+str2;
	}
	if(todo==1){
		regstr="(.*"+str1+".*:[0-9]*[A-Z])[0-9]*(.*)";
		ersstr="$01"+str2+"$02";
	}
	//cout<<"ErsString: "<<ersstr<<endl;
	//cout<<"RegEXP: "<<regstr<<endl;
	regex reg(regstr);
	//use it:
	if(is_selected()==0){
		cout<<"Nothing selected!"<<endl;
		return 1;
	}
	list<list<C_event>::iterator>::iterator iter;
	iter=selected.begin();
	while(iter!=selected.end()){
		changed.push_back((*iter));
		(**iter).change(reg,ersstr);
		++iter;
	}
	cout<<"Events changed!..."<<endl;
	return 0;
}

//Save as ics-file:
bool C_calendar::save_as_ics(string fileadress){
	cout << "Save to ics-file..."<<endl;
	//filestream:
	ofstream afile(fileadress.c_str());
	//iterate through begin:
	for(vector<string>::iterator it = begin.begin(); it != begin.end(); ++it)afile<<(*it)<<endl;
	//iterate through calendar:
	list<C_event>::iterator iter;
	vector<string>* vect;
	iter=events.begin();
	while(iter!=events.end()){
		afile<<"BEGIN:VEVENT"<<endl;
		vect=(*iter).pntr_vector();
		//iterate through event
		for(vector<string>::iterator it=(*vect).begin();it!=(*vect).end();++it)afile<<(*it)<<endl;
		afile<<"END:VEVENT"<<endl;
		++iter;
	}
	//iterate through end:
	for(vector<string>::iterator it = end.begin(); it != end.end(); ++it)afile<<(*it)<<endl;
	cout << "File saved!"<<endl;
	return 0;
}


//-------------------------------------------------------------------------------------
// Subclass for CalDav:
//-------------------------------------------------------------------------------------


//Creator:
C_caldav::C_caldav(){isics=0;}

//delete tmp-files on destructor
C_caldav::~C_caldav(){
	
}

//Load CalDav-Calendar:
//the Eventfiles are getting fetched from the server with cadaver and the
//getcal.sh bash-script and are saved in /tmp/$PID/. (1 File = 1 Event)
//The Events are saved in an List of C_event in the Mainclass.
int C_caldav::load(string calname, string pid){
	url=calname;
	//set dirs:
	tmpdir="/tmp/cal-"+pid;
	icsdir=tmpdir+"/ics";
	delfile=tmpdir+"/del";
	//tmp-vars:
	string command;
        DIR *dp;
	int gefunden;
	bool isevent=0;
        string str;
        string file;
        string zeile;
	string server;
        struct dirent *dirp;

	//Check if entry for that server exists in ~/.netrc:
	file=file_in_homedir(".netrc");
	ifstream nfile(file.c_str());
	if(nfile){
		while(true){
			getline(nfile, zeile);
			gefunden=zeile.find(server_from_url(url));
			if(gefunden>=0)break;
			if(nfile.eof()){
				cout<<"Server "<<server<<" not in .netrc file!"<<endl;
				return 2;
			}
		}
	}
	else {
		cout<<".netrc-file does not exist!"<<endl;
		return 2;
	}
	//Fetch files:
	cout << "Fetching events from CalDav-Server... " << endl;
	//create tmpdir
	command="mkdir "+tmpdir;
	system(command.c_str());
        //fetch eventfiles:
	command="getcal.sh "+url+" "+tmpdir;
	system(command.c_str());
	cout << "Finished fetching.. reading files.."<<endl;

	//Read files:
        if((dp  = opendir(icsdir.c_str())) == NULL) {
                cout << "Error: ICSDIR not found!";
		return 1;
        }
	//while from file to file
        while ((dirp = readdir(dp)) != NULL) {
                str=string(dirp->d_name);
                if((str!=".")&&(str!="..")&&(str!="")){
                        file=icsdir+"/"+str;
                        ifstream ifile(file.c_str());
                        //while through file from line to line
			C_event neuevent;
			str.erase(str.end()-4,str.end());
			neuevent.setname(str);
			isevent=0;
                        while(true){
                                getline(ifile, zeile);
				gefunden=zeile.find("BEGIN:VEVENT");
				if(gefunden>=0){
					getline(ifile,zeile);
					isevent=1;}
				gefunden=zeile.find("END:VEVENT");
				if(gefunden>=0)break;
                                if(ifile.eof())break;
				if(isevent==1)neuevent.add(zeile);
                        }
			events.push_back(neuevent);
                }
        }
        closedir(dp);
	return C_calendar::finish_load();
}


bool C_caldav::save(){
	cout << "Save changes on Server!!"<<endl;
	//tmp-vars:
	string command;
	//used iterators:
	list<list<C_event>::iterator>::iterator iter;
	vector<string>* pntrvec;
	vector<string>::iterator iterv;
	//file where the commands for cadaver gets stored:
	ofstream cfile(delfile.c_str());
	//iterate through changed events:
	iter=changed.begin();
	string filename;
	while(iter!=changed.end()){
		filename=tmpdir+"/"+(**iter).showname()+".ics";
		ofstream afile(filename.c_str());
		//save eventfile:
		pntrvec=(**iter).pntr_vector();
		for(vector<string>::iterator it = begin.begin(); it != begin.end(); ++it)afile<<(*it)<<endl;
		for(vector<string>::iterator it = (*pntrvec).begin(); it != (*pntrvec).end(); ++it)afile<<(*it)<<endl;
		for(vector<string>::iterator it = end.begin(); it != end.end(); ++it)afile<<(*it)<<endl;
		//add command in command-file:
		command="put "+(**iter).showname()+".ics";
		cfile<<command<<endl;
		++iter;
	}
	//iterate through deleted events and add command:
	iterv=removed.begin();
	while(iterv!=removed.end()){
		command="delete "+(*iterv)+".ics";
		cfile<<command<<endl;
		++iterv;
	}
	//Do that shit:
	command="putcal.sh "+url+" "+tmpdir;
	system(command.c_str());
	removed.clear();
	changed.clear();
	return 0;
}


//-------------------------------------------------------------------------------------
//Subclass for ICS:
//-------------------------------------------------------------------------------------

//Creator:
C_ics::C_ics(){isics=1;}

//Destructor:
C_ics::~C_ics(){}

//Load ICS-File:
//the file gets splitted in begin, events and end.
//The beginning an the ending of the ICS-File gets saved in Subclass
//The Events are saved in an List of C_event in the Mainclass
int C_ics::load(string filename, string pid){
	adress=filename;
	//tmp-variable:
	int gefunden;
	string zeile;
	bool isevent=0;

	cout << "Reading file..."<< endl;
	ifstream ifile(adress.c_str());
	//Begin:
	begin.clear();
	while(true){
		if(ifile.eof())break;
		getline(ifile, zeile);
		gefunden=zeile.find("BEGIN:VEVENT");
		if(gefunden>=0)break;
		begin.push_back(zeile);
	}
	//Events and End:
	end.clear();
	C_event* neuevent;
	while(true){
		if(ifile.eof())break;
		gefunden=zeile.find("BEGIN:VEVENT");
		if(gefunden>=0){
			neuevent = new C_event;
			isevent=1;
			getline(ifile, zeile);
		}
		if(isevent==0)end.push_back(zeile);
		gefunden=zeile.find("END:VEVENT");
		if(gefunden>=0){
			(*neuevent).setname("uuid");
			events.push_back(*neuevent);
			delete neuevent;
			isevent=0;
		}
		if(isevent==1)(*neuevent).add(zeile);
		getline(ifile, zeile);
	}
	return C_calendar::finish_load();
}

//Save ics-file:
bool C_ics::save(){
	removed.clear();
	changed.clear();
	return C_calendar::save_as_ics(adress);
}


//-------------------------------------------------------------------------------------
//Create Calendar and return pointer:
//-------------------------------------------------------------------------------------

//this part is very rubbish :(
//pointers are created because of the type-casting (look in Macro EXECU)
C_calendar* createcal(string calname){
        C_calendar* pointer;
        bool issics=1;
        int first=calname.find("http");
        if(first>=0)issics=0;
        if(issics==1){
                static int i_ics=0;
                static C_ics caliv[2];
                C_ics cal;
                caliv[i_ics]=cal;
                //pointer=&caliv.at(caliv.size()-1);
                pointer=&caliv[i_ics];
                if(i_ics==1)i_ics=0;
                else i_ics++;
        }
        else {
                static int i_caldav=0;
                static C_caldav caldv[2];
                C_caldav cal;
                caldv[i_caldav]=cal;
                //caldv.push_back(cal);
                pointer=&caldv[i_caldav];
                //pointer=&caldv.at(caldv.size()-1);
                if(i_caldav==1)i_caldav=0;
                else i_caldav++;
        }
        return pointer;
}
