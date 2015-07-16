#include <string>

//Prototypes:
std::string convert_datetime(std::string input);
std::string convert_time(int hour,int minute);
std::string convert_date(int year,int month,int day);
std::string server_from_url(std::string url);
std::string file_in_homedir(std::string filename);
int new_netrc_entry(std::string server,std::string username,std::string password);
int remove_netrc_entry(std::string server);
