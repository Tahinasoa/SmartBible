#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsSimpleTextItem>
#include <QPair>
#include<QMessageBox>
#include<QFileDialog>
#include<QScreen>
#include<QCompleter>
#include<QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      screenListActionGroup(this)
{
    ui->setupUi(this);

    setWindowTitle("SmartBible") ;
    ui->tabWidget->setTabText(0,"Livre") ;
    ui->tabWidget->setTabText(1,"Recherche") ;
    ui->tabWidget->setTabText(2,"signets") ;
    ui->graphicsView->setScene(&scene) ;

    //versetValidation
    QRegExp exp{" *[1-9][0-9]* *( *- *[1-9][0-9]* *)?"} ;
    verseValidator = new QRegExpValidator(exp, this) ;
    ui->lineEdit_verse->setValidator(verseValidator) ;

    //Connect book selection signals and slots
    connect(ui->comboBox_bookList, SIGNAL(currentIndexChanged(int)), this,SLOT(currentBookChanged(int))) ;
    connect(ui->spinBox_chapitre , SIGNAL(valueChanged(int)), this,SLOT(currentChapterChanged())) ;


    //Make other connections
    connect(ui->pushButton_voirLivre, SIGNAL(clicked()),this, SLOT(loadBook()));
    initialiseBookList();

    //Update screens list
    connect(ui->action_ScreenList, SIGNAL(triggered(bool)),this, SLOT(updateScreensList()));


    //Page navigation connection
    connect(ui->pushButton_nextPage, SIGNAL(clicked()),&scene, SLOT(showNext()));
    connect(ui->pushButton_previousPage, SIGNAL(clicked()),&scene, SLOT(showPrevious()));
    ui->pushButton_nextPage->setShortcut(Qt::Key_N);
    ui->pushButton_previousPage->setShortcut(Qt::Key_P);

    connect(ui->action_zoomIn, SIGNAL(triggered(bool)),this, SLOT(zoomIn()));
    connect(ui->action_zoomOut, SIGNAL(triggered(bool)),this, SLOT(zoomOut()));
    updateScreensList();

    //Slide initialisation
    slide.setScene(&scene) ;
    connect(ui->action_showSlide, SIGNAL(triggered(bool)),this, SLOT(showSlide(bool)));
    connect(&slide, &Slide::onClosed, ui->action_showSlide, [=](){ui->action_showSlide->setChecked(false);});
    connect(&slide, SIGNAL(onNextPressed()),&scene, SLOT(showNext()));
    connect(&slide, SIGNAL(onPreviousPressed()),&scene, SLOT(showPrevious()));

    //Menu and tool connection
    connect(ui->action_afficherVerset, SIGNAL(triggered(bool)),this, SLOT(verseVisibilityChanged(bool)));

    //Set auto Completion for line edit
    QVector<QPair<int,QString>> books = data.getbooks();
    QStringList booksList;
    for(const auto &book : qAsConst(books)){
        booksList<<book.second;
    }
    QCompleter *bookNameCompleter = new QCompleter(booksList,this);
    bookNameCompleter->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive) ;
    bookNameCompleter->setFilterMode(Qt::MatchFlag::MatchContains) ;
    ui->lineEdit_currentBook->setCompleter(bookNameCompleter) ;
    connect(ui->lineEdit_currentBook,&QLineEdit::editingFinished,this,[&](){
        for(int index = ui->comboBox_bookList->count()-1 ; index>=0; index--){
            QString text = ui->comboBox_bookList->itemText(index);
            if(text.toLower() == ui->lineEdit_currentBook->text().toLower()){
                ui->comboBox_bookList->setCurrentIndex(index);
                ui->lineEdit_currentBook->setText(text) ;
            }
        }
    });

    connect(ui->action_afficherLivre,SIGNAL(triggered(bool)),&scene,SLOT(setTitleVisibility(bool)));
    //Init theme
    initialiseTheme();
    //Init title
    connect(&scene,&Scene::titleVisibilityChanged, ui->action_afficherLivre, &QAction::setChecked) ;

    //Init search widget and tool
    setFontSizeText();
    initSearchTab();
    initSignetPage() ;
    initFileManagement() ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->graphicsView->fitInView(0,0,scene.width(),scene.height(),Qt::AspectRatioMode::KeepAspectRatio) ;

}

void MainWindow::showEvent(QShowEvent *event)
{
    resizeEvent(nullptr) ;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    slide.close();
}

