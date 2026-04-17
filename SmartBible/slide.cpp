#include "slide.h"
#include "scene.h"
#include<QGraphicsScene>
#include<QKeyEvent>

Slide::Slide()
{

}

void Slide::showEvent(QShowEvent *)
{
    resizeEvent(nullptr);

}

void Slide::resizeEvent(QResizeEvent *)
{
    QGraphicsView::fitInView(sceneRect(),Qt::AspectRatioMode::KeepAspectRatio) ;
}

void Slide::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key::Key_Escape){
        close();
    }
    else if(e->key() == Qt::Key::Key_N){
        emit onNextPressed();
    }
    else if(e->key() == Qt::Key::Key_P){
        emit onPreviousPressed();
    }

}

void Slide::closeEvent(QCloseEvent *e)
{
    emit onClosed();
    e->accept();
}
