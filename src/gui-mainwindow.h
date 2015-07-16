#ifndef CALEDIT_GUI_H
#define CALEDIT_GUI_H

#include <gtkmm.h>
#include <string>

#include "calclass.h"
#include "gui-dialog.h"

class ExampleWindow : public Gtk::Window{
	public:
		ExampleWindow(std::string pid);
		virtual ~ExampleWindow();

	protected:
		virtual void register_stock_items();
		virtual void add_stock_item(const Glib::RefPtr<Gtk::IconFactory> & factory,const std::string& filepath,const Glib::ustring& id, const Glib::ustring& label);
		std::list<std::list<C_event>::iterator> liststore_to_iterlist(bool x);
		void disableelements();
		void enableelements();
		//For Calendar:
		std::string pid;
		C_calendar* cal1;
		void update_ui();
		bool file_loaded;
		int ret;

		//Columns for Eventlist:
		class Columns:public Gtk::TreeModel::ColumnRecord{
			public:
				Columns(){
					add(col_x);
					add(col_uuid);
					add(col_summary);
					add(col_start);
					add(col_end);
					add(col_iter);
				}
				~Columns() {}
				Gtk::TreeModelColumn<bool> col_x;
				Gtk::TreeModelColumn<Glib::ustring> col_uuid;
				Gtk::TreeModelColumn<Glib::ustring> col_summary;
				Gtk::TreeModelColumn<Glib::ustring> col_start;
				Gtk::TreeModelColumn<Glib::ustring> col_end;
				Gtk::TreeModelColumn<std::list<C_event>::iterator> col_iter;
		};

		//Filter Summary:
		class HBoxSummary:public Gtk::HBox{
			public:
				HBoxSummary(){
					m_label.set_markup("contains");
					button.set_label("Filter");
					pack_start(m_label,Gtk::PACK_SHRINK);
					pack_start(entry);
					pack_end(button,Gtk::PACK_SHRINK);
				}
				~HBoxSummary(){};
				Gtk::Entry entry;
				Gtk::Button button;
			private:
				Gtk::Label m_label;
		};

		//Filter UID:
		class HBoxUID:public Gtk::HBox{
			public:
				HBoxUID(){
					m_label.set_markup("contains");
					button.set_label("Filter");
					pack_start(m_label,Gtk::PACK_SHRINK);
					pack_start(entry);
					pack_end(button,Gtk::PACK_SHRINK);
				}
				~HBoxUID(){}
				Gtk::Entry entry;
				Gtk::Button button;
			private:
				Gtk::Label m_label;
		};

		//Filter Date:
		class HBoxDate:public Gtk::HBox{
			public:
				HBoxDate(){
					combobox.append(">");
					combobox.append("=");
					combobox.append("<");
					combobox.set_active_text(">");
					pack_start(combobox,Gtk::PACK_SHRINK);
					Glib::RefPtr<Gtk::Adjustment> spin_day=Gtk::Adjustment::create(1,1,31,1,1,0);
					Glib::RefPtr<Gtk::Adjustment> spin_month=Gtk::Adjustment::create(1,1,12,1,1,0);
					Glib::RefPtr<Gtk::Adjustment> spin_year=Gtk::Adjustment::create(2014,1950,2050,1,1,0);
					day.set_adjustment(spin_day);
					month.set_adjustment(spin_month);
					year.set_adjustment(spin_year);
					day.set_value(31);
					month.set_value(5);
					year.set_value(2014);
					m_label1.set_markup(".");
					m_label2.set_markup(".");
					pack_start(day);
					pack_start(m_label1,Gtk::PACK_SHRINK);
					pack_start(month);
					pack_start(m_label2,Gtk::PACK_SHRINK);
					pack_start(year);
					button.set_label("Filter");
					pack_end(button,Gtk::PACK_SHRINK);
				}
				~HBoxDate(){}
				Gtk::SpinButton day;
				Gtk::SpinButton month;
				Gtk::SpinButton year;
				Gtk::Button button;
				Gtk::ComboBoxText combobox;
			private:
				Gtk::Label m_label1;
				Gtk::Label m_label2;
		};

		//Filter Time:
		class HBoxTime:public Gtk::HBox{
			public:
				HBoxTime(){
					Glib::RefPtr<Gtk::Adjustment> spin_minute1=Gtk::Adjustment::create(1,0,59,1,1);
					Glib::RefPtr<Gtk::Adjustment> spin_hour1=Gtk::Adjustment::create(1,0,23,1,1);
					Glib::RefPtr<Gtk::Adjustment> spin_minute2=Gtk::Adjustment::create(1,0,59,1,1);
					Glib::RefPtr<Gtk::Adjustment> spin_hour2=Gtk::Adjustment::create(1,0,23,1,1);
					hour1.set_adjustment(spin_hour1);
					hour2.set_adjustment(spin_hour2);
					minute1.set_adjustment(spin_minute1);
					minute2.set_adjustment(spin_minute2);
					hour1.set_value(12);
					hour2.set_value(13);
					minute1.set_value(0);
					minute2.set_value(30);
					m_label1.set_markup(":");
					m_label2.set_markup("-");
					m_label3.set_markup(":");
					button.set_label("Filter");
					pack_start(hour1);
					pack_start(m_label1);
					pack_start(minute1);
					pack_start(m_label2);
					pack_start(hour2);
					pack_start(m_label3);
					pack_start(minute2);
					pack_end(button,Gtk::PACK_SHRINK);
				}
				~HBoxTime(){}
				Gtk::SpinButton hour1;
				Gtk::SpinButton hour2;
				Gtk::SpinButton minute1;
				Gtk::SpinButton minute2;
				Gtk::Button button;
			private:
				Gtk::Label m_label1;
				Gtk::Label m_label2;
				Gtk::Label m_label3;
		};

		//Signal handlers:
		virtual void on_action_file_new();
		virtual void on_action_file_quit();
		virtual void on_action_openics();
		virtual void on_action_openurl();
		virtual void on_action_file_save();
		virtual void on_action_others();
		virtual void on_action_about();
		void buttonclicked_summary();
		void buttonclicked_uuid();
		void buttonclicked_date();
		void buttonclicked_time();
		void buttonclicked_delsel();
		void buttonclicked_edit();
		void buttonclicked_remsel();
		void comboboxchanged();

		//Child widgets:
		Gtk::VBox m_Box;
		Glib::RefPtr<Gtk::UIManager> m_refUIManager;
		Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
		//Für Eventlist:
		Gtk::Label m_label_eventlist;
		Gtk::ScrolledWindow scrollWindow;
		Gtk::TreeView treeView;
		Glib::RefPtr<Gtk::ListStore> listStore;
		Columns cols;
		//Filter Events:
		Gtk::ComboBoxText m_combobox;
		Gtk::Label m_label_filter;
		Gtk::HBox m_hbox_filter;
		Gtk::HBox* m_refhbox;
		Gtk::HBox m_hbox_place;
		//Buttons and Statusbar:
		Gtk::HButtonBox m_buttonbox;
		Gtk::Button m_button_delsel;
		Gtk::Button m_button_edit;
		Gtk::Button m_button_remsel;
		Gtk::Statusbar m_statusbar;
		//Dialogs and Custom Stuff:
		AboutDialog aboutd;
		HBoxSummary m_hbox_summary;
		HBoxUID m_hbox_uuid;
		HBoxDate m_hbox_date;
		HBoxTime m_hbox_time;
};

#endif //CALEDIT_GUI_H
