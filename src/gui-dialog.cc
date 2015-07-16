#include <iostream>

#include "gui-dialog.h"
#include "functions.h"

using namespace std;

//-----------------------------------------------------------------------------
//About Dialog: -> run with AboutDialog.m_Dialog.show()
//-----------------------------------------------------------------------------

AboutDialog::AboutDialog(){
	m_Dialog.set_program_name("Caledit");
	m_Dialog.set_version("0.9");
	m_Dialog.set_copyright("Andreas Gattringer");
	m_Dialog.set_comments("Mass-edit ICS-Files and CalDAV-Calendars.");
	m_Dialog.set_license("GPLv2");

	m_Dialog.set_website("http://uriesk.wordpress.com");
	m_Dialog.set_website_label("uriesks blog");

	std::vector<Glib::ustring> list_authors;
	list_authors.push_back("uriesk aka Andreas Gattringer");
	m_Dialog.set_authors(list_authors);

	m_Dialog.signal_response().connect(sigc::mem_fun(*this, &AboutDialog::on_about_dialog_response));
}

AboutDialog::~AboutDialog(){}

int AboutDialog::show(){m_Dialog.show();m_Dialog.present();}

void AboutDialog::on_about_dialog_response(int response_id){
	if((response_id == Gtk::RESPONSE_CLOSE) || (response_id == Gtk::RESPONSE_CANCEL) ){
		m_Dialog.hide();
	}
}

//-----------------------------------------------------------------------------
// EnterURL Dialog: -> run with EnterURL.run()
//-----------------------------------------------------------------------------

EnterURL::EnterURL():vbox(false,6){
	dialogurl.add_button(Gtk::StockID(Gtk::Stock::OK), Gtk::RESPONSE_OK);
	dialogurl.add_button(Gtk::StockID(Gtk::Stock::CANCEL), Gtk::RESPONSE_CANCEL);
	dialogurl.set_title("CalDAV");

	label_url.set_markup("Enter CalDav-URL:");
	vbox.pack_start(label_url);
	entry_url.set_text("http://");
	vbox.pack_start(entry_url);

	dialogurl.get_content_area()->add(vbox);
	dialogurl.show_all();
}

EnterURL::~EnterURL(){}

int EnterURL::run(){
	switch(dialogurl.run()){
		case Gtk::RESPONSE_OK:
			url=entry_url.get_text();
			dialogurl.hide();
			return 0;
		default:
			return 1;
	}
}

//-----------------------------------------------------------------------------
// EnterPWD Dialog: -> run with EnterPWD.run()
//-----------------------------------------------------------------------------

EnterPWD::EnterPWD():vbox(false,6){
	dialogpwd.add_button(Gtk::StockID(Gtk::Stock::OK), Gtk::RESPONSE_OK);
	dialogpwd.add_button(Gtk::StockID(Gtk::Stock::CANCEL), Gtk::RESPONSE_CANCEL);
	dialogpwd.set_title("CalDAV");

	label_username.set_markup("User:");
	label_password.set_markup("Password:");
	checkbutton_save.set_label("Remember Password");
	entry_password.set_visibility(false);
	entry_password.set_invisible_char('*');
	vbox.pack_start(label_username);
	vbox.pack_start(entry_username);
	vbox.pack_start(label_password);
	vbox.pack_start(entry_password);
	vbox.pack_start(checkbutton_save);

	dialogpwd.get_content_area()->add(vbox);
	dialogpwd.show_all();
}

EnterPWD::~EnterPWD(){}

int EnterPWD::run(){
	switch(dialogpwd.run()){
		case Gtk::RESPONSE_OK:
			username=entry_username.get_text();
			password=entry_password.get_text();
			save=checkbutton_save.get_active();
			dialogpwd.hide();
			return 0;
		default:
			return 1;
	}
}

void EnterPWD::hide(){
	dialogpwd.hide();
}

//-----------------------------------------------------------------------------
// LoadEvents Dialog: -> run with Wait.start() -> kill with Wait.end()
//-----------------------------------------------------------------------------

Wait::Wait():vbox(false,6){
	//actualisation of Progressbar every 0,5s
	pulsar=Glib::signal_timeout().connect(sigc::mem_fun(*this,&Wait::update),500);
	//usual stuff
	dialog.set_title("waiting...");
	label.set_markup("Downloading Eventfiles...");
	vbox.pack_start(label);
	vbox.pack_start(progressbar);
	dialog.get_content_area()->add(vbox);
	dialog.set_modal(false);
}

Wait::~Wait(){
	pulsar.disconnect();
}

void Wait::start(){
	dialog.show_all();
}

