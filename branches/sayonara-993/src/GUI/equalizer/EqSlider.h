#ifndef EQSLIDER_H
#define EQSLIDER_H

#include <QSlider>
#include <QLabel>
#include <QEvent>

class EqSlider : public QSlider {
	Q_OBJECT

	signals:
		void sig_value_changed(int idx, int val);
		void sig_slider_got_focus(int idx);
		void sig_slider_lost_focus(int idx);

	public:
		EqSlider(QWidget* parent);
		virtual ~EqSlider() {}

		void setData(int idx, QLabel* label);
		QLabel* getLabel();
		int getIndex();
		void setIndex(int idx);


	protected:
		virtual void sliderChange(SliderChange change);

	private:

		QLabel* _label;
		int _idx;

		void focusInEvent(QFocusEvent* e);
		void focusOutEvent(QFocusEvent* e);
		bool event(QEvent *e);
};

#endif // EQSLIDER_H
