#include <iostream>
#include <string>

#include "functions.h"
#include "calclass.h"
#include "gui-mainwindow.h"

using namespace std;

ExampleWindow::ExampleWindow(string pidget):Gtk::Window(),m_hbox_filter(false,3){
	set_title("Caledit");
	set_default_size(600, 400);
	file_loaded=0;
	pid = pidget;

	add(m_Box);

	//Signals:
	m_combobox.signal_changed().connect(sigc::mem_fun(*this,&ExampleWindow::comboboxchanged));
	m_hbox_summary.button.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_summary));
	m_hbox_uuid.button.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_uuid));
	m_hbox_date.button.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_date));
	m_hbox_time.button.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_time));
	m_button_delsel.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_delsel));
	m_button_edit.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_edit));
	m_button_remsel.signal_clicked().connect(sigc::mem_fun(*this,&ExampleWindow::buttonclicked_remsel));

	//Define the actions for the Menubar and Toolbar:
	m_refActionGroup = Gtk::ActionGroup::create();
	m_refActionGroup->add( Gtk::Action::create("MenuFile", "_File") );
	m_refActionGroup->add( Gtk::Action::create("openics", Gtk::Stock::OPEN),sigc::mem_fun(*this, &ExampleWindow::on_action_openics) );
	m_refActionGroup->add( Gtk::Action::create("openurl", Gtk::Stock::CONNECT),sigc::mem_fun(*this, &ExampleWindow::on_action_openurl) );
	m_refActionGroup->add( Gtk::Action::create("save", Gtk::Stock::SAVE),sigc::mem_fun(*this, &ExampleWindow::on_action_file_save) );
	m_refActionGroup->add( Gtk::Action::create("quit", Gtk::Stock::QUIT),sigc::mem_fun(*this, &ExampleWindow::on_action_file_quit) );
	m_refActionGroup->add( Gtk::Action::create("MenuEdit", "_Edit") );
	m_refActionGroup->add( Gtk::Action::create("synchronize", "_Synchronize"),sigc::mem_fun(*this, &ExampleWindow::on_action_others) );
	m_refActionGroup->add( Gtk::Action::create("MenuHelp", Gtk::Stock::HELP) );
	m_refActionGroup->add( Gtk::Action::create("about", Gtk::Stock::ABOUT),sigc::mem_fun(*this, &ExampleWindow::on_action_about) );

	//Define how the actions are presented in the menus and toolbars:
	Glib::RefPtr<Gtk::UIManager> m_refUIManager = Gtk::UIManager::create();
	m_refUIManager->insert_action_group(m_refActionGroup);
	add_accel_group(m_refUIManager->get_accel_group());
	//Layout the actions in a menubar and toolbar:
	try{
		Glib::ustring ui_info =
		"<ui>"
		"  <menubar name='MenuBar'>"
		"    <menu action='MenuFile'>"
		"      <menuitem action='openics'/>"
		"      <menuitem action='openurl'/>"
		"      <menuitem action='save'/>"
		"      <separator/>"
		"      <menuitem action='quit'/>"
		"    </menu>"
		"    <menu action='MenuEdit'>"
		"      <menuitem action='synchronize'/>"
		"    </menu>"
		"    <menu action='MenuHelp'>"
		"      <menuitem action='about'/>"
		"    </menu>"
		"  </menubar>"
		"  <toolbar  name='ToolBar'>"
		"    <toolitem action='openics'/>"
		"    <toolitem action='openurl'/>"
		"    <toolitem action='save'/>"
		"    <toolitem action='about'/>"
		"    <toolitem action='quit'/>"
		"  </toolbar>"
		"</ui>";
		m_refUIManager->add_ui_from_string(ui_info);
	}catch(const Glib::Error& ex){std::cerr << "building menus and toolbars failed: " <<  ex.what();}
	//Add the MenuBar to the window:
	Gtk::Widget* pMenuBar = m_refUIManager->get_widget("/MenuBar") ;
	m_Box.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
	//Add the ToolBar to the window:
	Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
	m_Box.pack_start(*pToolbar, Gtk::PACK_SHRINK);

	//Eventlist:
	m_label_eventlist.set_markup("No File loaded... / No Connection to CalDAV-Server...");
	treeView.set_model(listStore=Gtk::ListStore::create(cols));
	treeView.append_column_editable("",cols.col_x);
	treeView.append_column("UID",cols.col_uuid);
	treeView.append_column("Summary",cols.col_summary);
	treeView.append_column("Starttime",cols.col_start);
	treeView.append_column("Endtime",cols.col_end);
	for(int n=0;n<5;n++){
		treeView.get_column(n)->set_sizing(Gtk::TREE_VIEW_COLUMN_GROW_ONLY);
		treeView.get_column(n)->set_expand(false);
		if(n==2)n++;
	}
	treeView.get_column(2)->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
	treeView.get_column(2)->set_min_width(200);
	treeView.get_column(2)->set_expand(true);
	treeView.get_selection()->set_mode(Gtk::SELECTION_NONE);
	scrollWindow.add(treeView);
	scrollWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
	m_Box.pack_start(m_label_eventlist,Gtk::PACK_SHRINK);
	m_Box.pack_start(scrollWindow);

	//Filter Items:
	m_label_filter.set_markup("Add Filter:");
	m_combobox.append("Summary");
	m_combobox.append("Date");
	m_combobox.append("Start- and Endtime");
	m_combobox.append("UID");
	m_hbox_filter.pack_start(m_label_filter,Gtk::PACK_SHRINK);
	m_hbox_filter.pack_start(m_combobox,Gtk::PACK_SHRINK);
	//m_refhbox is going to be replaced, when combobox changes:
	m_hbox_filter.pack_start(m_hbox_place,Gtk::PACK_EXPAND_WIDGET);
	m_refhbox = &m_hbox_place;
	m_Box.pack_start(m_hbox_filter,Gtk::PACK_SHRINK);
	//Placer:
	Gtk::Alignment *al = Gtk::manage(new Gtk::Alignment());
	al->set_size_request(2,15);
	m_Box.pack_start(*al,false,true);
	//ButtonBox
	m_button_delsel.add_label("Delete\nSelected Events");
	m_button_edit.set_label("Edit\nSelected Events");
	m_button_remsel.set_label("Reset Filter\nand Selection");
	m_buttonbox.set_layout(Gtk::BUTTONBOX_EDGE);
	m_buttonbox.set_spacing(30);
	m_buttonbox.pack_start(m_button_delsel);
	m_buttonbox.pack_start(m_button_edit);
	m_buttonbox.pack_start(m_button_remsel);
	m_Box.pack_start(m_buttonbox,Gtk::PACK_SHRINK);
	//Statusbar
	m_Box.pack_end(m_statusbar,Gtk::PACK_SHRINK);

	disableelements();
	show_all_children();
}

