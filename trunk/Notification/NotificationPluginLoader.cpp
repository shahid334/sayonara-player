
NotificationPluginLoader* NotificationPluginLoader::getInstance(){
	static NotificationPluginLoader instance;
	return &instance;
}

        
virtual NotificationPluginLoader::~NotificationPluginLoader(){

	_notification_plugins.clear();
}

void NotificationPluginLoader::search_plugins(){
	QDir plugin_dir = QDir(app_dir);
	QStringList entry_list = plugin_dir.entryList(QDir::Files);
	foreach(QString filename, entry_list){
		QPluginLoader loader(plugin_dir.absoluteFilePath(filename);
		QObject* plugin = loader.instance();
		if(!plugin) continue;
			
		Notification* notification = qobject_cast<Notification*>(plugin);
		if(!notification) continue;
		
		qDebug() << "Found plugin " << notification->get_name();
		_notification_plugins.push_back(notification);		
	}
				
}

QList<Notification> NotificationPluginLoader::get_plugins(){
	return _notification_plugins;

}

        
NotificationPluginLoader::NotificationPluginLoader(){
	search_plugins();
}
        

