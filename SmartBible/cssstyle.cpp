#include "cssstyle.h"

CssStyle::CssStyle()
{

}

QString CssStyle::toString()
{
    QMapIterator<QString,QMap<QString,QString>> i(styleMap) ;
    QString cssStyle ;

    i.toBack();
    while(i.hasPrevious()){
        i.previous();
        cssStyle+= i.key()+"{";
        QMapIterator<QString,QString>j(i.value());
        j.toBack();
        while(j.hasPrevious()){
            j.previous();
            cssStyle+= j.key()+":"+j.value()+";";
        }
        cssStyle+="}" ;
    }
    return cssStyle ;

}

void CssStyle::setStyle(QString cssSelector, QString property, QString value)
{
    QMap<QString,QString> p = styleMap[cssSelector];
    p.insert(property,value);

    styleMap.insert(cssSelector,p) ;
}
