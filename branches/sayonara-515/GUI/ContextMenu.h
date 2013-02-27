#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QAction>

#define ENTRY_INFO (1 << 0)
#define ENTRY_EDIT (1 << 1)
#define ENTRY_REMOVE (1 << 2)
#define ENTRY_DELETE (1 << 3)
#define ENTRY_PLAY_NEXT (1 << 4)


class ContextMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ContextMenu(QWidget *parent = 0);
    virtual ~ContextMenu();
    void setup_entries(int entries);

    
signals:
    void sig_info_clicked();
    void sig_edit_clicked();
    void sig_remove_clicked();
    void sig_delete_clicked();
    void sig_play_next_clicked();

private slots:
    void info_clicked();
    void edit_clicked();
    void remove_clicked();
    void delete_clicked();
    void play_next_clicked();

public slots:

private:
    QAction* 			_info_action;
    QAction* 			_edit_action;
    QAction* 			_remove_action;
    QAction* 			_delete_action;
    QAction*            _play_next_action;

    void clear_actions();
    
};

#endif // CONTEXTMENU_H
