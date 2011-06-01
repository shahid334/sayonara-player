#include "GUI/GUI_Simpleplayer.h"
#include "ui_GUI_Simpleplayer.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QDebug>

#include <QFileDialog>


GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI_SimplePlayer)
{
    ui->setupUi(this);
    initGUI();

    this -> ui->albumCover->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png")));
    this -> m_playing = false;
    this -> m_cur_searching = false;
    this -> m_mute = false;

    m_trayIcon = new QSystemTrayIcon();
    m_trayIcon->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    setupTrayContextMenu();
    m_trayIcon->show();

    m_minTriggerByTray = false;
    m_minimized2tray = false;

    QSize size = CSettingsStorage::getInstance()->getPlayerSize();
    QRect rect = this->geometry();
    rect.setWidth( size.width() );
    rect.setHeight( size.height() );
    setGeometry(rect);

    connect(this ->ui ->  play, SIGNAL(clicked(bool)),this,SLOT(playClicked(bool)));
    connect(this ->ui ->  fw, SIGNAL(clicked(bool)),this,SLOT(forwardClicked(bool)));
    connect(this ->ui ->  bw, SIGNAL(clicked(bool)),this,SLOT(backwardClicked(bool)));
    connect(this ->ui ->  stop, SIGNAL(clicked(bool)),this,SLOT(stopClicked(bool)));


    connect(this ->ui ->  action_OpenFile, SIGNAL(triggered(bool)),this,SLOT(fileSelectedClicked(bool)));
    connect(this ->ui ->  action_OpenFolder, SIGNAL(triggered(bool)),this,SLOT(folderSelectedClicked(bool)));
    connect(this ->ui ->  volumeSlider, SIGNAL(sliderMoved(int)),this,SLOT(volumeChangedSlider(int)));
    connect(this ->ui ->  btn_mute, SIGNAL(released()), this, SLOT(muteButtonPressed()));

    connect(this ->ui ->  songProgress, SIGNAL(sliderPressed()),this,SLOT(searchSliderPressed()));
    connect(this ->ui ->  songProgress, SIGNAL(sliderMoved(int)), this, SLOT(searchSliderMoved(int)));
    connect(this ->ui ->  songProgress, SIGNAL(sliderReleased()),this,SLOT(searchSliderReleased()));

    connect(this->m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showAgain(QSystemTrayIcon::ActivationReason)));
    connect(this->ui->action_ViewEqualizer, SIGNAL(toggled(bool)), this, SLOT(showEqualizer(bool)));
    connect(this->ui->action_Dark, SIGNAL(toggled(bool)), this, SLOT(changeSkin(bool)));
    connect(this->ui->action_lastFM, SIGNAL(triggered(bool)), this, SLOT(lastFMClicked(bool)));
    connect(this->ui->action_reloadLibrary, SIGNAL(triggered(bool)), this, SLOT(reloadLibraryClicked(bool)));
    connect(this->ui->action_setLibPath, SIGNAL(triggered(bool)), this, SLOT(setLibraryPathClicked(bool)));
    connect(this->ui->action_fetch_all_covers, SIGNAL(triggered(bool)), this, SLOT(fetch_all_covers_clicked(bool)));
   // connect(this->ui->eq_widget, SIGNAL(closed()), this, SLOT(show_equalizer()));



    ui_playlist = 0;

    m_skinSuffix = "";
}

GUI_SimplePlayer::~GUI_SimplePlayer()
{
    delete ui;
}


void GUI_SimplePlayer::setVolume(int vol){

	this->ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit volumeChanged((qreal)vol);

}


void GUI_SimplePlayer::changeSkin(bool dark){
	if(dark){
		this->ui->centralwidget->setStyleSheet("background-color: rgb(56, 56, 56);\ncolor: rgb(255, 255, 255);");
		this->setStyleSheet("background-color: rgb(56, 56, 56);\ncolor: rgb(255, 255, 255);");

				m_skinSuffix = QString("_dark");
	}

	else {
		this->ui->centralwidget->setStyleSheet("");
		this->ui->playlist_widget->setStyleSheet("");
		this->setStyleSheet("");
		m_skinSuffix = QString("");
	}

	setupVolButton(this->ui->volumeSlider->value());
	//emit skinChanged(dark);
	this->ui_library->change_skin(dark);
	this->ui_playlist->change_skin(dark);

}



