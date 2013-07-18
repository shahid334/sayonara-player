CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(ICONS 
        append.png
        append_small.png
        broom.png
        bwd.png
        bwd_orange.png
        close.png
        fwd.png
        fwd_orange.png
        pause.png
        play.png
        rec.png
        rec_off.png
        rep1.png
        repAll.png
        save.png
        save_as.png
        shuffle.png
        stop.png
        vol_1.png
        vol_1_dark.png
        vol_2.png
        vol_2_dark.png
        vol_3.png
        vol_3_dark.png
        vol_mute.png
        vol_mute_dark.png
        gui.jpg
        play_small.png
        lastfm_logo.jpg
        lastfm_red_small.png
        power_on.png
        power_off.png
        edit.png
        id3.png
        reload.png
        dynamic.png
        arrow_up.png
        arrow_down.png
        arrow_right.png
        arrow_left.png
        arrow_up_orange.png
        arrow_down_orange.png
        arrow_right_orange.png
        arrow_left_orange.png
        scroll_hor_bg.png
        scroll_ver_bg.png
        slider_hor_bg.png
        slider_ver_bg.png
        delete.png
        import.png
        lyrics.png
        info.png
	info_small.png
        numbers.png
        radio.png
        cb_checked.png
        cb_checked_disabled.png
        rb_checked.png
        rb_checked_disabled.png
        logo.png
        logo_small.png
        podcast.png
        playlist.png
)

INSTALL(FILES ${ICONS} DESTINATION share/sayonara)
INSTALL(FILES "sayonara.png" DESTINATION /usr/share/icons)
INSTALL(FILES "sayonara.png" DESTINATION /usr/share/pixmaps)


