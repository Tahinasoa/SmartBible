#ifndef VERSEITEM_H
#define VERSEITEM_H

#include <QGraphicsRectItem>
#include <QObject>

class VerseItem : public QGraphicsRectItem
{
public:
    VerseItem(QString bookId, QString bookName, QString chapter, QString verseMin, QString verseMax,QString preview);
    ~VerseItem() ;
    void setWidth(int newWidth) ;

    const QString &bookId() const;
    const QString &bookName() const;
    const QString &chapter() const;
    const QString &verseMin() const;
    const QString &verseMax() const;
    const QString &preview() const;
    QString toText() ;

protected:
    QGraphicsTextItem m_titleItem{this} ;
    QGraphicsTextItem m_previewItem{this} ;


    QString m_bookId;
    QString m_bookName;
    QString m_chapter;
    QString m_verseMin;
    QString m_verseMax;
    QString m_preview;
    bool m_isSelected{false} ;


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    bool isSelected() const;
    void setIsSelected(bool newIsSelected);
};

#endif // VERSEITEM_H
