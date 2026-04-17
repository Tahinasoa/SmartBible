#include "data.h"
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <stdexcept>
#include<QMap>

Data::Data()
{
    QString filename{"baiboly.db"} ;
    QFileInfo info{qApp->applicationDirPath()+"/"+filename} ;
    if( !info.exists() || info.isDir()){
        QMessageBox::critical(nullptr , "Erreur de la base de donnée" , "Le fichier "+filename+" n'est pas dans le dossier : "+qApp->applicationDirPath()) ;
        throw std::runtime_error("Base de donnée no trouvée") ;
    }
    database = QSqlDatabase::addDatabase("QSQLITE") ;
    database.setDatabaseName(filename) ;
    database.open() ;
}

Data::~Data()
{
    database.close() ;
}
QVector<QPair<int , QString>> Data::getbooks()
{
    QSqlQuery req("SELECT id , name from books order by id") ;
    req.exec() ;
    QVector<QPair<int , QString>> books ;
    while(req.next()){
        int id = req.value(0).toInt();
        QString name = req.value(1).toString() ;
        books.push_back({id , name}) ;
    }
    return books ;

}

QString Data::getBookName(int bookId)
{
    QSqlQuery req;
    req.prepare("SELECT name from books where id=:bookId") ;
    req.bindValue(":bookId",bookId) ;
    req.exec() ;
    QString name;
    if(req.next()) name = req.value(0).toString() ;

    return name ;
}

int Data::getChapterCount(int book_id)
{
    QSqlQuery req;
    req.prepare("SELECT chapterCount from books where id=:bookId") ;
    req.bindValue(":bookId",book_id) ;
    req.exec() ;

    if(req.next()){
        return req.value(0).toInt();
    }
    return 0 ;
}

int Data::getVerseCount(int book_id, int chapter)
{
    QSqlQuery req;
    req.prepare("SELECT COUNT(id) from verses where book_id=:book and chapter=:chapter") ;
    req.bindValue(":book",book_id) ;
    req.bindValue(":chapter",chapter) ;
    req.exec() ;


    if(req.next()){
        return req.value(0).toInt();
    }
    return 0 ;

}

QStringList Data::getPassage(int book_id, int chapter, int verseMin, int verseMax)
{
    QSqlQuery req;
    req.prepare("select book_id ,chapter , verse ,text from verses where book_id=:book_id and chapter=:chapter and verse>= :min and verse<= :max order by verse") ;
    req.bindValue(":book_id",book_id) ;
    req.bindValue(":chapter",chapter) ;
    req.bindValue(":min", verseMin) ;
    req.bindValue(":max",verseMax) ;
    req.exec() ;
    QStringList wordList ;
    while(req.next()){
        QString text_tmp = req.value(3).toString() ;
        QString verseNum = req.value(2).toString();
        wordList.append(formatVerse(verseNum , text_tmp)) ;
    }
    return wordList ;
}

QList<QMap<QString, QString> > Data::search(QString patern, int bookId)
{
    QSqlQuery req;
    req.prepare("SELECT book_id,chapter,verse,text FROM verses WHERE text LIKE :patern and (book_id=:bookId or :bookId=-1) limit 100");
    req.bindValue(":patern","%"+patern+"%");
    req.bindValue(":bookId", bookId) ;
    req.exec() ;

    QList <QMap<QString,QString>>result;
    while(req.next()){
        QMap<QString,QString> map;
        QString bookId = req.value(0).toString() ;
        QString chapter = req.value(1).toString();
        QString verse = req.value(2).toString() ;
        QString text = req.value(3).toString() ;

        map.insert("bookId",bookId);
        map.insert("bookName",getBookName(bookId.toInt())) ;
        map.insert("chapter",chapter);
        map.insert("verse",verse) ;
        map.insert("text",text) ;
        result.append(map) ;
    }
    return result ;
}

QList<QMap<QString, QString> > Data::getBookmark(int limit)
{
    QSqlQuery req ;
    req.exec("SELECT bookId,chapter,verseMin,verseMax,preview FROM bookmarks") ;
    QList<QMap<QString,QString>> bookmarks ;
    while(req.next()){
        QMap<QString,QString> values;
        values.insert("bookId",req.value(0).toString()) ;
        values.insert("chapter",req.value(1).toString()) ;
        values.insert("verseMin",req.value(2).toString()) ;
        values.insert("verseMax",req.value(3).toString()) ;
        values.insert("preview",req.value(4).toString()) ;
        QString bookName = getBookName(req.value(0).toInt()) ;
        values.insert("bookName",bookName) ;
        bookmarks.append(values) ;
    }
    return bookmarks ;
}

void Data::saveBookmark(QMap<QString, QString> data)
{

}

QStringList Data::formatVerse(QString verseNum , QString verse)
{
    verse = verse.simplified();
    QString title;
    if(verse[0] == "[" && verse.contains(']')){
        verse.remove(0,1);
        int i = verse.indexOf(']');
        verse = verse.replace(i , 1 , "|&|") ;
        QStringList l = verse.split("|&|");
        title = l[0];
        verse = l[1];
    }
    QStringList wordList;

    if(!title.isEmpty()){
        wordList = title.split(" ");
        for(auto &word : wordList)
            word = "<span class='title'>"+word+"</span>" ;
        wordList.append("<br/>");
    }

    QStringList tmp_list{verse.split(" ")} ;
    tmp_list[0]= "<span class='verseNum' style='vertical-align:super' >"+verseNum+" </span>"+tmp_list[0];
    wordList.append(tmp_list) ;
    for(auto &word : wordList){
        word=" "+word;
    }
    return wordList ;
}
