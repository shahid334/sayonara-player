#include "GUI/GUI_Simpleplayer.h"
#include "ui_GUI_Simpleplayer.h"
#include "HelperStructs/Helper.h"

#include <QDebug>

#include <QFileDialog>
#include <lastfmlib/lastfmscrobbler.h>





GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI_SimplePlayer)
{
    ui->setupUi(this);
    initGUI();

    this -> ui->albumCover->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "gui.jpg")));
    this -> m_playing = false;
    this -> m_cur_searching = false;
    this -> m_mute = false;

    m_trayIcon = new QSystemTrayIcon();
    m_trayIcon->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    setupTrayContextMenu();
    m_trayIcon->show();

    m_minTriggerByTray = false;
    m_minimized2tray = false;

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
    connect(this->ui->action_ViewPlaylist, SIGNAL(toggled(bool)), this, SLOT(showPlaylist(bool)));
    connect(this->ui->action_Dark, SIGNAL(toggled(bool)), this, SLOT(changeSkin(bool)));
    connect(this->ui->action_lastFM, SIGNAL(triggered(bool)), this, SLOT(lastFMClicked(bool)));


    ui_playlist = 0;

    m_skinSuffix = "";
}

GUI_SimplePlayer::~GUI_SimplePlayer()
{
    delete ui;
}


void GUI_SimplePlayer::changeSkin(bool dark){
	if(dark){
		this->ui->centralwidget->setStyleSheet("background-color: rgb(92, 92, 92);\ncolor: rgb(255, 255, 255);");
		m_skinSuffix = QString("_dark");
	}

	else {
		this->ui->centralwidget->setStyleSheet("color: rgb(0, 0, 0);");
		m_skinSuffix = QString("");
	}

	setupVolButton(this->ui->volumeSlider->value());
	emit skinChanged(dark);

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

void GUI_SimplePlayer::fillSimplePlayer (const MetaData & in) {

	if(in.year < 1000 || in.album.contains(QString::number(in.year)))
		this -> ui->album->setText(in.album);
	else this -> ui->album->setText(in.album + " (" + QString::number(in.year) +")");

    this -> ui->artist->setText(in.artist);
    this -> ui->title->setText(in.title);

    m_trayIcon->setToolTip("Currently playing: \"" + in.title + "\" by " + in.artist);

    QString lengthString = getLengthString(in.length_ms);

    this->ui->play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));

    this -> ui->maxTime->setText(lengthString);
    if (in.rating> 5) {
        qDebug() << "Error rating is to big";
    }

    int tmpRating = (rand() % 4) + 1;

    QString tmp;
    for (int i=0; i<tmpRating; ++i){
        //TODO make fancy stars or something
        tmp += "*";
    }
    this -> ui ->  rating->setText(tmp);
    this -> setWindowTitle(QString("Sayonara - ") + in.title);

//    if (albumCover!=NULL) {
//        this -> ui->albumCover->setPixmap(QPixmap::fromImage(*albumCover));
//    }
//    else {
//        this -> ui->albumCover->setPixmap(QPixmap::fromImage(QImage("Images/dummyCover.jpg")));
//    }
    this -> m_completeLength_ms = in.length_ms;
    this -> m_playing = true;




    emit wantCover(in);


/*
    LastFmScrobbler* scrobbler = new LastFmScrobbler(string("LucioCarreras"), string("rocks"), false, true);


    SubmissionInfo info(in.artist.toStdString(), in.title.toStdString());

    info.setAlbum(in.album.toStdString());
    info.setTrackNr(1);
    info.setTrackLength(in.length_ms / 1000);

    scrobbler->startedPlaying(info);

    cout << "scroblled" << endl;
*/





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

void GUI_SimplePlayer::searchSliderMoved(int search_percent){
	m_cur_searching = true;

	emit search( search_percent );
}

void GUI_SimplePlayer::volumeChangedSlider(int volume_percent) {

	setupVolButton(volume_percent);

    emit volumeChanged((qreal)volume_percent);
}

