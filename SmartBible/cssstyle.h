#ifndef CSSSTYLE_H
#define CSSSTYLE_H
#include<QString>
#include<QMap>

class CssStyle
{
public:
    CssStyle();
    QString toString();
    void setStyle(QString cssSelector, QString property, QString value);
private:
    QMap<QString,QMap<QString,QString>>styleMap;
};

#endif // CSSSTYLE_H