QString GUI_SimplePlayer::getLengthString (quint32 length_ms)const {
    QString lengthString;
    int length_sec = length_ms /1000;
    QString min = QString::number(length_sec/60);
    QString sek = QString::number(length_sec%60);

    if(min.length() < 2)
    	min = QString('0') + min;

    if(sek.length() < 2)
    	sek = QString('0') + sek;

    lengthString = min + QString(":") + sek;
    return lengthString;
}

void GUI_SimplePlayer::update_info(const MetaData& in){
	// sometimes ignore the date
		if(in.year < 1000 || in.album.contains(QString::number(in.year)))
			this -> ui->album->setText(in.album);

		else this -> ui->album->setText(in.album + " (" + QString::number(in.year) +")");

	    this -> ui->artist->setText(in.artist);
	    this -> ui->title->setText(in.title);

	    m_trayIcon->setToolTip("Currently playing: \"" + in.title + "\" by " + in.artist);
	    this -> setWindowTitle(QString("Sayonara - ") + in.title);

	    emit wantCover(in);

}


void GUI_SimplePlayer::fillSimplePlayer (const MetaData & in) {

	// sometimes ignore the date
	if(in.year < 1000 || in.album.contains(QString::number(in.year)))
		this -> ui->album->setText(in.album);

	else this -> ui->album->setText(in.album + " (" + QString::number(in.year) +")");

    this -> ui->artist->setText(in.artist);
    this -> ui->title->setText(in.title);

    m_trayIcon->setToolTip("Currently playing: \"" + in.title + "\" by " + in.artist);

    QString lengthString = getLengthString(in.length_ms);

    this->ui->play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    this->m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    this->m_playAction->setText("Pause");

    this -> ui->maxTime->setText(lengthString);
    if (in.rating> 5) {
        qDebug() << "Error rating is to big";
    }

   // int tmpRating = (rand() % 4) + 1;



    QString tmp = QString("<font color=\"#FFAA00\" size=\"+10\">");
    qDebug() << "Bitrate = " << in.bitrate;
    if(in.bitrate < 96000) tmp += "*";
    else if(in.bitrate < 128000) tmp += "**";
    else if(in.bitrate < 160000) tmp += "***";
    else if(in.bitrate < 256000) tmp += "****";
    else tmp += "*****";
    tmp += "</font>";

    this -> ui ->  rating->setText(tmp);
    this->ui->rating->setToolTip(QString("<font color=\"#000000\">") + QString::number(in.bitrate) + "</font>");

    this -> setWindowTitle(QString("Sayonara - ") + in.title);

    this -> m_completeLength_ms = in.length_ms;
    this -> m_playing = true;

    emit wantCover(in);

}


void GUI_SimplePlayer::total_time_changed(qint64 total_time){
	m_completeLength_ms = total_time;
	this->ui->maxTime->setText(getLengthString(total_time));

}


void GUI_SimplePlayer::setCurrentPosition (quint32 pos_sec) {

    if (m_completeLength_ms != 0) {

    	double newSliderVal = (double)(pos_sec * 1000.0 * 100.0 / m_completeLength_ms);

    	if(!m_cur_searching)
    		this -> ui ->songProgress->setValue((int) newSliderVal);

        int min, sec;


        Helper::cvtSecs2MinAndSecs(pos_sec, &min, &sec);

        QString curPosString = 	Helper::cvtSomething2QString(min, 2) +
								QString(':') +
								Helper::cvtSomething2QString(sec, 2);


        this -> ui ->curTime->setText(curPosString);
    }
}

void GUI_SimplePlayer::playClicked(bool) {
    qDebug() << Q_FUNC_INFO;

    if (this -> m_playing == true) {
    	this->ui->play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    	m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    	m_playAction->setText("Play");
    	qDebug() << "pause";
    	emit pause();

    	this->ui->albumCover->setFocus();

    }

    else {
    	this->ui->play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    	m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    	m_playAction->setText("Pause");
    	qDebug() << "play";
    	emit play();

    	this->ui->albumCover->setFocus();

    }
    this -> m_playing = !this -> m_playing;
}

void GUI_SimplePlayer::stopClicked(bool) {
    qDebug() << Q_FUNC_INFO;
    this->ui->play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_playAction->setText("Play");
    m_playing = false;
    this->ui->title->setText("Sayonara Player");
    this->ui->rating->setText("");
    this->ui->album->setText("Written by Lucio Carreras");
    this->ui->artist->setText("");
    this->setWindowTitle("Sayonara");
    this->ui->songProgress->setValue(0);
    this->ui->curTime->setText("00:00");
    this->ui->maxTime->setText("00:00");
    this -> ui->albumCover->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png")));

    this->ui->albumCover->setFocus();
    emit stop();
}