void Wait::end(){
	dialog.hide();
}

bool Wait::update(){
	progressbar.pulse();
	return true;
}

//-----------------------------------------------------------------------------
// EditEvents Dialog: -> run with EnterURL.run()
//-----------------------------------------------------------------------------

EditEvents::EditEvents(C_calendar* cal):m_Box(false,5){
	cal1=cal;
	dialogedit.add_button(Gtk::StockID(Gtk::Stock::OK), Gtk::RESPONSE_OK);
	dialogedit.add_button(Gtk::StockID(Gtk::Stock::CANCEL), Gtk::RESPONSE_CANCEL);
	dialogedit.set_title("Edit Events");

	cblabel.set_markup("What to edit: ");
	cbedit.append("Summary");
	cbedit.append("Start- and Endtime");
	cbedit.set_active_text("Summary");
	cbhbox.pack_start(cblabel);
	cbhbox.pack_start(cbedit);

	//--- Just one is active - pntr_hbox points to that one
	//Edit Summary
	sumlabel.set_markup("to");
	sumhbox.pack_start(sumlabel,Gtk::PACK_SHRINK);
	sumhbox.pack_start(sumentry);
	//Edit Times
	startlabel.set_markup("Starttime:");
	endlabel.set_markup("Endtime:");
	timelabel1.set_markup(":");
	timelabel2.set_markup(":");
	Glib::RefPtr<Gtk::Adjustment> ad_minute1=Gtk::Adjustment::create(1,0,59,1,1);
	Glib::RefPtr<Gtk::Adjustment> ad_hour1=Gtk::Adjustment::create(1,0,23,1,1);
	Glib::RefPtr<Gtk::Adjustment> ad_minute2=Gtk::Adjustment::create(1,0,59,1,1);
	Glib::RefPtr<Gtk::Adjustment> ad_hour2=Gtk::Adjustment::create(1,0,23,1,1);
	spin_hour1.set_adjustment(ad_hour1);
	spin_hour2.set_adjustment(ad_hour2);
	spin_minute1.set_adjustment(ad_minute1);
	spin_minute2.set_adjustment(ad_minute2);
	spin_hour1.set_value(12);
	spin_hour2.set_value(13);
	spin_minute1.set_value(0);
	spin_minute2.set_value(30);
	startbox.pack_start(spin_hour1);
	startbox.pack_start(timelabel1);
	startbox.pack_start(spin_minute1);
	endbox.pack_start(spin_hour2);
	endbox.pack_start(timelabel2);
	endbox.pack_start(spin_minute2);
	timevbox.pack_start(startlabel);
	timevbox.pack_start(startbox);
	timevbox.pack_start(endlabel);
	timevbox.pack_start(endbox);
	timehbox.pack_start(timevbox);
	//---

	//Signals:
	cbedit.signal_changed().connect(sigc::mem_fun(*this,&EditEvents::cbeditchanged));

	m_Box.pack_start(cbhbox);
	m_Box.pack_start(sumhbox);
	ptr_hbox=&sumhbox;

	dialogedit.get_content_area()->add(m_Box);
	dialogedit.show_all();
}

EditEvents::~EditEvents(){}

void EditEvents::cbeditchanged(){
	if(cbedit.get_active_text()=="Summary"){
		m_Box.remove(*ptr_hbox);
		m_Box.pack_start(sumhbox);
		ptr_hbox=&sumhbox;
	}
	if(cbedit.get_active_text()=="Start- and Endtime"){
		m_Box.remove(*ptr_hbox);
		m_Box.pack_start(timehbox);
		ptr_hbox=&timehbox;
	}
	dialogedit.show_all();
}

int EditEvents::run(){
	switch(dialogedit.run()){
		case Gtk::RESPONSE_OK:
			if(cbedit.get_active_text()=="Summary"){
				std::string sumstr= sumentry.get_text();
				std::cout<<"Change Summery to "<<sumstr<<std::endl;
				EXECU(ret,cal1,change(0,"SUMM",sumstr))
			}
			if(cbedit.get_active_text()=="Start- and Endtime"){
				std::string starttime=convert_time(spin_hour1.get_value_as_int(),spin_minute1.get_value_as_int());
				std::string endtime=convert_time(spin_hour2.get_value_as_int(),spin_minute2.get_value_as_int());
				std::cout<<"Change times to "<<starttime<<" - "<<endtime<<std::endl;
				EXECU(ret,cal1,change(1,"DTSTART",starttime))
				EXECU(ret,cal1,change(1,"DTEND",endtime))
			}
			dialogedit.hide();
			return ret;
			break;
		default:
			return 0;
	}
}
