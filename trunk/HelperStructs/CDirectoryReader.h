#ifndef DIRECTORY_READER
#define DIRECTORY_READER
#include <QString>
#include <QStringList>
#include <QDir>

class CDirectoryReader {
    public:
        CDirectoryReader ();
        virtual ~CDirectoryReader ();

        /**
          * Set filter for files.
          * @param filter list of different filters
          */
        void setFilter (const QStringList & filter);

        void getFilesInsiderDirRecursive (QDir baseDir, QStringList & files, int& num_files);
        void getFilesInsideDirectory (QDir baseDir, QStringList & files);

    private:
        QStringList m_filters;

};

#endif