void GUI_SimplePlayer::backwardClicked(bool) {
    qDebug() << Q_FUNC_INFO;
    this->ui->albumCover->setFocus();
    emit backward();
}

void GUI_SimplePlayer::forwardClicked(bool) {
    qDebug() << Q_FUNC_INFO;
    this->ui->albumCover->setFocus();
    emit forward();
}


void GUI_SimplePlayer::folderSelectedClicked(bool) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    getenv("$HOME"),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir != "")
    	emit baseDirSelected(dir);
}


void GUI_SimplePlayer::fileSelectedClicked(bool) {
    qDebug() << Q_FUNC_INFO;
    QStringList list = QFileDialog::getOpenFileNames(this, tr("Open Soundfiles"), QDir::homePath(), tr("MP3s (*.mp3)"));


    if(list.size() > 0) emit fileSelected(list);
}

void GUI_SimplePlayer::searchSliderPressed(){
	m_cur_searching = true;
}

void GUI_SimplePlayer::searchSliderReleased() {
	m_cur_searching = false;


}

void GUI_SimplePlayer::searchSliderMoved(int search_percent, bool by_app){
	m_cur_searching = true;

	if(!by_app) emit search( search_percent );
}

void GUI_SimplePlayer::volumeChangedSlider(int volume_percent) {

	setupVolButton(volume_percent);
    emit volumeChanged(volume_percent * 1.0);
    CSettingsStorage::getInstance()->setVolume(volume_percent);
}

void GUI_SimplePlayer::setupVolButton(int percent){

	QString butFilename = Helper::getIconPath() + "vol_";

	if(percent == 0){
		butFilename += QString("mute") + m_skinSuffix + ".png";
	}


	else if(percent < 40){
		butFilename += QString("1") + m_skinSuffix + ".png";
	}

	else if(percent < 80){
		butFilename += QString("2") + m_skinSuffix + ".png";
	}

	else {
		butFilename += QString("3") + m_skinSuffix + ".png";
	}


	this->ui->btn_mute->setIcon(QIcon(butFilename));


}

void GUI_SimplePlayer::muteButtonPressed(){

	if(m_mute){
		m_mute = false;
		this->ui->volumeSlider->setEnabled(true);

		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setText("Mute");

		setupVolButton(this->ui->volumeSlider->value());

		emit volumeChanged((qreal)this->ui->volumeSlider->value());
	}

	else{
		m_mute = true;
		this->ui->volumeSlider->setEnabled(false);
		this->ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_3.png"));
		m_muteAction->setText("Unmute");

		setupVolButton(0);

		emit volumeChanged(0);
	}


}




void GUI_SimplePlayer::cover_changed(QPixmap& cover){

	this -> ui->albumCover->setPixmap(cover);
}

void GUI_SimplePlayer::coverClicked(bool){


	emit wantMoreCovers();
}


void GUI_SimplePlayer::showEqualizer(bool vis){

	QRect rect = this->ui->playlist_widget->geometry();
	if (vis) {
		rect.setTop(rect.top() + this->ui_eq->height());
		rect.setHeight( this->ui_playlist->height() - this->ui_eq->height() );
		rect.setWidth( this->ui_playlist->width());

		QRect rect2 = this->ui_eq->geometry();
		rect2.setWidth(this->ui_playlist->width());

		this->ui_eq->setGeometry(rect2);

		this->ui->eq_widget->show();

	}

	else {
		this->ui->eq_widget->hide();
		rect.setHeight( this->ui_playlist->height() + this->ui_eq->height() );
		this->ui->action_ViewEqualizer->setChecked(false);

	}

	this->ui->playlist_widget->setGeometry(rect);
	this->ui_playlist->resize(this->ui->playlist_widget->size());
//	resizeEvent(0);

}


void GUI_SimplePlayer::close_eq(){
	showEqualizer(false);
}

void GUI_SimplePlayer::changeEvent(QEvent *event){

	if( event->type() == QEvent::WindowStateChange ){

		if(isMinimized())	hide();
		else show();
	}

}



void GUI_SimplePlayer::showAgain(QSystemTrayIcon::ActivationReason reason){

	switch(reason){
		case QSystemTrayIcon::Trigger:
			if(this->isMinimized() || isHidden())
				this->showNormal();
			if(!this->isActiveWindow())
				this->activateWindow();
			else {
				hide();
			}
		break;


		default:
		break;

	}

	Q_UNUSED(reason);

}

void GUI_SimplePlayer::setupIcons(){

}


