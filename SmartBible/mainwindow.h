#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QRegExp>
#include<QValidator>
#include<QActionGroup>
#include "data.h"
#include "scene.h"
#include "slide.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Data data;
    Scene scene;
    Slide slide;

    void resizeEvent(QResizeEvent *event) ;
    void showEvent(QShowEvent *event) ;
    void closeEvent(QCloseEvent *event) ;

    QRegExpValidator *verseValidator;
    QActionGroup screenListActionGroup;
    //Style initialisation
    QActionGroup *themeActionGroup ;
    void initialiseTheme();

    //Init search tab and tool
    void initSearchTab();
    void searchButtonClicked();


    //Init signet page;
    void initSignetPage() ;
    void addCurrentPassageToBookmark();

    QMap<QString,int>currentPassage;
    QString currentFile;
    void initFileManagement() ;

public slots :
    void initialiseBookList() ;
    void currentBookChanged(int index) ;
    void currentChapterChanged() ;
    void loadBook() ;
    void updateCurrentPassage(int bookId, int chapter, int verseMin,int verseMax) ;

    void updateScreensList();
    void showSlide(bool visibility);
    void verseVisibilityChanged(bool isVisible);

    void save() ;
    void saveAs() ;

    void openFile() ;

    void zoomIn() ;
    void zoomOut() ;
    void setFontSizeText() ;
};
#endif // MAINWINDOW_H