void MainWindow::initialiseTheme()
{
    themeActionGroup=new QActionGroup(this) ;
    connect(themeActionGroup,&QActionGroup::triggered,this,[&](){
        QAction *currentAction = themeActionGroup->checkedAction();
        if(currentAction != nullptr){
            if(currentAction->text() == "dark") {
                scene.setTheme(Scene::Theme::dark);
            }
            else{
                scene.setTheme(Scene::Theme::light);
            }
        }
    });

    QAction *a = ui->menu_theme->addAction("light") ;
    themeActionGroup->addAction(a);
    a->setCheckable(true);
    a->setData(Scene::Theme::light) ;

    a = ui->menu_theme->addAction("dark") ;
    themeActionGroup->addAction(a);
    a->setCheckable(true);
    a->setData(Scene::Theme::dark) ;
    a->setChecked(true); //default theme

    emit themeActionGroup->triggered(nullptr);
}

void MainWindow::initSearchTab()
{
    connect(ui->pushButton_search, &QPushButton::clicked, this, &MainWindow::searchButtonClicked);
    connect(ui->lineEdit_search,&QLineEdit::returnPressed,this, &MainWindow::searchButtonClicked) ;
    connect(ui->verseListView_search, &VerseListView::currentVerseChanged, this, &MainWindow::updateCurrentPassage) ;
}

void MainWindow::searchButtonClicked()
{
    QList<QMap<QString,QString>> result {data.search(ui->lineEdit_search->text(), ui->comboBox_bookListFilter->currentData().toInt())} ;
    ui->verseListView_search->clear() ;
    for(const auto &verse : result){
        ui->verseListView_search->append(verse.value("bookId"),
                                         verse.value("bookName"),
                                         verse.value("chapter"),
                                         verse.value("verse"),
                                         verse.value("verse"),
                                         verse.value("text")
                                         ) ;
    }
}

void MainWindow::initSignetPage()
{
    connect(ui->action_bookmark,&QAction::triggered,this,&MainWindow::addCurrentPassageToBookmark) ;
    connect(ui->verseListView_bookmark,&VerseListView::currentVerseChanged, this, &MainWindow::updateCurrentPassage);
}

void MainWindow::addCurrentPassageToBookmark()
{
    int bookId = currentPassage.value("bookId") ;
    int chapter = currentPassage.value("chapter") ;
    int verseMin = currentPassage.value("verseMin") ;
    int verseMax = currentPassage.value("verseMax") ;
    QString bookName = data.getBookName(bookId) ;
    QString passagePreview = data.getPassage(bookId,chapter,verseMin,verseMin).join("") ;
    ui->verseListView_bookmark->append(QString::number(bookId),
                                       bookName,
                                       QString::number(chapter),
                                       QString::number(verseMin),
                                       QString::number(verseMax),
                                       passagePreview
                                       ) ;
}

void MainWindow::initFileManagement()
{
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::save) ;
    connect(ui->action_saveAs, &QAction::triggered, this, &MainWindow::saveAs) ;
    connect(ui->action_open, &QAction::triggered, this, &MainWindow::openFile) ;
}

void MainWindow::initialiseBookList()
{
    //we initialize book list for search filtering
    ui->comboBox_bookListFilter->addItem("Boky rehetra",-1);

    QVector<QPair<int,QString>> books = data.getbooks();
    for(const auto &book : qAsConst(books)){
        ui->comboBox_bookList->addItem(book.second,book.first) ;
        ui->comboBox_bookListFilter->addItem(book.second,book.first);
    }
}

void MainWindow::currentBookChanged(int index)
{
    ui->lineEdit_currentBook->setText(ui->comboBox_bookList->itemText( index)) ;

    int chapterCount = data.getChapterCount(ui->comboBox_bookList->itemData(index).toInt()) ;
    ui->spinBox_chapitre->setMinimum(1) ;
    ui->spinBox_chapitre->setMaximum(chapterCount) ;
    ui->label_chapitre->setText("Chapitre ("+QString::number(chapterCount)+")") ;
    currentChapterChanged();

}

void MainWindow::currentChapterChanged()
{

    QString verseCount = QString::number(data.getVerseCount(ui->comboBox_bookList->currentData().toInt(), ui->spinBox_chapitre->value())) ;
    ui->label_verse->setText("Verset ("+verseCount+")");

    if(ui->lineEdit_verse->text().isEmpty()){
        ui->lineEdit_verse->setText("1");
    }
}

void MainWindow::loadBook()
{
    int bookId = ui->comboBox_bookList->currentData().toInt();
    int chapter = ui->spinBox_chapitre->value() ;
    int verseCount = data.getVerseCount(bookId, chapter) ;

    QString textVerset = ui->lineEdit_verse->text() ;
    textVerset.remove(" ") ;
    QStringList v = textVerset.split("-") ;
    int verseMin = 1;
    int verseMax = 1;
    if(!v.isEmpty())
        verseMin = v[0].toInt();
    else{
        ui->lineEdit_verse->setText("1");
    }

    if(v.length()>1)
        if(v[1].isEmpty()) {
            verseMax = verseCount ;
            ui->lineEdit_verse->setText(QString::number(verseMin)+" - "+QString::number(verseMax)) ;
        }
        else {verseMax = v[1].toInt() ;}
    else  verseMax = verseMin ;

    if(verseMin>verseMax){ //we have to exchange de values
        int tmp = verseMax;
        verseMax = verseMin ;
        verseMin = tmp ;
        ui->lineEdit_verse->setText(QString::number(verseMin)+"-"+QString::number(verseMax)) ;
    }
    if(verseMax>verseCount){
        QMessageBox::warning(this,"Attention","Le nombre de verse est trop grand\naucun changement") ;
        return;
    }
    updateCurrentPassage(bookId,chapter,verseMin,verseMax);
}

