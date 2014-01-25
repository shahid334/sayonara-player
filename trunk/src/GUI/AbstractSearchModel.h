#ifndef ABSTRACT_SEARCH_MODEL_H_
#define ABSTRACT_SEARCH_MODEL_H_

#include <QModelIndex>
#include <QAbstractTableModel>
#include <QAbstractListModel>



// We need this for eventual disambiguation between the
// table itself and this interface
// in the Searchable View class
class AbstractSearchModelInterface {

public:
    virtual QModelIndex getFirstRowIndexOf(QString substr)=0;
};


// Searchable Model for tables
class AbstractSearchTableModel : public QAbstractTableModel, public AbstractSearchModelInterface {



public:
    AbstractSearchTableModel(QObject* parent=0) : QAbstractTableModel(parent){}
    virtual QModelIndex getFirstRowIndexOf(QString substr)=0;

};


// Searchable Model for lists
class AbstractSearchListModel : public QAbstractListModel, public AbstractSearchModelInterface {

public:
     AbstractSearchListModel(QObject* parent=0) : QAbstractListModel(parent){}
     virtual QModelIndex getFirstRowIndexOf(QString substr)=0;

};



#endif
