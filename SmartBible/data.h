#ifndef DATA_H
#define DATA_H
#include <QtSql>
#include <QPair>

class Data
{
public:
    Data();
    ~Data();
    QSqlDatabase database ;

    QVector<QPair<int , QString>> getbooks() ;
    QString getBookName(int bookId) ;
    int getChapterCount(int book_id) ;
    int getVerseCount(int book_id , int chapter) ;
    QStringList getPassage(int book_id, int chapter , int verseMin , int verseMax) ;
    QList<QMap<QString,QString>>search(QString patern, int bookId=-1) ; //return bookId, chapter,verse,text
    QList<QMap<QString,QString>>getBookmark(int limit = 20) ;
    void saveBookmark(QMap<QString,QString> data) ;

private :
    QStringList formatVerse(QString verseNum , QString verse) ;
};

#endif // DATA_H