ExampleWindow::~ExampleWindow(){}

void ExampleWindow::disableelements(){
	m_button_delsel.set_sensitive(false);
	m_button_edit.set_sensitive(false);
	m_button_remsel.set_sensitive(false);
	m_combobox.set_sensitive(false);
	m_label_filter.set_sensitive(false);
	treeView.set_sensitive(false);
}
void ExampleWindow::enableelements(){
	m_button_delsel.set_sensitive(true);
	m_button_edit.set_sensitive(true);
	m_button_remsel.set_sensitive(true);
	m_combobox.set_sensitive(true);
	m_label_filter.set_sensitive(true);
	treeView.set_sensitive(true);
}

void ExampleWindow::buttonclicked_summary(){
	std::string searchstr = m_hbox_summary.entry.get_text();
	EXECU(ret,cal1,select(0,"SUMMARY",searchstr))
	if(ret!=0){
		Gtk::MessageDialog dia(*this,"No matching events");
		dia.run();
	  }
	m_combobox.set_active_text("");
	update_ui();
}

void ExampleWindow::buttonclicked_uuid(){
	std::string searchstr = m_hbox_uuid.entry.get_text();
	EXECU(ret,cal1,select(0,"UID",searchstr))
	if(ret!=0){
		Gtk::MessageDialog dia(*this,"No matching events");
		dia.run();
	}
	m_combobox.set_active_text("");
	update_ui();
}

void ExampleWindow::buttonclicked_date(){
	std::string sel_date=convert_date(m_hbox_date.year.get_value_as_int(),m_hbox_date.month.get_value_as_int(),m_hbox_date.day.get_value_as_int());
	if(m_hbox_date.combobox.get_active_text()==">"){
		EXECU(ret,cal1,select(1,sel_date,"000000"))
	}
	if(m_hbox_date.combobox.get_active_text()=="<"){
		EXECU(ret,cal1,select(2,sel_date,"000000"))
	}
	if(m_hbox_date.combobox.get_active_text()=="="){
		EXECU(ret,cal1,select(1,sel_date,"000000"))
		EXECU(ret,cal1,select(2,sel_date,"000000"))
	}
	if(ret!=0){
		Gtk::MessageDialog dia(*this,"No matching events");
		dia.run();
	}
	m_combobox.set_active_text("");
	update_ui();
}

