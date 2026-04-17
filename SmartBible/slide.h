#ifndef SLIDE_H
#define SLIDE_H

#include <QGraphicsView>
#include <QObject>

class Slide : public QGraphicsView
{
    Q_OBJECT
public:
    Slide();

    void showEvent(QShowEvent*);
    void resizeEvent(QResizeEvent*) ;
    void keyPressEvent(QKeyEvent*e);
private:
    void closeEvent(QCloseEvent *e);
signals:
    void onClosed();
    void onNextPressed();
    void onPreviousPressed();
};

#endif // SLIDE_H
