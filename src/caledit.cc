#include "../config.h"
#include <iostream>
#include <string>
#include <unistd.h> //for getpid

#include "functions.h"
#include "calclass.h"

#ifdef ENABLE_GUI
#include "gui-mainwindow.h"
#endif

using namespace std;

//Prototype:
int editics(C_calendar* calendar);
int start_gui(std::string pid, int argc, char** argv);

int show_useage(){
	cout << "Useage: caledit <calendar>"<<endl;
	#ifdef ENABLE_GUI
	cout << "Useage: caledit --gui"<<endl;
	#endif
	cout << "Useage: caledit --version"<<endl;
	cout <<endl;
	cout << "caledit is able to mass-edit .ics-Files or Online-Calendars(CalDAV)"<<endl;
	cout << "Events are selected with 'sel' and can then be removed 'del' or changed 'edit'"<<endl;
	cout << "You can save an CalDav-Calender to an ics-file with 'save-ics'."<<endl;
	cout <<endl;
	cout <<"Caledit can also be used in scripts - Examples are in the README File!"<<endl;
	return 0;
}

int main(int argc, char** argv){
	//PID ermitteln und umwandeln auf string:
        int pidi=getpid();
        string pid = static_cast<ostringstream*>( &(ostringstream() << pidi) )->str();
	if(argc==1){show_useage();return 1;} //show help when no flag is given
	string input=argv[1];

	#ifdef ENABLE_GUI
	if(input=="--gui"){
		argc=1;
		return start_gui(pid,argc,argv);
	}
	#endif

	if(input=="--version"){cout<<VERSION<<endl;return 1;}
	if(argc!=2){show_useage();return 1;}
	string* anfang= new string(input,0,1);
	if((*anfang)=="-"){show_useage();return 1;}
	delete anfang;

	int ret; //Return-Value

	C_calendar* cal1 = createcal(input);
	EXECU(ret,cal1,load(input,pid))
	if(ret!=0)return ret;

	return editics(cal1);
}

//CLI um Kalender zu bearbeiten:
int editics(C_calendar* calendar){
	//tmp-Variablen:
	string auswahl;
	string astr,sstr;
	string start,end;
	int ret;

	cout << "Be aware that this application is just for mass-editing events!"<<endl;
	cout << "It's not very effective in just editing single events, for that"<<endl;
	cout << "you should use something like Thunderbird/Icedove!"<<endl;
	cout << "--Enter help for help ;)"<<endl;
	cout << "------------------------------------------------------------------------"<<endl;

	while(true){
	cout << "cal>";
	cin >> auswahl;
	//tmp-Variablen zurücksetzen:
	sstr="";
	astr="";
	start="";
	end="";
	if(auswahl=="sel"){
		cout << "What whould you like to select:"<<endl;
		cout << "1)Event-Description"<<endl;
		cout << "2)Exact Start- and Endtime (hhmmss)"<<endl;
		cout << "3)Everything after an specific Date"<<endl;
		cout << "4)Everything before an specific Date"<<endl;
		cout << "5)Event-UID"<<endl;
		cout << "cal/sel>";
		int aw;
		cin >> aw;
		if(aw==1){
			cout << "Searchterm: "<<endl;
			cin >> sstr;
			//getline(cin,sstr);
			EXECU(ret,calendar,select(0,"SUMMARY",sstr))
		}
		else if(aw==2){
			cout << "Starttime (hhmmss): ";
			cin >> start;
			if(start.size()!=6){cout<<"Wrong Timeformat!"<<endl;break;}
			cout << "Endtime (hhmmss): ";
			cin >> end;
			if(end.size()!=6){cout<<"Wrong Timeformat!"<<endl;break;}
			EXECU(ret,calendar,select(3,start,end))
		}
		else if(aw==3){
			cout << "Date (YYYYMMDD): ";
			cin >> sstr;
			cout << "Time (HHMMSS): ";
			cin >> astr;
			if((sstr.size()!=8)||(astr.size()!=6)){cout<<"Wrong Format!"<<endl;break;}
			EXECU(ret,calendar,select(1,sstr,astr))
		}
		else if(aw==4){
			cout << "Date (YYYYMMDD): ";
			cin >> sstr;
			cout << "Time (HHMMSS): ";
			cin >> astr;
			if((sstr.size()!=8)||(astr.size()!=6)){cout<<"Wrong Format!"<<endl;break;}
			EXECU(ret,calendar,select(2,sstr,astr))
		}
		else if(aw==5){
			cout << "Searchterm: "<<endl;
			cin >> sstr;
			//getline(cin,sstr);
			EXECU(ret,calendar,select(0,"UID",sstr))
		}
		else{cout << "Command not recognized!"<<endl;break;}
	}
	else if(auswahl=="ls"){
		EXECU(ret,calendar,show_selected())
	}
	else if(auswahl=="del"){
		EXECU(ret,calendar,remove())
	}
	else if(auswahl=="mod"){
		cout << "What should get modified?"<<endl;
		cout << "1)Event-Description"<<endl;
		cout << "2)Start- and Endtime (hhmmss)"<<endl;
		cout << "cal/mod>";
		int aw;
		cin >> aw;
		if(aw==1){
			cout<<"Enter new description:"<<endl;
			getline(cin,sstr);//WARUM ZWEIMAL?
			getline(cin,sstr);
			EXECU(ret,calendar,change(0,"SUMMARY",sstr))
			EXECU(ret,calendar,rmsel())
		}
		else if(aw==2){
			cout<<"New start time:"<<endl;
			cin>>start;
			if(start.size()!=6){cout<<"Wrong time format!"<<endl;break;}
			cout<<"New end time:"<<endl;
			cin>>end;
			if(end.size()!=6){cout<<"Wrong time format!"<<endl;break;}
			EXECU(ret,calendar,change(1,"DTSTART",start))
			EXECU(ret,calendar,change(1,"DTEND",end))
			EXECU(ret,calendar,rmsel())
		}
		else{cout << "Command not recognized!"<<endl;break;}
	}
	else if(auswahl=="rel"){
		cout << "Canceled selection!"<<endl;
		EXECU(ret,calendar,rmsel())
	}
	else if(auswahl=="save"){
		EXECU(ret,calendar,save())
	}
	else if(auswahl=="help"){
		cout << "sel: Select events" <<endl;
		cout << "ls: Show selected events" <<endl;
		cout << "rel: Cancel selection" <<endl;
		cout << "del: Delete selected events" <<endl;
		cout << "mod: Modify selected events" <<endl;
		cout << "help: Show help" <<endl;
		cout << "save: Save changes" <<endl;
		cout << "exit: Exit" <<endl;}
	else if(auswahl=="exit"){
		EXECU(ret,calendar,is_changed())
		if(ret==1){
			cout << "Not saved changes will be lost!" <<endl;
			cout << "Are you sure? (y/n)" <<endl;
			cin >> sstr;
			if(sstr=="n"){continue;}
		}
		cout<<"bye"<<endl;
		return 0;
	}
	else if(auswahl!=""){cout << "Command " << auswahl<< " not found!"<<endl;}
	}
}

#ifdef ENABLE_GUI
//Start GUI
int start_gui(std::string pid, int argc, char** argv){
	std::cout << "Starting GUI..."<<std::endl;
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "uriesk.caledit.rulez");
	ExampleWindow window(pid);

	//Shows the window and returns when it is closed.
	return app->run(window);
}
#endif
