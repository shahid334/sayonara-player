/********************************************************************************
** Form generated from reading UI file 'GUI_equalizer.ui'
**
** Created: Fri May 20 14:59:40 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_EQUALIZER_H
#define UI_GUI_EQUALIZER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GUI_Equalizer
{
public:
    QGridLayout *gridLayout_2;
    QSlider *sli_9;
    QSlider *sli_8;
    QSlider *sli_7;
    QSlider *sli_6;
    QSlider *sli_5;
    QSlider *sli_3;
    QSlider *sli_4;
    QSlider *sli_1;
    QSlider *sli_2;
    QSlider *sli_0;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QCheckBox *cb_enabled;
    QComboBox *combo_presets;
    QFrame *line;

    void setupUi(QWidget *GUI_Equalizer)
    {
        if (GUI_Equalizer->objectName().isEmpty())
            GUI_Equalizer->setObjectName(QString::fromUtf8("GUI_Equalizer"));
        GUI_Equalizer->resize(379, 119);
        gridLayout_2 = new QGridLayout(GUI_Equalizer);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 5, 0, 2);
        sli_9 = new QSlider(GUI_Equalizer);
        sli_9->setObjectName(QString::fromUtf8("sli_9"));
        sli_9->setMinimumSize(QSize(0, 65));
        sli_9->setMaximumSize(QSize(16777215, 65));
        sli_9->setMinimum(-24);
        sli_9->setMaximum(24);
        sli_9->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_9, 2, 10, 1, 1);

        sli_8 = new QSlider(GUI_Equalizer);
        sli_8->setObjectName(QString::fromUtf8("sli_8"));
        sli_8->setMinimumSize(QSize(0, 65));
        sli_8->setMaximumSize(QSize(16777215, 65));
        sli_8->setMinimum(-24);
        sli_8->setMaximum(24);
        sli_8->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_8, 2, 9, 1, 1);

        sli_7 = new QSlider(GUI_Equalizer);
        sli_7->setObjectName(QString::fromUtf8("sli_7"));
        sli_7->setMinimumSize(QSize(0, 65));
        sli_7->setMaximumSize(QSize(16777215, 65));
        sli_7->setMinimum(-24);
        sli_7->setMaximum(24);
        sli_7->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_7, 2, 8, 1, 1);

        sli_6 = new QSlider(GUI_Equalizer);
        sli_6->setObjectName(QString::fromUtf8("sli_6"));
        sli_6->setMinimumSize(QSize(0, 65));
        sli_6->setMaximumSize(QSize(16777215, 65));
        sli_6->setMinimum(-24);
        sli_6->setMaximum(24);
        sli_6->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_6, 2, 7, 1, 1);

        sli_5 = new QSlider(GUI_Equalizer);
        sli_5->setObjectName(QString::fromUtf8("sli_5"));
        sli_5->setMinimumSize(QSize(0, 65));
        sli_5->setMaximumSize(QSize(16777215, 65));
        sli_5->setMinimum(-24);
        sli_5->setMaximum(24);
        sli_5->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_5, 2, 6, 1, 1);

        sli_3 = new QSlider(GUI_Equalizer);
        sli_3->setObjectName(QString::fromUtf8("sli_3"));
        sli_3->setMinimumSize(QSize(0, 65));
        sli_3->setMaximumSize(QSize(16777215, 65));
        sli_3->setMinimum(-24);
        sli_3->setMaximum(24);
        sli_3->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_3, 2, 4, 1, 1);

        sli_4 = new QSlider(GUI_Equalizer);
        sli_4->setObjectName(QString::fromUtf8("sli_4"));
        sli_4->setMinimumSize(QSize(0, 65));
        sli_4->setMaximumSize(QSize(16777215, 65));
        sli_4->setMinimum(-24);
        sli_4->setMaximum(24);
        sli_4->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_4, 2, 5, 1, 1);

        sli_1 = new QSlider(GUI_Equalizer);
        sli_1->setObjectName(QString::fromUtf8("sli_1"));
        sli_1->setMinimumSize(QSize(0, 65));
        sli_1->setMaximumSize(QSize(16777215, 65));
        sli_1->setMinimum(-24);
        sli_1->setMaximum(24);
        sli_1->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_1, 2, 2, 1, 1);

        sli_2 = new QSlider(GUI_Equalizer);
        sli_2->setObjectName(QString::fromUtf8("sli_2"));
        sli_2->setMinimumSize(QSize(0, 65));
        sli_2->setMaximumSize(QSize(16777215, 65));
        sli_2->setMinimum(-24);
        sli_2->setMaximum(24);
        sli_2->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_2, 2, 3, 1, 1);

        sli_0 = new QSlider(GUI_Equalizer);
        sli_0->setObjectName(QString::fromUtf8("sli_0"));
        sli_0->setMinimumSize(QSize(0, 65));
        sli_0->setMaximumSize(QSize(16777215, 65));
        sli_0->setMinimum(-24);
        sli_0->setMaximum(24);
        sli_0->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(sli_0, 2, 1, 1, 1);

        label = new QLabel(GUI_Equalizer);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 1, 1, 1);

        label_2 = new QLabel(GUI_Equalizer);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        label_3 = new QLabel(GUI_Equalizer);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 1, 3, 1, 1);

        label_4 = new QLabel(GUI_Equalizer);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 1, 4, 1, 1);

        label_5 = new QLabel(GUI_Equalizer);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 1, 5, 1, 1);

        label_6 = new QLabel(GUI_Equalizer);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 1, 6, 1, 1);

        label_7 = new QLabel(GUI_Equalizer);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 1, 7, 1, 1);

        label_8 = new QLabel(GUI_Equalizer);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_2->addWidget(label_8, 1, 8, 1, 1);

        label_9 = new QLabel(GUI_Equalizer);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 1, 9, 1, 1);

        label_10 = new QLabel(GUI_Equalizer);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 1, 10, 1, 1);

        cb_enabled = new QCheckBox(GUI_Equalizer);
        cb_enabled->setObjectName(QString::fromUtf8("cb_enabled"));
        cb_enabled->setChecked(true);

        gridLayout_2->addWidget(cb_enabled, 0, 1, 1, 4);

        combo_presets = new QComboBox(GUI_Equalizer);
        combo_presets->setObjectName(QString::fromUtf8("combo_presets"));

        gridLayout_2->addWidget(combo_presets, 0, 7, 1, 4);

        line = new QFrame(GUI_Equalizer);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line, 3, 1, 1, 10);


        retranslateUi(GUI_Equalizer);
        QObject::connect(sli_0, SIGNAL(valueChanged(int)), label, SLOT(setNum(int)));
        QObject::connect(sli_1, SIGNAL(valueChanged(int)), label_2, SLOT(setNum(int)));
        QObject::connect(sli_2, SIGNAL(valueChanged(int)), label_3, SLOT(setNum(int)));
        QObject::connect(sli_3, SIGNAL(valueChanged(int)), label_4, SLOT(setNum(int)));
        QObject::connect(sli_4, SIGNAL(valueChanged(int)), label_5, SLOT(setNum(int)));
        QObject::connect(sli_5, SIGNAL(valueChanged(int)), label_6, SLOT(setNum(int)));
        QObject::connect(sli_6, SIGNAL(valueChanged(int)), label_7, SLOT(setNum(int)));
        QObject::connect(sli_7, SIGNAL(valueChanged(int)), label_8, SLOT(setNum(int)));
        QObject::connect(sli_8, SIGNAL(valueChanged(int)), label_9, SLOT(setNum(int)));
        QObject::connect(sli_9, SIGNAL(valueChanged(int)), label_10, SLOT(setNum(int)));

        QMetaObject::connectSlotsByName(GUI_Equalizer);
    } // setupUi

    void retranslateUi(QWidget *GUI_Equalizer)
    {
        GUI_Equalizer->setWindowTitle(QApplication::translate("GUI_Equalizer", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("GUI_Equalizer", "0", 0, QApplication::UnicodeUTF8));
        cb_enabled->setText(QApplication::translate("GUI_Equalizer", "activate", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GUI_Equalizer: public Ui_GUI_Equalizer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_EQUALIZER_H