void GUI_SimplePlayer::setupVolButton(int percent){

	QString butFilename = Helper::getIconPath() + "vol_";

	if(percent == 0){
		butFilename += "mute.png";
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

void GUI_SimplePlayer::setVolume (qreal vol_percent) {


	qDebug() << "Volume is " << vol_percent;
    //this -> ui ->volumeSlider->setValue(vol_percent*100 -1);
}


void GUI_SimplePlayer::cover_changed(QPixmap& cover){

	this -> ui->albumCover->setPixmap(cover);
}

void GUI_SimplePlayer::coverClicked(bool){


	emit wantMoreCovers();
}


void GUI_SimplePlayer::showPlaylist(bool vis){

	if (vis) {
		this->ui_playlist->show();
		QRect rect = this->geometry();
		rect.setHeight(680);
		this->setGeometry(rect);
	}

	else {
		this->ui_playlist->hide();
		QRect rect = this->geometry();
		rect.setHeight(200);
		this->setGeometry(rect);
	}

}


void GUI_SimplePlayer::changeEvent(QEvent *event){

	if(isMinimized()  && !m_minTriggerByTray){
		if (m_trayIcon->isVisible()) {
			hide();
			QMainWindow::changeEvent(event);
			//event->ignore();
			m_minimized2tray = true;
		}
	}

	else event->ignore();
	m_minTriggerByTray = false;
}



void GUI_SimplePlayer::showAgain(QSystemTrayIcon::ActivationReason reason){

	m_minTriggerByTray = false;

	switch(reason){
		case QSystemTrayIcon::DoubleClick:
			if(m_minimized2tray){
				this->show();
				m_minimized2tray = false;
				m_minTriggerByTray = true;
			}
			else {
				this->hide();
				m_minimized2tray = true;
			}

			break;

		case QSystemTrayIcon::Context:

			break;

		default: break;


	}


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
	connect(showAction, SIGNAL(triggered()), this, SLOT(show()));



	trayContextMenu->addAction(m_playAction);
	trayContextMenu->addAction(m_stopAction);
	trayContextMenu->addSeparator();

	trayContextMenu->addAction(m_fwdAction);
	trayContextMenu->addAction(m_bwdAction);

	trayContextMenu->addSeparator();

	trayContextMenu->addAction(m_muteAction);
	trayContextMenu->addSeparator();
	trayContextMenu->addAction(m_closeAction);

	trayContextMenu->addAction(showAction);


	m_trayIcon->setContextMenu(trayContextMenu);



}


void GUI_SimplePlayer::keyPressEvent(QKeyEvent* e){




	switch(e->key()){
		case Qt::Key_Play:

			cout << "play" << endl;
			break;
		case Qt::Key_Stop:

			cout << "stop" << endl;
			break;
		default:
			//QMainWindow::keyPressEvent(event);
		break;

	}

}


QWidget* GUI_SimplePlayer::getParentOfPlaylist(){
	return this->ui->playlist_widget;
}




void GUI_SimplePlayer::setPlaylist(GUI_Playlist* playlist){


	ui_playlist = playlist;

	QSize tmpSize = this->size();
	qDebug() << tmpSize;
	QSize tmpSize2 = tmpSize;
	tmpSize.setWidth(tmpSize.width() -20 );
	tmpSize.setHeight(tmpSize.height() - 210);

	this->ui->playlist_widget->resize(tmpSize);
	this->ui_playlist->resize(tmpSize);

}



void GUI_SimplePlayer::resizeEvent(QResizeEvent* e){

	QMainWindow::resizeEvent(e);

	QSize tmpSize = e->size();
	tmpSize.setWidth(tmpSize.width() -20);
	tmpSize.setHeight(tmpSize.height() - 210);

	this->ui->playlist_widget->resize(tmpSize);
	this->ui_playlist->resize(tmpSize);


}


void GUI_SimplePlayer::lastFMClicked(bool b){
	qDebug() << "setupLastFM";
	emit setupLastFM();

}

void GUI_SimplePlayer::initGUI(){

	this->ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	this->ui->play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	this->ui->stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	this->ui->fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	this->ui->bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));




}
