#include "scene.h"
#include<QFont>
#include<QTextDocument>
#include<QDebug>
#include<QBrush>
#include<QGradient>
#include<QTextBlockFormat>
#include<QTextCursor>

Scene::Scene(QObject *parent) : QGraphicsScene(parent)
{
    addItem(&textItem) ;
    textItem.setTextWidth(m_width) ;
    addItem(&titleItem) ;
    titleItem.setTextWidth(m_width);

    QTextCursor cur= titleItem.textCursor() ;
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    cur.setBlockFormat(format) ;

    QFont f("Liberation serif",fontSize);
    textItem.setFont(f);
    textItem.setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);
    titleItem.setFont(f);
    titleItem.setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);

    setSceneRect({0,0,m_width,m_height});
    setVerseNumVisibility(false);//by default verses number are not visibles

    setTheme(currentTheme) ;
    cssStyle.setStyle(".title","font-weight","bold") ;
    textItem.document()->setDocumentMargin(30) ;

    setTitleVisibility(false);//by default, title is hidden
}
qreal Scene::height() const
{
    return m_height;
}

qreal Scene::width() const
{
    return m_width;
}

void Scene::setSizeRatio(qreal wr, qreal hr)
{
    if(hr==0) hr=1;
    if(wr==0) wr=1;

    m_height = m_width*hr/wr ;
    setSceneRect(0,0,m_width,m_height);
    setCurrentText();
    setTitleGeometry();
}

void Scene::setContent(QStringList newContent)
{
    firstIndex = 0;
    lastIndex = 0 ;
    m_content = newContent;
    setCurrentText();
    setTitleGeometry();

}

QStringList Scene::content() const
{
    return m_content;
}

void Scene::setTitle(QString title)
{
    titleItem.setHtml("<body><h2>"+title+"</h2></body>") ;
}

void Scene::setVerseNumVisibility(bool isVisible)
{
    if(isVisible){
        cssStyle.setStyle(".verseNum","font-size","1em") ;
    }
    else{
        cssStyle.setStyle(".verseNum","font-size","10px") ;
    }
    textItem.document()->setDefaultStyleSheet(cssStyle.toString()) ;
    setCurrentText() ;
}

void Scene::setTheme(Theme t)
{
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(2000, 2000));
    if(t == Theme::dark){
        linearGrad.setColorAt(0, QColor(0,0,0));
        linearGrad.setColorAt(1, QColor(0,0,0));
        cssStyle.setStyle("body","color","white");
        cssStyle.setStyle(".title","color","yellow");
    }
    else if(t == Theme::light){
        linearGrad.setColorAt(0, QColor(250,250,250));
        linearGrad.setColorAt(1, QColor(250,250,250));
        cssStyle.setStyle("body","color","black");
        cssStyle.setStyle(".title","color","darkblue");
    }

    QBrush backgroundBrush{linearGrad};
    setBackgroundBrush(backgroundBrush);
    textItem.document()->setDefaultStyleSheet(cssStyle.toString()) ;
    titleItem.document()->setDefaultStyleSheet(cssStyle.toString()) ;
    currentTheme = t;

    setTitle(titleItem.toPlainText());
    setCurrentText();
}

int Scene::getFontSize() const
{
    return fontSize;
}

void Scene::setTitleVisibility(bool isVisible)
{
    titleItem.setVisible(isVisible);
    textItem.setVisible(!isVisible) ;
}

void Scene::setCurrentText(bool reverse)
{
    textItem.setHtml("");
    QString html;
    if(!reverse){
        for(int i=firstIndex; i<m_content.length(); i++){
            textItem.setHtml(html+m_content[i]);
            if(textItem.boundingRect().height()>m_height) break;

            html.append(m_content[i]);
            lastIndex = i ;
        }
    }
    else{
        for(int i=lastIndex; i>=0; i--){
            textItem.setHtml(m_content[i]+html);
            if(textItem.boundingRect().height()>m_height) break;

            html.prepend(m_content[i]);
            firstIndex = i ;
        }
    }

    html = "<html><body>"+html+"</body></html>" ;
    textItem.setHtml(html);
}

void Scene::setTitleGeometry()
{
    titleItem.setPos(0,m_height/2 - titleItem.boundingRect().height()) ;
}

void Scene::showNext()
{
    if(titleItem.isVisible()){
        setTitleVisibility(false);
        emit titleVisibilityChanged(false);
        return;
    }

    if(lastIndex+1<m_content.length()){
        firstIndex   = lastIndex+1 ;
        setCurrentText();
    }
}

void Scene::showPrevious()
{
    if(titleItem.isVisible()){
        setTitleVisibility(false);
        emit titleVisibilityChanged(false);
        return;
    }

    if(firstIndex>1){
        lastIndex = firstIndex-1 ;
        setCurrentText(true);
    }
}

void Scene::zoomIn()
{
    fontSize+=10;
    QFont f("Liberation serif",fontSize);
    textItem.setFont(f);
    titleItem.setFont(f) ;
    setCurrentText();
}
void Scene::zoomOut()
{
    fontSize-=10;
    if(fontSize<20)fontSize=20 ;
    QFont f("Liberation serif",fontSize);
    titleItem.setFont(f) ;
    textItem.setFont(f);
    setCurrentText();
}