void ExampleWindow::buttonclicked_time(){
	std::string starttime=convert_time(m_hbox_time.hour1.get_value_as_int(),m_hbox_time.minute1.get_value_as_int());
	std::string endtime=convert_time(m_hbox_time.hour2.get_value_as_int(),m_hbox_time.minute2.get_value_as_int());
	EXECU(ret,cal1,select(3,starttime,endtime))
	if(ret!=0){
		Gtk::MessageDialog dia(*this,"No matching events");
		dia.run();
	}
	m_combobox.set_active_text("");
	update_ui();
}

void ExampleWindow::buttonclicked_delsel(){
	list<list<C_event>::iterator> dellist;
	list<list<C_event>::iterator> omlist;
	//Selected events:
	dellist=liststore_to_iterlist(1);
	//Not selected, but filtered events:
	omlist=liststore_to_iterlist(0);
	//filtered events:
	list<list<C_event>::iterator>* filtered;
	EXECU(filtered,cal1,pntr_selected())
	//check some things:
	if(dellist.size()==0){
		Gtk::MessageDialog dia(*this,"Nothing selected!");
		dia.run();
		return;
	  }
	EXECU(ret,cal1,is_selected())
	if((ret==0)&((*filtered).size()==dellist.size())){
		Gtk::MessageDialog dia(*this,"Everything selected!");
		dia.set_secondary_text("i won't do that, bro");
		dia.run();
		return;
	}
	//override C_calender-list with selected-list and remove events:
	(*filtered)=dellist;
	EXECU(ret,cal1,remove())
	if(omlist.size()!=0)(*filtered)=omlist;
	else {
		EXECU(ret,cal1,rmsel())
		listStore->clear();
	}
	update_ui();
}

void ExampleWindow::buttonclicked_edit(){
	list<list<C_event>::iterator> editlist;
	list<list<C_event>::iterator> tmplist;
	//Selected events:
	editlist=liststore_to_iterlist(1);
	//filtered events:
	list<list<C_event>::iterator>* filtered;
	EXECU(filtered,cal1,pntr_selected())
	//check some things:
	if(editlist.size()==0){
		Gtk::MessageDialog dia(*this,"Nothing selected!");
		dia.run();
		return;
	}
	EXECU(ret,cal1,is_selected())
	if((ret==0)&((*filtered).size()==editlist.size())){
		Gtk::MessageDialog dia(*this,"Everything selected!");
		dia.set_secondary_text("i won't do that, bro");
		dia.run();
		return;
	}
	//override list in C_calender with selected-list and EditEvents
	tmplist=(*filtered);
	(*filtered)=editlist;
	EditEvents ee(cal1);
	ret=ee.run();
	if(ret!=0){
		Gtk::MessageDialog dia(*this,"Input invalid!");
		dia.run();
	}
	(*filtered)=tmplist;
	update_ui();
}

void ExampleWindow::buttonclicked_remsel(){
	EXECU(ret,cal1,rmsel())
	listStore->clear();
	update_ui();
}

void ExampleWindow::comboboxchanged(){
	if(m_combobox.get_active_text()==""){
		m_hbox_filter.remove(*m_refhbox);
		m_hbox_filter.pack_start(m_hbox_place);
		m_hbox_filter.reorder_child(m_hbox_place,5);
		m_refhbox=&m_hbox_place;
	}
	if(m_combobox.get_active_text()=="Summary"){
		m_hbox_filter.remove(*m_refhbox);
		m_hbox_filter.pack_start(m_hbox_summary);
		m_hbox_filter.reorder_child(m_hbox_summary,5);
		m_refhbox=&m_hbox_summary;
	}
	else if(m_combobox.get_active_text()=="UID"){
		m_hbox_filter.remove(*m_refhbox);
		m_hbox_filter.pack_start(m_hbox_uuid);
		m_hbox_filter.reorder_child(m_hbox_uuid,5);
		m_refhbox=&m_hbox_uuid;
	}
	else if(m_combobox.get_active_text()=="Date"){
		m_hbox_filter.remove(*m_refhbox);
		m_hbox_filter.pack_start(m_hbox_date);
		m_hbox_filter.reorder_child(m_hbox_date,5);
		m_refhbox=&m_hbox_date;
	}
	else if(m_combobox.get_active_text()=="Start- and Endtime"){
		m_hbox_filter.remove(*m_refhbox);
		m_hbox_filter.pack_start(m_hbox_time);
		m_hbox_filter.reorder_child(m_hbox_time,5);
		m_refhbox=&m_hbox_time;
	}
	show_all_children();
}