void GUI_SimplePlayer::setupTrayContextMenu(){

	 QMenu*						trayContextMenu;
	 QAction*					showAction;


	trayContextMenu = new QMenu();


	m_playAction = new QAction(tr("Play"), this);
	m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	connect(m_playAction, SIGNAL(triggered()), this, SLOT(playClicked()));

	m_stopAction = new QAction(tr("Stop"), this);
	m_stopAction->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stopClicked()));

	m_bwdAction = new QAction(tr("Previous"), this);
	m_bwdAction->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
	connect(m_bwdAction, SIGNAL(triggered()), this, SLOT(backwardClicked()));


	m_fwdAction = new QAction(tr("Next"), this);
	m_fwdAction->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	connect(m_fwdAction, SIGNAL(triggered()), this, SLOT(forwardClicked()));

	m_muteAction = new QAction(tr("Mute"), this);
	m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
	connect(m_muteAction, SIGNAL(triggered()), this, SLOT(muteButtonPressed()));


	m_closeAction = new QAction(tr("Close"), this);
	m_closeAction->setIcon(QIcon(Helper::getIconPath() + "close.png"));
	connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close()));

	showAction = new QAction(tr("Show"), this);
	connect(showAction, SIGNAL(triggered()), this, SLOT(showNormal()));



	trayContextMenu->addAction(m_playAction);
	trayContextMenu->addAction(m_stopAction);
	trayContextMenu->addSeparator();

	trayContextMenu->addAction(m_fwdAction);
	trayContextMenu->addAction(m_bwdAction);

	trayContextMenu->addSeparator();

	trayContextMenu->addAction(m_muteAction);
	trayContextMenu->addSeparator();
	trayContextMenu->addAction(m_closeAction);

	//trayContextMenu->addAction(showAction);


	m_trayIcon->setContextMenu(trayContextMenu);



}


void GUI_SimplePlayer::keyPressEvent(QKeyEvent* e){

	qDebug() << e->key();
	switch(e->key()){
		case Qt::Key_MediaPlay:

			playClicked(true);
			break;
		case Qt::Key_MediaStop:

			stopClicked();
			break;

		case Qt::Key_MediaNext:

			forwardClicked(true);
			break;

		case Qt::Key_MediaPrevious:

			backwardClicked(true);
			break;

		case Qt::Key_E:
			this->ui->action_ViewEqualizer->setChecked(!this->ui->action_ViewEqualizer->isChecked());
		default:
		break;

	}

}


QWidget* GUI_SimplePlayer::getParentOfPlaylist(){
	return this->ui->playlist_widget;
}

QWidget* GUI_SimplePlayer::getParentOfLibrary(){
	return this->ui->library_widget;
}

QWidget* GUI_SimplePlayer::getParentOfEqualizer(){
	return this->ui->eq_widget;
}


void GUI_SimplePlayer::setPlaylist(GUI_Playlist* playlist){
	ui_playlist = playlist;

}



void GUI_SimplePlayer::setLibrary(GUI_Library_windowed* library){

	ui_library = library;
}

void GUI_SimplePlayer::setEqualizer(GUI_Equalizer* eq){
	ui_eq = eq;
}



void GUI_SimplePlayer::resizeEvent(QResizeEvent* e){

	Q_UNUSED(e);
	this->ui_playlist->resize(this->ui->playlist_widget->size());
	this->ui_library->resize(this->ui->library_widget->size());
	this->ui_eq->resize(this->ui->eq_widget->size());

	CSettingsStorage::getInstance()->setPlayerSize(this->size());
}


void GUI_SimplePlayer::lastFMClicked(bool b){

	Q_UNUSED(b);
	emit setupLastFM();

}

void GUI_SimplePlayer::reloadLibraryClicked(bool b){
	Q_UNUSED(b);
	emit reloadLibrary();
}

void GUI_SimplePlayer::initGUI(){

	this->ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	this->ui->play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	this->ui->stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	this->ui->fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	this->ui->bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));

}


void GUI_SimplePlayer::setLibraryPathClicked(bool b){
	Q_UNUSED(b);

	QString start_dir = getenv("$HOME");
	QString old_dir = CSettingsStorage::getInstance()->getLibraryPath();
	if(old_dir != "")
		start_dir = old_dir;

	 QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
								old_dir,
								QFileDialog::ShowDirsOnly);
	if(dir != ""){
		emit libpath_changed(dir);
		CSettingsStorage::getInstance()->setLibraryPath(dir);
	}


}


void GUI_SimplePlayer::fetch_all_covers_clicked(bool b){
	Q_UNUSED(b);
	emit fetch_all_covers();
}
