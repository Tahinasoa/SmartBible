#ifndef VERSELISTVIEW_H
#define VERSELISTVIEW_H

#include <QGraphicsView>
#include <QObject>
#include<QGraphicsScene>
#include<QVector>
#include<QMenu>
#include "verseitem.h"

class VerseListView : public QGraphicsView
{
    Q_OBJECT
public:
    VerseListView(QWidget *parent);
    ~VerseListView() ;
    void append(QString bookId, QString bookName, QString chapter, QString verseMin, QString verseMax,QString preview);
    QString toText() const;
    void clear() ;

protected:
    QGraphicsScene *m_scene ;
    QVector<VerseItem*> m_verses ;
    VerseItem *m_currentVerse{nullptr} ;
    void alignItems() ;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    VerseItem *verseAt(QPointF scenePos);

    void emitCurrentVerseChanged(bool showChapter=false);
//Menu
    QMenu m_menu{this} ;

signals :
    void currentVerseChanged(int bookId, int chapter, int verseMin, int verseMax) ;
};

#endif // VERSELISTVIEW_H