void MainWindow::updateCurrentPassage(int bookId, int chapter, int verseMin, int verseMax)
{
    QStringList passage = data.getPassage(bookId,chapter,verseMin,verseMax);
    scene.setContent(passage) ;

    QString bookName = data.getBookName(bookId);
    QString title = bookName+" "+QString::number(chapter)+" : "+QString::number(verseMin);
    if(verseMin!=verseMax) title+="-"+QString::number(verseMax);
    scene.setTitle(title);
    currentPassage.clear() ;
    currentPassage.insert("bookId",bookId) ;
    currentPassage.insert("chapter",chapter) ;
    currentPassage.insert("verseMin",verseMin) ;
    currentPassage.insert("verseMax",verseMax) ;
}

void MainWindow::updateScreensList()
{
    QList<QScreen*> screens = qApp->screens();
    ui->menu_screens->clear();

    for (auto screen : qAsConst(screens)){
        QAction *a = new QAction(ui->menu_screens) ;
        a->setText(screen->name()) ;
        QVariant data{screen->geometry()} ;
        a->setData(data);
        ui->menu_screens->addAction(a);
        screenListActionGroup.addAction(a);
        a->setCheckable(true) ;
        a->setChecked(true);
    }
}

void MainWindow::showSlide(bool visibility)
{
    if(visibility){
        QRect geometry{0,0,500,500};
        QAction *action = screenListActionGroup.checkedAction();
        if(action != nullptr){
            geometry = action->data().toRect();
        }
        scene.setSizeRatio(geometry.width(),geometry.height());
        slide.setGeometry(geometry);
        slide.showFullScreen();
        resizeEvent(nullptr);
    }
    else{
        slide.hide();
    }
}

void MainWindow::verseVisibilityChanged(bool isVisible)
{
    scene.setVerseNumVisibility(isVisible);
}

void MainWindow::saveAs()
{
    currentFile = QFileDialog::getSaveFileName(this,"Enregistrer les marques pages", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) ;
    if(currentFile.isEmpty()) return ;

    save() ;
}

void MainWindow::save()
{

    if (currentFile.isEmpty()){
        saveAs() ;
        return ;
    }

    QString text  = ui->verseListView_bookmark->toText();
    QFile file(currentFile) ;
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, "Attention", "impossible d'ouvrir "+currentFile) ;
        return ;
    }
    file.write(text.toStdString().c_str()) ;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Ouvrir",QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) ;
    if(fileName.isEmpty()) return ;
    currentFile = fileName ;
    QFile file(fileName) ;
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "Attention", "impossible d'ouvrir "+fileName+"\n"+file.errorString()) ;
        return ;
    }
    QTextStream textStream(&file) ;
    for(QString line = textStream.readLine(); !line.isEmpty(); line = textStream.readLine()){
        QStringList bookInfo = line.split(';', QString::SkipEmptyParts) ;
        if(bookInfo.count() == 5){
            auto bookName = bookInfo[1] ;
            bool ok, isOk = true ;

            auto bookId  = bookInfo[0].toInt(&ok) ;
            if (!ok) isOk = false ;
            auto chapter = bookInfo[2].toInt(&ok) ;
            if (!ok) isOk = false ;
            auto verseMin= bookInfo[3].toInt(&ok) ;
            if (!ok) isOk = false ;
            auto verseMax= bookInfo[4].toInt(&ok) ;
            if (!ok) isOk = false ;

            if(isOk){
                QString passagePreview = data.getPassage(bookId,chapter,verseMin,verseMin).join("") ;
                ui->verseListView_bookmark->append(QString::number(bookId),
                                                   bookName,
                                                   QString::number(chapter),
                                                   QString::number(verseMin),
                                                   QString::number(verseMax),
                                                   passagePreview
                                                   ) ;
            }
            else {
                QMessageBox::warning(this, "Attention", "Mauvaise ligne") ;
                return ;
            }
        }
    }
}

void MainWindow::zoomIn()
{
    scene.zoomIn();
    setFontSizeText() ;

}

void MainWindow::zoomOut()
{
    scene.zoomOut() ;
    setFontSizeText() ;
}

void MainWindow::setFontSizeText()
{
    ui->action_fontSize->setText(QString::number(scene.getFontSize())) ;
}
