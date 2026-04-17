#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include<QGraphicsTextItem>
#include "cssstyle.h"

#include <QObject>

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Theme{
        dark,
        light
    };
public :
    explicit Scene(QObject *parent = nullptr);
    qreal height() const;
    qreal width() const;
    void setSizeRatio(qreal wr,qreal hr);

    void setContent(QStringList newContent) ;
    QStringList content() const;
    void setTitle(QString title) ;
    void setVerseNumVisibility(bool isVisible) ;

    void setTheme(Theme t) ;
    Theme currentTheme{Theme::light} ;
    CssStyle cssStyle;
    int getFontSize() const;

private :
    QGraphicsTextItem textItem;
    QGraphicsTextItem titleItem;
    qreal m_width{2000} ;
    qreal m_height{2000} ;
    int fontSize = 120;
    //
    QStringList m_content; //The list of word to insert in the scene
    int firstIndex{0} ;//Index of the first word currently shown
    int lastIndex{0} ;//Index of the last word currently shown

    void setCurrentText(bool reverse=false);
    void setTitleGeometry();
public slots :
    void showNext();
    void showPrevious();
    void zoomIn() ;
    void zoomOut() ;

    void setTitleVisibility(bool isVisible);
signals:
    void titleVisibilityChanged(bool);
};

#endif // SCENE_H
