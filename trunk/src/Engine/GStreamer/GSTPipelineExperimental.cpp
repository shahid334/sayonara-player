#include "Engine/GStreamer/GSTPipelineExperimental.h"
#include "Engine/GStreamer/GSTEngineHelper.h"
#include "HelperStructs/globals.h"
#include "gst/base/gstbasesrc.h"
#include "gst/base/gstbasetransform.h"

static void pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data){

	GstElement* ac = (GstElement*) data;
	GstPad* ac_pad = gst_element_get_static_pad(ac, "sink");

	if(ac_pad && gst_pad_is_linked(ac_pad)) {
		ENGINE_DEBUG << " Decoder and tee are already linked";
		return;

	}

	GstPadLinkReturn s = gst_pad_link(new_pad, ac_pad);
	if(s != GST_PAD_LINK_OK) ENGINE_DEBUG << " Could not link decoder with tee";
}


GSTPipelineExperimental::GSTPipelineExperimental(QObject* parent)
{

			bool success = false;
			int i = 0;
			_pipeline = 0;
			_timer = new QTimer();
			_timer->setInterval(5);
			connect(_timer, SIGNAL(timeout()), this, SLOT(start_play()));

			// eq -> autoaudiosink is packaged into a bin
			do {
				// create equalizer element
				GstElement* tmp_pipeline = gst_pipeline_new("Pipeline");
				_test_and_error(tmp_pipeline, "Engine: Pipeline sucks");

				_audio_src = gst_element_factory_make("uridecodebin", "src");

				_bus = gst_pipeline_get_bus(GST_PIPELINE(tmp_pipeline));

				_audio_convert = gst_element_factory_make("audioconvert", "audio_convert");

				_equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
				_volume = gst_element_factory_make("volume", "volume");

				_level = gst_element_factory_make("level", "level");
				_spectrum = gst_element_factory_make("spectrum", "spectrum");

				_level_audio_convert = gst_element_factory_make("audioconvert", "level_convert");
				_spectrum_audio_convert = gst_element_factory_make("audioconvert", "spectrum_convert");

				_audio_sink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

				_eq_queue = gst_element_factory_make("queue", "eq_queue");
				_tee = gst_element_factory_make("tee", "tee");
				_level_queue = gst_element_factory_make("queue", "level_queue");
				_spectrum_queue = gst_element_factory_make("queue2", "spectrum_queue");

				_level_sink = gst_element_factory_make("appsink", "level_sink");
				_spectrum_sink = gst_element_factory_make("fakesink", "spectrum_sink");


				if(!_test_and_error(_bus, "Engine: Something went wrong with the bus")) break;
				if(!_test_and_error(_audio_src, "Engine: Source creation fail")) break;
				if(!_test_and_error(_level, "Engine: Level cannot be created")) break;
				if(!_test_and_error(_spectrum, "Engine: Spectrum cannot be created")) break;
				if(!_test_and_error(_tee, "Engine: Tee cannot be created")) break;
				if(!_test_and_error(_equalizer, "Engine: Equalizer cannot be created")) break;
				if(!_test_and_error(_eq_queue, "Engine: Equalizer cannot be created")) break;
				if(!_test_and_error(_audio_sink, "Engine: Audio Sink cannot be created")) break;
				if(!_test_and_error(_level_queue, "Engine: Queue cannot be created")) break;
				if(!_test_and_error(_spectrum_queue, "Engine: Queue cannot be created")) break;
				if(!_test_and_error(_level_audio_convert, "Engine: Level converter fail")) break;
				if(!_test_and_error(_spectrum_audio_convert, "Engine: Spectrum converter fail")) break;


				gst_object_ref(_audio_src);

				gst_bin_add_many(GST_BIN(tmp_pipeline),

#ifdef ENGINE_OLD_PIPELINE
	_audio_src_file,
	_decoder,
#else
	_audio_src,

#endif
				_audio_convert, _tee,
				_eq_queue, _equalizer, _volume, _audio_sink,
				_level_queue, _level_audio_convert, _level, _level_sink,
				_spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);

#ifdef ENGINE_OLD_PIPELINE
				 success = gst_element_link_many(_audio_src_file, _decoder, NULL);
				 _test_and_error_bool(success, "Engine: Cannot link src with decoder");
#endif

				success = gst_element_link_many(_level_queue, _level_sink, NULL);
				_test_and_error_bool(success, "Engine: Cannot link Level pipeline");





				success = gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
				_test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline");

				success = gst_element_link_many(_eq_queue, _volume, _equalizer, _audio_sink, NULL);
				if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) break;

				success = gst_element_link_many(_audio_convert, _tee, NULL);
				if(!_test_and_error_bool(success, "Engine: Cannot link audio convert with tee")) break;

#ifdef ENGINE_OLD_PIPELINE
				_filemode = GSTFileModeFile;
				g_signal_connect (_decoder, "pad-added", G_CALLBACK (pad_added_handler), _audio_convert);
#else
				g_signal_connect (_audio_src, "pad-added", G_CALLBACK (pad_added_handler), _audio_convert);
#endif

				// Connect tee

				GstPad* tee_eq_pad;
				GstPad* eq_pad;
				GstPadLinkReturn s;

				// create tee pads
				_tee_src_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(_tee), "src%d");

				// "outputs" of tee to eq and queue
				tee_eq_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
					if(!_test_and_error(tee_eq_pad, "Engis == GST_PAD_LINK_OK)ne: tee_eq_pad is NULL")) break;
				eq_pad = gst_element_get_static_pad(_eq_queue, "sink");
					if(!_test_and_error(eq_pad, "Engine: eq pad is NULL")) break;

				_tee_level_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
					if(!_test_and_error(_tee_level_pad, "Engine: Tee-Level Pad NULL")) break;
				_level_pad = gst_element_get_static_pad(_level_queue, "sink");
					if(!_test_and_error(_level_pad, "Engine: Level Pad NULL")) break;

				_tee_spectrum_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
					if(!_test_and_error(_tee_spectrum_pad, "Engine: Tee-Spectrum Pad NULL")) break;
				_spectrum_pad = gst_element_get_static_pad(_spectrum_queue, "sink");
					if(!_test_and_error(_spectrum_pad, "Engine: Spectrum Pad NULL")) break;

				s = gst_pad_link (tee_eq_pad, eq_pad);
					_test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee eq with eq");
				s = gst_pad_link (_tee_level_pad, _level_pad);
					_test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee with level");
				s = gst_pad_link (_tee_spectrum_pad, _spectrum_pad);
					_test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee with spectrum");

				g_object_set (_eq_queue, "silent", TRUE, NULL);
				g_object_set (_level_queue, "silent", TRUE, NULL);
				g_object_set(_spectrum_queue, "silent", TRUE, NULL);

				guint64 interval = 30000000;
				gint threshold = - crop_spectrum_at;

		/*****/
				GstCaps* audio_caps = gst_caps_from_string (AUDIO_CAPS);
				g_object_set (_level_sink,
							  "drop", TRUE,
							  "max-buffers", 1,
							  "caps", audio_caps,
							  "emit-signals", FALSE,
							  NULL);

			   g_signal_connect (_level_sink, "new-buffer", G_CALLBACK (new_buffer), NULL);
		/*****/



				g_object_set (G_OBJECT (_level),
							  "message", TRUE,
							  "interval", interval / 10,
							  NULL);

				g_object_set (G_OBJECT (_spectrum),
							  "interval", interval,
							  "bands", N_BINS,
							  "threshold", threshold,
							  "message", TRUE,
							  "message-phase", FALSE,
							  "message-magnitude", TRUE,
							  "multi-channel", FALSE,
							  NULL);



				 /* run synced and not as fast as we can */
				g_object_set (G_OBJECT (_level_sink), "sync", TRUE, NULL);
				g_object_set (G_OBJECT (_level_sink), "async", FALSE, NULL);

				g_object_set (G_OBJECT (_spectrum_sink), "sync", TRUE, NULL);
				g_object_set (G_OBJECT (_spectrum_sink), "async", FALSE, NULL);

				gst_pad_set_active(_level_pad, false);
				gst_pad_set_active(_level_pad, true);


				gst_element_set_state(tmp_pipeline, GST_STATE_READY);

				success = true;
				_pipeline = tmp_pipeline;

				break;
			} while (i);


			if(success) gst_bus_add_watch(_bus, bus_state_changed, this);

			ENGINE_DEBUG << "Experimental Engine: constructor finished: " << success;
}



GSTPipelineExperimental::~GSTPipelineExperimental(){}


bool GSTPipelineExperimental::set_uri(gchar* uri){

	if(!uri) return false;


	ENGINE_DEBUG << "Pipeline experimental: " << uri;
	g_object_set(G_OBJECT(_audio_src), "uri", uri, NULL);
	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}


bool GSTPipelineExperimental::set_next_uri(gchar* uri){

	return GSTPipeline::set_next_uri(uri);
}

void GSTPipelineExperimental::start_timer(qint64 play_ms){

	if(play_ms > 0) _timer->start(play_ms);
}

void GSTPipelineExperimental::start_play(){
	_timer->stop();
	play();

}
