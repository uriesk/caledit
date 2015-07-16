#include <iostream>
#include <sys/types.h>//both used ->
#include <pwd.h>      //for homedir-detection
#include <fstream> //file-access
#include <unistd.h> //for getuid()
#include <regex> //Regular Expressions

#include "functions.h"
#include "calclass.h"

using namespace std;

//Stringconversions (Date/Time/etc.):

//Converts YYYYMMDDhhmmss to YYYY.MM.DD-hh:mm
std::string convert_datetime(std::string input){
	input.pop_back();
	input.pop_back();
	input.insert(4,".");
	input.insert(7,".");
	input.insert(10,"-");
	input.insert(13,":");
	return input;
}

//Converts hh and mm to hhmmss
std::string convert_time(int hour,int minute){
	//int to string
	std::string str_hour = static_cast<ostringstream*>( &(ostringstream() << hour) )->str();
	std::string str_minute = static_cast<ostringstream*>( &(ostringstream() << minute) )->str();
	if(hour<10) str_hour="0"+str_hour;
	if(minute<10) str_minute="0"+str_minute;
	return str_hour+str_minute+"00";
}

//Converts YYYY, MM and DD to YYYYMMDD
std::string convert_date(int year,int month,int day){
	//int to string
	std::string str_year = static_cast<ostringstream*>( &(ostringstream() << year) )->str();
	std::string str_month = static_cast<ostringstream*>( &(ostringstream() << month) )->str();
	std::string str_day = static_cast<ostringstream*>( &(ostringstream() << day) )->str();
	if(month<10) str_month="0"+str_month;
	if(day<10) str_day="0"+str_day;
	return str_year+str_month+str_day;
}

//Some useful stuff:

//Get serveraddress out of URL
std::string server_from_url(std::string url){
	regex rurl("http:..([^/]*).*");
	regex rurls("https:..([^/]*).*");
	std::string server;
	server=regex_replace(url,rurl,"$1",regex_constants::format_no_copy);
	if(server.size()==0)server=regex_replace(url,rurls,"$1",regex_constants::format_no_copy);
	return server;
}

//Get full path of file in homedir
std::string file_in_homedir(std::string filename){
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string file=(string)pw->pw_dir+"/"+filename;
	return file;
}

//Add something to .netrc-file
int new_netrc_entry(std::string server,std::string username,std::string password){
	std::string file=file_in_homedir(".netrc");
	ofstream ofile(file.c_str(),ios::app);
	ofile<<"machine "<<server<<endl;
	ofile<<"\tlogin "<<username<<endl;
	ofile<<"\tpassword "<<password<<endl;
	return 0;
	cout<<"SAVED IN NETRC"<<endl;
}

//Remove something from .netrc-file
int remove_netrc_entry(std::string server){
	std::string zeile;
	std::string file=file_in_homedir(".netrc");
	std::string tmpfile=file_in_homedir(".netrc_tmp");
	ifstream ifile(file.c_str());
	ofstream ofile(tmpfile.c_str());
	while(true){
		getline(ifile, zeile);
		if(zeile=="machine "+server){
			getline(ifile, zeile);
			getline(ifile, zeile);
			getline(ifile, zeile);
		}
		if(ifile.eof())break;
		ofile<<zeile<<endl;
	}
	std::string command="mv "+tmpfile+" "+file;
	system(command.c_str());
	return 0;
}
