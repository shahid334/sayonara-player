/********************************************************************************
** Form generated from reading UI file 'GUI_TagEdit.ui'
**
** Created: Wed May 25 14:11:41 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_TAGEDIT_H
#define UI_GUI_TAGEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GUI_TagEdit
{
public:
    QGridLayout *gridLayout;
    QLabel *lab_filepath;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_2;
    QLabel *lab_title;
    QLineEdit *le_title;
    QLabel *label;
    QLineEdit *le_album;
    QLabel *label_2;
    QLineEdit *le_artist;
    QLabel *label_5;
    QComboBox *cb_genre;
    QLabel *label_3;
    QSpinBox *sb_track_num;
    QLabel *label_6;
    QPlainTextEdit *te_comment;
    QLabel *label_4;
    QSpinBox *sb_year;
    QLabel *lab_track_num;
    QPushButton *pb_prev;
    QPushButton *pb_next_track;
    QPushButton *btn_all_album;
    QPushButton *btn_all_artist;
    QPushButton *btn_all_year;
    QPushButton *btn_all_genre;
    QProgressBar *pb_progress;
    QDialogButtonBox *bb_ok_cancel;

    void setupUi(QWidget *GUI_TagEdit)
    {
        if (GUI_TagEdit->objectName().isEmpty())
            GUI_TagEdit->setObjectName(QString::fromUtf8("GUI_TagEdit"));
        GUI_TagEdit->resize(561, 273);
        gridLayout = new QGridLayout(GUI_TagEdit);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lab_filepath = new QLabel(GUI_TagEdit);
        lab_filepath->setObjectName(QString::fromUtf8("lab_filepath"));
        lab_filepath->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lab_filepath, 0, 0, 1, 2);

        scrollArea = new QScrollArea(GUI_TagEdit);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 532, 236));
        gridLayout_2 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lab_title = new QLabel(scrollAreaWidgetContents);
        lab_title->setObjectName(QString::fromUtf8("lab_title"));

        gridLayout_2->addWidget(lab_title, 1, 0, 1, 1);

        le_title = new QLineEdit(scrollAreaWidgetContents);
        le_title->setObjectName(QString::fromUtf8("le_title"));

        gridLayout_2->addWidget(le_title, 1, 1, 1, 4);

        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 2, 0, 1, 1);

        le_album = new QLineEdit(scrollAreaWidgetContents);
        le_album->setObjectName(QString::fromUtf8("le_album"));

        gridLayout_2->addWidget(le_album, 2, 1, 1, 4);

        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 3, 0, 1, 1);

        le_artist = new QLineEdit(scrollAreaWidgetContents);
        le_artist->setObjectName(QString::fromUtf8("le_artist"));

        gridLayout_2->addWidget(le_artist, 3, 1, 1, 4);

        label_5 = new QLabel(scrollAreaWidgetContents);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 4, 0, 1, 1);

        cb_genre = new QComboBox(scrollAreaWidgetContents);
        cb_genre->setObjectName(QString::fromUtf8("cb_genre"));

        gridLayout_2->addWidget(cb_genre, 4, 1, 1, 4);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 5, 0, 1, 1);

        sb_track_num = new QSpinBox(scrollAreaWidgetContents);
        sb_track_num->setObjectName(QString::fromUtf8("sb_track_num"));

        gridLayout_2->addWidget(sb_track_num, 5, 1, 1, 2);

        label_6 = new QLabel(scrollAreaWidgetContents);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 6, 0, 1, 1);

        te_comment = new QPlainTextEdit(scrollAreaWidgetContents);
        te_comment->setObjectName(QString::fromUtf8("te_comment"));

        gridLayout_2->addWidget(te_comment, 6, 1, 1, 5);

        label_4 = new QLabel(scrollAreaWidgetContents);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_4, 5, 3, 1, 1);

        sb_year = new QSpinBox(scrollAreaWidgetContents);
        sb_year->setObjectName(QString::fromUtf8("sb_year"));
        sb_year->setMinimum(0);
        sb_year->setMaximum(2100);
        sb_year->setValue(2011);

        gridLayout_2->addWidget(sb_year, 5, 4, 1, 1);

        lab_track_num = new QLabel(scrollAreaWidgetContents);
        lab_track_num->setObjectName(QString::fromUtf8("lab_track_num"));
        lab_track_num->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(lab_track_num, 0, 3, 1, 1);

        pb_prev = new QPushButton(scrollAreaWidgetContents);
        pb_prev->setObjectName(QString::fromUtf8("pb_prev"));
        pb_prev->setMinimumSize(QSize(30, 30));
        pb_prev->setMaximumSize(QSize(30, 30));

        gridLayout_2->addWidget(pb_prev, 0, 1, 1, 1);

        pb_next_track = new QPushButton(scrollAreaWidgetContents);
        pb_next_track->setObjectName(QString::fromUtf8("pb_next_track"));
        pb_next_track->setMinimumSize(QSize(30, 30));
        pb_next_track->setMaximumSize(QSize(30, 30));

        gridLayout_2->addWidget(pb_next_track, 0, 5, 1, 1);

        btn_all_album = new QPushButton(scrollAreaWidgetContents);
        btn_all_album->setObjectName(QString::fromUtf8("btn_all_album"));

        gridLayout_2->addWidget(btn_all_album, 2, 5, 1, 1);

        btn_all_artist = new QPushButton(scrollAreaWidgetContents);
        btn_all_artist->setObjectName(QString::fromUtf8("btn_all_artist"));

        gridLayout_2->addWidget(btn_all_artist, 3, 5, 1, 1);

        btn_all_year = new QPushButton(scrollAreaWidgetContents);
        btn_all_year->setObjectName(QString::fromUtf8("btn_all_year"));

        gridLayout_2->addWidget(btn_all_year, 5, 5, 1, 1);

        btn_all_genre = new QPushButton(scrollAreaWidgetContents);
        btn_all_genre->setObjectName(QString::fromUtf8("btn_all_genre"));

        gridLayout_2->addWidget(btn_all_genre, 4, 5, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 1, 0, 1, 2);

        pb_progress = new QProgressBar(GUI_TagEdit);
        pb_progress->setObjectName(QString::fromUtf8("pb_progress"));
        pb_progress->setValue(24);

        gridLayout->addWidget(pb_progress, 2, 0, 1, 1);

        bb_ok_cancel = new QDialogButtonBox(GUI_TagEdit);
        bb_ok_cancel->setObjectName(QString::fromUtf8("bb_ok_cancel"));
        bb_ok_cancel->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(bb_ok_cancel, 2, 1, 1, 1);


        retranslateUi(GUI_TagEdit);

        QMetaObject::connectSlotsByName(GUI_TagEdit);
    } // setupUi

    void retranslateUi(QWidget *GUI_TagEdit)
    {
        GUI_TagEdit->setWindowTitle(QApplication::translate("GUI_TagEdit", "Form", 0, QApplication::UnicodeUTF8));
        lab_filepath->setText(QApplication::translate("GUI_TagEdit", "Filepath", 0, QApplication::UnicodeUTF8));
        lab_title->setText(QApplication::translate("GUI_TagEdit", "Title", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GUI_TagEdit", "Album", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("GUI_TagEdit", "Artist", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("GUI_TagEdit", "Genre", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("GUI_TagEdit", "# Track", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("GUI_TagEdit", "Comment", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("GUI_TagEdit", "Year", 0, QApplication::UnicodeUTF8));
        lab_track_num->setText(QApplication::translate("GUI_TagEdit", "Track 1/15", 0, QApplication::UnicodeUTF8));
        pb_prev->setText(QApplication::translate("GUI_TagEdit", "<", 0, QApplication::UnicodeUTF8));
        pb_next_track->setText(QApplication::translate("GUI_TagEdit", ">", 0, QApplication::UnicodeUTF8));
        btn_all_album->setText(QApplication::translate("GUI_TagEdit", "all", 0, QApplication::UnicodeUTF8));
        btn_all_artist->setText(QApplication::translate("GUI_TagEdit", "all", 0, QApplication::UnicodeUTF8));
        btn_all_year->setText(QApplication::translate("GUI_TagEdit", "all", 0, QApplication::UnicodeUTF8));
        btn_all_genre->setText(QApplication::translate("GUI_TagEdit", "all", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GUI_TagEdit: public Ui_GUI_TagEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_TAGEDIT_H
