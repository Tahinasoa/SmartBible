#include "verseitem.h"
#include<QBrush>
#include<QPen>
#include<QGraphicsSimpleTextItem>
#include<QPainter>
#include<QTextDocument>

VerseItem::VerseItem(QString bookId, QString bookName, QString chapter, QString verseMin, QString verseMax, QString preview):
    m_bookId{bookId},
    m_bookName{bookName},
    m_chapter{chapter},
    m_verseMin{verseMin},
    m_verseMax{verseMax},
    m_preview{preview}
{
    setPen(Qt::NoPen) ;

    QString titleText = m_bookName+" "+m_chapter+" : "+m_verseMin ;
    if(m_verseMin != m_verseMax)
        titleText+= "-"+m_verseMax ;

    //Style
    m_titleItem.document()->setDefaultStyleSheet("body"
                                               "{"
                                                   "color:white;"
                                                   "font-weight:bold;"
                                               "}") ;

    m_titleItem.setHtml("<html><body>"+titleText+"</body><html>") ;
    m_previewItem.setHtml(m_preview) ;
}

VerseItem::~VerseItem()
{
}

void VerseItem::setWidth(int newWidth)
{
    m_titleItem.document()->setTextWidth(newWidth) ;
    m_previewItem.document()->setTextWidth(newWidth) ;

    m_previewItem.setPos(0,m_titleItem.boundingRect().height()) ;

    setRect(childrenBoundingRect()) ;
}

const QString &VerseItem::bookId() const
{
    return m_bookId;
}

const QString &VerseItem::bookName() const
{
    return m_bookName;
}

const QString &VerseItem::chapter() const
{
    return m_chapter;
}

const QString &VerseItem::verseMin() const
{
    return m_verseMin;
}

const QString &VerseItem::verseMax() const
{
    return m_verseMax;
}

const QString &VerseItem::preview() const
{
    return m_preview;
}

QString VerseItem::toText()
{
    QString text  ;
    text+= m_bookId +";";
    text+= m_bookName+";" ;
    text+= m_chapter+";" ;
    text+= m_verseMin+";" ;
    text+= m_verseMax ;
    return text ;
}

bool VerseItem::isSelected() const
{
    return m_isSelected;
}

void VerseItem::setIsSelected(bool newIsSelected)
{
    m_isSelected = newIsSelected;
    update() ;
}

void VerseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush brush ;
    brush.setStyle(Qt::BrushStyle::SolidPattern) ;
    if(isSelected()){
        brush.setColor(QColor{150,150,200}) ;
        painter->fillRect(boundingRect(),brush) ;
    }
    brush.setColor(QColor(QString("#A1A1FF"))) ;
    painter->fillRect(m_titleItem.boundingRect(),brush) ;

}
