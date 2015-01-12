CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(ICONS 
	arrow_left.png
	arrow_right.png
	arrow_up.png
	arrow_down.png
	arrow_left_orange.png
	arrow_right_orange.png
	arrow_up_orange.png
	arrow_down_orange.png
	bg-checker.png
	border_a.png
	border_b.png
	cb_checked.png
	cb_checked_disabled.png
	favicon.ico
	rb_checked.png
	rb_checked_disabled.png
	slider_ver_bg.png
	slider_hor_bg.png
)	
	

INSTALL(FILES "sayonara.png" DESTINATION /usr/share/icons)
INSTALL(FILES "sayonara.png" DESTINATION /usr/share/pixmaps)
INSTALL(FILES ${ICONS} DESTINATION share/sayonara)


