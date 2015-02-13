#include "GUI/equalizer/EqSlider.h"

EqSlider::EqSlider(QWidget *parent) :
	QSlider(parent)
{
}

void  EqSlider::setData(int idx, QLabel* label){
	_label = label;
	_idx = idx;
}

QLabel* EqSlider::getLabel(){
	return _label;
}

bool EqSlider::event(QEvent *e){

	switch(e->type()){

		case QEvent::HoverEnter:
			emit sig_slider_got_focus(_idx);
			break;


		case QEvent::HoverLeave:

			if(!this->hasFocus()){
				emit sig_slider_lost_focus(_idx);
			}

			break;

		default: break;
	}

	return QSlider::event(e);

}

void EqSlider::focusInEvent(QFocusEvent* e){
	QSlider::focusInEvent(e);
	emit sig_slider_got_focus(_idx);
}

void EqSlider::focusOutEvent(QFocusEvent* e){
	QSlider::focusOutEvent(e);

	emit sig_slider_lost_focus(_idx);
}

int EqSlider::getIndex(){
	return _idx;
}

void EqSlider::setIndex(int idx){
	_idx = idx;
}

void EqSlider::sliderChange(SliderChange change){
	QSlider::sliderChange(change);

	if(change == QAbstractSlider::SliderValueChange){

		emit sig_value_changed(_idx, this->value());
	}
}