void ExampleWindow::on_action_file_quit(){
	hide(); //Closes the main window to stop the Gtk::Main::run().
}

void ExampleWindow::on_action_file_new(){
	std::cout << "A File|New menu item was selected." << std::endl;
}

void ExampleWindow::on_action_file_save(){
	std::cout << "A File|New menu item was selected." << std::endl;
}

void ExampleWindow::on_action_others(){
	std::cout << "A menu item was selected." << std::endl;
}

void ExampleWindow::on_action_about(){
	aboutd.show();
}

void ExampleWindow::on_action_openics(){
	if(file_loaded==1){
		if((*cal1).is_changed()==1){
			Gtk::Dialog dia;
			dia.set_title("Unsaved changes will get lost!");
			//dia.set_secondary_text("Should i open calendar?");
			dia.add_button(Gtk::StockID(Gtk::Stock::OK), Gtk::RESPONSE_OK);
			dia.add_button(Gtk::StockID(Gtk::Stock::CANCEL), Gtk::RESPONSE_CANCEL);
			if(dia.run()==Gtk::RESPONSE_CANCEL)return;
		}
  	}
	Gtk::FileChooserDialog diag("Please choose a file..",Gtk::FILE_CHOOSER_ACTION_OPEN);
	diag.set_transient_for(*this);
	diag.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	diag.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_ics = Gtk::FileFilter::create();
	filter_ics->set_name("ics-Files");
	filter_ics->add_pattern("*.ics");
	diag.add_filter(filter_ics);
	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any Files");
	filter_any->add_pattern("*");
	diag.add_filter(filter_any);

	if(diag.run()==Gtk::RESPONSE_OK){
		std::string filename=diag.get_filename();
		std::cout<<"File choosen: "<<filename<<std::endl;
		cal1 = createcal(filename);
		EXECU(ret,cal1,load(filename,pid))
		file_loaded=true;
		listStore->clear();
		update_ui();
		enableelements();
	}
}

void ExampleWindow::on_action_openurl(){
	if(file_loaded==1){
		if((*cal1).is_changed()==1){
			Gtk::Dialog dia;
			dia.set_title("Unsaved changes will get lost!");
			//dia.set_secondary_text("Should i open calendar?");
			dia.add_button(Gtk::StockID(Gtk::Stock::OK), Gtk::RESPONSE_OK);
			dia.add_button(Gtk::StockID(Gtk::Stock::CANCEL), Gtk::RESPONSE_CANCEL);
			if(dia.run()==Gtk::RESPONSE_CANCEL)return;
		}
	}
	EnterURL geturl;
	if(geturl.run()!=0)return;
	//Check Format:
	int first=geturl.url.find("http");
	if((first<0)||(geturl.url.size()<8)){
		Gtk::MessageDialog dia(*this,"Not a valid URL!");
		dia.set_secondary_text("Fuck You!");
		dia.run();
		return;
	}
	cal1 = createcal(geturl.url);
	//Load events:
	Wait wait;
	wait.start();
	EXECU(ret,cal1,load(geturl.url,pid))
	//wait.end();
	if(ret==2){
		EnterPWD pwd;
		if(pwd.run()!=0)return;
		std::string server=server_from_url(geturl.url);
		new_netrc_entry(server,pwd.username,pwd.password);
		//Load events:
		wait.start();
		EXECU(ret,cal1,load(geturl.url,pid))
		wait.end();
		if(pwd.save==false)remove_netrc_entry(server);
	}
	if(ret==1){
		Gtk::MessageDialog dia(*this,"Something went terrible wrong!");
		dia.set_secondary_text("I don't know if it's your fault, but i guess so!");
		dia.run();
		return;
	  }
	  file_loaded=true;
	  listStore->clear();
	  update_ui();
	  enableelements();
}

