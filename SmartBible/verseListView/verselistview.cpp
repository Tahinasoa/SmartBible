#include "verselistview.h"
#include<QDebug>
#include<QBrush>
#include<QMouseEvent>
#include<QApplication>

VerseListView::VerseListView(QWidget *parent):
    QGraphicsView(parent)
{
    m_scene = new QGraphicsScene() ;
    setScene(m_scene) ;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff) ;
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn) ;

    //Menu
    QAction *show = m_menu.addAction("ouvrir") ;
    QAction *showAll = m_menu.addAction("ouvrir le chapitre") ;
    connect(show,&QAction::triggered,this,[this](){emitCurrentVerseChanged();}) ;
    connect(showAll,&QAction::triggered,this,[this](){emitCurrentVerseChanged(true);}) ;
}

VerseListView::~VerseListView()
{
    delete m_scene ;
}

void VerseListView::append(QString bookId, QString bookName, QString chapter, QString verseMin, QString verseMax, QString preview)
{
    auto verse = new VerseItem(bookId,bookName,chapter,verseMin,verseMax,preview) ;
    m_verses.append(verse) ;
    m_scene->addItem(verse) ;
    alignItems() ;
}

QString VerseListView::toText() const
{
    QString text;
    for(VerseItem*v  : m_verses){
        text+= v->toText()+"\n" ;
    }
    return text ;
}

void VerseListView::clear()
{
    m_verses.clear() ;
    m_scene->clear() ;
    m_currentVerse = nullptr ;
}

void VerseListView::alignItems()
{
    int x=0, y=0 ;
    for(auto item : qAsConst(m_verses)){
        item->setPos(x,y) ;
        item->setWidth(viewport()->width()) ;
        y+=item->boundingRect().height() ;
    }
    QRectF rec = m_scene->itemsBoundingRect() ;
    if(rec.height()<height()) rec.setHeight(height()) ;
    m_scene->setSceneRect(rec) ;

}

void VerseListView::resizeEvent(QResizeEvent *event)
{
    alignItems() ;
}

void VerseListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton || event->button() == Qt::MouseButton::RightButton){
        QPointF scenePos = mapToScene(event->pos()) ;
        m_currentVerse = verseAt(scenePos) ;
        for(auto v : qAsConst(m_verses)){
            v->setIsSelected(false) ;
        }
        if(m_currentVerse){
            m_currentVerse->setIsSelected(true) ;
            if(event->button() == Qt::MouseButton::LeftButton){
                emitCurrentVerseChanged();
            }
            else if(event->button() == Qt::MouseButton::RightButton){
                m_menu.popup(event->globalPos()) ;
            }
        }
    }
}

VerseItem *VerseListView::verseAt(QPointF scenePos)
{
    for(auto verse : qAsConst(m_verses)){
        QPointF pos =  verse->mapFromScene(scenePos) ;
        verse->setIsSelected(false) ;
        if(verse->contains(pos)){
            return verse ;
        }
    }
    return nullptr;
}

void VerseListView::emitCurrentVerseChanged(bool showChapter)
{
    int verseMax = m_currentVerse->verseMax().toInt() ;
    if(showChapter) verseMax = 200 ;
    emit currentVerseChanged(m_currentVerse->bookId().toInt(),
                             m_currentVerse->chapter().toInt(),
                             m_currentVerse->verseMin().toInt(),
                             verseMax) ;

}
