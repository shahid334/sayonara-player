#include <QFile>

bool id3_write_discnumber(QFile& f, int discnumber, int n_discs=1);
bool id3_extract_discnumber(QFile& f, int* discnumber, int* n_discs);
