#ifndef CALEDIT_GUI_DIALOG_H
#define CALEDIT_GUI_DIALOG_H

#include <gtkmm.h>
#include <string>

#include "calclass.h"

//-----------------------------------------------------------------------------
//About Dialog: -> run with AboutDialog.m_Dialog.show()
//-----------------------------------------------------------------------------
struct AboutDialog {
	public:
		AboutDialog();
		virtual ~AboutDialog();
		int show();
	private:
		//Signal handlers:
		void on_about_dialog_response(int response_id);
		//Widgets:
		Gtk::AboutDialog m_Dialog;
};

//-----------------------------------------------------------------------------
// EnterURL Dialog: -> run with EnterURL.run()
//-----------------------------------------------------------------------------

struct EnterURL {
	public:
		EnterURL();
		virtual ~EnterURL();
		int run();
		std::string url;
		std::string username;
		std::string password;
	private:
		Gtk::Dialog dialogurl;
		Gtk::Entry entry_url;
		Gtk::Label label_url;
		Gtk::VBox vbox;
};

//-----------------------------------------------------------------------------
// EnterPWD Dialog: -> run with EnterPWD.run()
//-----------------------------------------------------------------------------

struct EnterPWD {
	public:
		EnterPWD();
		virtual ~EnterPWD();
		int run();
		void hide();
		std::string username;
		std::string password;
		bool save;
	private:
		Gtk::Dialog dialogpwd;
		Gtk::Entry entry_username;
		Gtk::Entry entry_password;
		Gtk::Label label_username;
		Gtk::Label label_password;
		Gtk::CheckButton checkbutton_save;
		Gtk::VBox vbox;
};

//-----------------------------------------------------------------------------
// LoadEvents Dialog: -> run with Wait.start() -> kill with Wait.end()
//-----------------------------------------------------------------------------

struct Wait{
	public:
		Wait();
		virtual ~Wait();
		void start();
		void end();
	private:
		Gtk::Dialog dialog;
		Gtk::VBox vbox;
		Gtk::Label label;
		Gtk::ProgressBar progressbar;
		bool update();
		sigc::connection pulsar;
};

//-----------------------------------------------------------------------------
// EditEvents Dialog: -> run with EnterURL.run()
//-----------------------------------------------------------------------------

struct EditEvents{
	public:
		EditEvents(C_calendar* cal);
		virtual ~EditEvents();
		int run();
	private:
		Gtk::Dialog dialogedit;
		C_calendar* cal1;
		int ret;
		void cbeditchanged();
		Gtk::VBox m_Box;
		Gtk::Label cblabel;
		Gtk::ComboBoxText cbedit;
		Gtk::HBox cbhbox;
		//Edit Summary
		Gtk::Label sumlabel;
		Gtk::Entry sumentry;
		Gtk::HBox sumhbox;
		//Edit Start- and Endtime
		Gtk::Label startlabel;
		Gtk::Label endlabel;
		Gtk::Label timelabel1;
		Gtk::Label timelabel2;
		Gtk::SpinButton spin_hour1;
		Gtk::SpinButton spin_hour2;
		Gtk::SpinButton spin_minute1;
		Gtk::SpinButton spin_minute2;
		Gtk::HBox startbox;
		Gtk::HBox endbox;
		Gtk::VBox timevbox;
		Gtk::HBox timehbox;
		//Pointer to currently active selection:
		Gtk::HBox* ptr_hbox;
};

#endif //CALEDIT_GUI_DIALOG_H
