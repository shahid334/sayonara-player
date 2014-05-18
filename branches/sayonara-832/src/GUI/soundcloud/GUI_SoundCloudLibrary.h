#ifndef GUI_SOUNDCLOUDLIBRARY_H
#define GUI_SOUNDCLOUDLIBRARY_H

#include "GUI/ui_GUI_Library_windowed.h"
#include "GUI/library/GUI_Library_windowed.h"

class Soundcloud_windowed : public Ui::Library_windowed {

};

class GUI_SoundCloudLibrary : public GUI_Library_windowed, private Soundcloud_windowed
{
	Q_OBJECT

public:
	explicit GUI_SoundCloudLibrary(QWidget *parent = 0);

	
};

#endif // GUI_SOUNDCLOUDLIBRARY_H