//Updates UI - should NOT be called before file is loaded!
// If this runs correctly, there should be always the same amount of events
// in the ListView and in the C_calendar Selection (which is used as a filter).
void ExampleWindow::update_ui(){
	//Get list of filtered items:
	list<list<C_event>::iterator>* eventlist;
	EXECU(eventlist,cal1,pntr_selected())
	//Is something filtered?
	EXECU(ret,cal1,size())
	std::string eventsz = static_cast<ostringstream*>( &(ostringstream() << ret) )->str();
	EXECU(ret,cal1,is_selected())
	std::string selectedz = static_cast<ostringstream*>( &(ostringstream() << (*eventlist).size()) )->str();
	if(ret==0){
		m_label_filter.set_markup("Add Filter:");
		m_label_eventlist.set_markup("No Filter active - showing all "+eventsz+"  items:");
	}
	else {
		m_label_filter.set_markup("Further Filter:");
		m_label_eventlist.set_markup(selectedz+" items filtered:");
	}
	typedef Gtk::TreeModel::Children type_child;
	type_child children=listStore->children();
	//Load TreeView new if empty:
	if(children.size()==0){
		listStore->clear();
		Gtk::TreeModel::Row row;
		for(list<list<C_event>::iterator>::iterator iter=(*eventlist).begin();iter!=(*eventlist).end();++iter){
			row=(*listStore->append());
			row[cols.col_x]=true;
			if((**iter).cmpuuid()!="")row[cols.col_uuid]=(**iter).cmpuuid();
			if((**iter).cmpsummary()!="")row[cols.col_summary]=(**iter).cmpsummary();
			if((**iter).cmpstart()!="")row[cols.col_start]=convert_datetime((**iter).cmpstart());
			if((**iter).cmpend()!="")row[cols.col_end]=convert_datetime((**iter).cmpend());
			row[cols.col_iter]=(*iter);
		}
	}
	//Else Update TreeView:
	else{
		std::list<Gtk::TreeModel::RowReference> filtered_rows;
		//iterate through listStore:
		for(type_child::iterator iter = children.begin();iter!=children.end();++iter){
			list<C_event>::iterator liter=(*iter)[cols.col_iter];
			//check if event is in filtered-list and if not, save it in row as Reference
			if(std::find((*eventlist).begin(),(*eventlist).end(),liter) == (*eventlist).end()){
				Gtk::TreeModel::Path path(iter);
				filtered_rows.push_back(Gtk::TreeModel::RowReference(listStore, path));
				continue;
			}
			(*iter)[cols.col_uuid]=(*liter).cmpuuid();
			(*iter)[cols.col_summary]=(*liter).cmpsummary();
			(*iter)[cols.col_start]=convert_datetime((*liter).cmpstart());
			if((*liter).cmpend()!="")(*iter)[cols.col_end]=convert_datetime((*liter).cmpend());
		}
		//delete not-filtered events from TreeView:
		for(std::list<Gtk::TreeModel::RowReference>::iterator i=filtered_rows.begin();i!=filtered_rows.end();++i){
			Gtk::TreeModel::iterator treeiter = listStore->get_iter(i->get_path());
			listStore->erase(treeiter);
		}
	}
	//Update Statusbar:
	std::string statusbar;
	EXECU(statusbar,cal1,show_status())
	m_statusbar.pop();
	m_statusbar.push(statusbar);
}

//Creates an Iterlist out of the selected or not selected events in the liststore (col_x=x)
list<list<C_event>::iterator> ExampleWindow::liststore_to_iterlist(bool x){
	typedef Gtk::TreeModel::Children type_child;
	type_child children=listStore->children();
	list<list<C_event>::iterator> selected;
	for(type_child::iterator iter = children.begin();iter!=children.end();++iter){
		if((*iter)[cols.col_x]==x){
			selected.push_back((*iter)[cols.col_iter]);
		}
	}
	return selected;
}

void ExampleWindow::add_stock_item(const Glib::RefPtr<Gtk::IconFactory> & factory,const std::string& filepath,const Glib::ustring& id, const Glib::ustring& label)
{
	Gtk::IconSource source;
	try{
		//This throws an exception if the file is not found:
		source.set_pixbuf( Gdk::Pixbuf::create_from_file(filepath) );
	}
	catch(const Glib::Exception& ex){
		std::cout << ex.what() << std::endl;
	}

	source.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	source.set_size_wildcarded(); //Icon may be scaled.

	Glib::RefPtr<Gtk::IconSet> icon_set;
	icon_set->add_source(source); //More than one source per set is allowed.

	const Gtk::StockID stock_id(id);
	factory->add(stock_id, icon_set);
	Gtk::Stock::add(Gtk::StockItem(stock_id, label));
}

void ExampleWindow::register_stock_items(){
	Glib::RefPtr<Gtk::IconFactory> factory = Gtk::IconFactory::create();
	add_stock_item(factory, "rain.png", "example_stock_rain", "Stay dry");
	factory->add_default(); //Add factory to list of factories.
}
