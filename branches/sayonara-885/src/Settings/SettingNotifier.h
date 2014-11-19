#ifndef SETTINGNOTIFIER_H
#define SETTINGNOTIFIER_H

#include <QObject>

/*	Connect a Setting to a private slot in a class that wanna be notified
	whenever the setting changed:

	call: REGISTER_LISTENER(Set::LFM_Active, lfm_active_changed);
	where lfm_active_changed() is a private Slot in that class;
*/
#define REGISTER_LISTENER(setting_key, fn) \
	SettingNotifier<setting_key##_t>* v_##fn = SettingNotifier<setting_key##_t>::getInstance();\
	connect(v_##fn, SIGNAL(sig_value_changed()), this, SLOT( fn() )); \
	fn()



/*
 * The setting notifier emits a sig_value_changed whenever the value of
 * the underlying setting (defined by the SettingKey) has changed.
 * After the signal has been received the listener still can decide if
 * it's reading the new value or not
 */


/* we need an abstract instance of the notifier because Qt
 * Qt does not allow that a template class (like SettingNotifier)
 * can be a QObject and therefore signals
 */
class AbstrSettingNotifier : public QObject{

	Q_OBJECT

	protected:
		AbstrSettingNotifier(QObject* parent=0){}

	signals:
		void sig_value_changed();
};

/* A Setting notifier has to be a singleton */
template < typename T >
class SettingNotifier : public AbstrSettingNotifier{

	private:
		SettingNotifier( QObject* parent=0 ) : AbstrSettingNotifier(parent){}
		SettingNotifier( const SettingNotifier& ){}

	public:

	static SettingNotifier< T >* getInstance(){
		static SettingNotifier< T > inst;
		return &inst;
	}

	void val_changed(){
		emit sig_value_changed();
	}
};


#endif // SETTINGNOTIFIER_H
