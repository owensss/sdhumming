#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
// #include <QDialog>
#include <QFileDialog>
#include <QtGui>
#include <QTextCodec>
#include "SModel.h"
//#include "SSearch.h"
#ifdef WIN32
    #include "SDFuzzySearch.h"
    #pragma comment(lib, "D:/sdhumming/Gao/SDFuzzySearch.lib")
#endif
#include "SUtil.h"
#include "SMelody.h"
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <QProcess>
#include <cstdlib>
#include <windows.h>

// const QString MainWindow::player = "D:/Soft/Foobar2000/foobar2000.exe ";

int STester(double& score, char* szModel, char* szModelinfo, char* szWav, char* szOut);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTextCodec *encoding = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(encoding);
    QTextCodec::setCodecForCStrings(encoding);
    QTextCodec::setCodecForLocale(encoding);
    ui->setupUi(this);
    setDefaultMusicPath();
    // no festival fuck constant
    loadDefaultMusic(musicpath+"music.list");
    QStringList args;
	args << player;
    external_player = new QProcess(this);
    external_player->start("cmd", args);
    playerController(none, "");
    // external_player.write("D:/Soft/Foobar2000/foobar2000.exe /add D:/sdhumming/Gao/midi/rhythsea.mid\n");
    // external_player.write("D:/Soft/Foobar2000/foobar2000.exe /play\n");
    // vlc.write("play\n");
    // vlc.write("quit\n");
//    connect(external_player, SIGNAL(readyRead()), this, SLOT(readoutput()));
//    connect(external_player, SIGNAL(destroyed()), this, SLOT(destroyProcess()));
    connect(this, SIGNAL(destroyed()), this, SLOT(destroyProcess()));
}

void MainWindow::readoutput() {
    qDebug() << external_player->readAll();
}

bool MainWindow::setDefaultMusicPath(void) {
    QFile fs("./config.txt");
    if (! fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // try to open it manually
        QMessageBox::warning(this, "Error", "Config.txt Not Found!");
        fs.setFileName(QFileDialog::getOpenFileName(NULL,
             QString("打开配置文件")));
        if (! fs.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(NULL, QString("Error"), QString("Opening Config.txt Failed!"));
            return false;
        }
    }
    // file must in the form
    /*
       present_name = filename(without suffix)
     */
    QTextStream text(&fs);
    QStringList spliter;
    QString line;

    while (! text.atEnd()) {
        line = text.readLine();
        spliter = line.split("=");
        if (spliter.size() != 2) { // format error, just skip
            QMessageBox::warning(this, "Warning", "config File format  incorrect#"+ line);
            continue;
        }
        if (spliter[0].remove(" ") == "MUSIC_PATH") {
            musicpath = spliter[1].remove(" ");
            if (musicpath[musicpath.size()-1]!='/')
                musicpath.push_back('/');
            qDebug() << musicpath << spliter[1];
        } else if (spliter[0].remove(" ") == "BuildModelExecutable") {
			build_model_exe = spliter[1].remove(" ");
		} else if (spliter[0].remove(" ") == "MUSIC_PLAYER") {
			player = spliter[1].remove(" ");
		}
    }

    return true;
}

bool MainWindow::loadDefaultMusic(const QString &filename) {
    QTextCodec *encoding = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(encoding);
    QTextCodec::setCodecForCStrings(encoding);
    QTextCodec::setCodecForLocale(encoding);
    QFile fs(filename);
    qDebug() << filename;
    if (! fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // try to open it manually
        QMessageBox::warning(this, "Error", "Music List Not Found!");
        fs.setFileName(QFileDialog::getOpenFileName(NULL,
             QString("打开音乐列表文件")));
        if (! fs.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(NULL, QString("Error"), QString("Opening Music List Failed!"));
            return false;
        }
    }
    // file must in the form
    /*
       present_name = filename(without suffix)
     */
    QTextStream text(&fs);
    QStringList spliter;
    QString line;

    while (! text.atEnd()) {
        line = text.readLine();
        spliter = line.split("=");
        if (spliter.size() != 2) { // format error, just skip
            QMessageBox::warning(this, "Warning", "MusicList File format  incorrect#"+ line);
            continue;
        }
        music_list.insert(spliter[0].remove(" "), musicpath+spliter[1].remove(" "));
    }

    return true;
}

void MainWindow::setDefaultMusicList(void) {
    ui->musicList->clear();
    for (auto i = music_list.begin(); i != music_list.end(); ++i)
        ui->musicList->addItem(i.key());
}

void MainWindow::destroyProcess(void) {
    playerController(exit, "");
    delete external_player;
    external_player = NULL;
}

MainWindow::~MainWindow()
{
    qDebug() << "lallllala";
    destroyProcess();
    delete ui;
    ui = NULL;
}

bool MainWindow::setCurrentMusic(const QString &music_name) {
    auto iter = music_list.find(music_name);
    if (iter != music_list.end()) {
        selected_item =  iter.key();
        ui->lbMusicName->setText(selected_item);
        setLyric(iter.value());
        // ui->timeLcd.display("00:00");
        // timeLcd->display("00:00");
    }
    return false;
}

void MainWindow::on_musicList_itemClicked(QListWidgetItem *item) {
    setCurrentMusic(item->text());
    playerController(stop, "");
    playerController(none, "");
}

void MainWindow::on_btnLoad_clicked(void) {
     QString tmp = QFileDialog::getOpenFileName(this, tr("Choose Recorded File"), ".", tr("Wave File (*.wav)"));
     if (tmp != "") {
         loadRecordFile(tmp);
     }
}

void MainWindow::loadRecordFile(const QString &recorder_name) {
    recorder_file = recorder_name;
    ui->lbRcd->setText(recorder_file);
}

void MainWindow::on_btnStart_clicked() {
    playerController(play, music_list[selected_item]+".mid");
//    qDebug() << music_list[selected_item]+".mid";
}

void MainWindow::on_btnPause_clicked() {
    playerController(pause, "");
}

void MainWindow::on_btnStop_clicked() {
    playerController(stop, "");
}

void MainWindow::playerController(enum MainWindow::PlayerState next_state, const QString &args) {
    static PlayerState ps = none;
    if (external_player == NULL) return;

    switch (next_state) {
    case none:
        external_player->write((player+" /runcmd=\"File/Remove playlist\" /hide \n").toAscii());
        break;
    case stop:
        external_player->write((player+" /stop /hide\n").toAscii());
        break;
    case pause:
        external_player->write((player+" /pause /hide \n").toAscii());
        break;
    case play:
        if (ps!=pause) {
            external_player->write((player+" /runcmd=\"File/Remove playlist\" /hide \n").toAscii());
            external_player->write((player+" /context_command:play "+args+" /hide \n").toAscii());
            // external_player.write((player+" "+args+" /hide \n").toAscii());
        } else {
            external_player->write((player+" /play /hide \n").toAscii());
            external_player->write((player+" /hide \n").toAscii());
        }

        break;
    case exit:
        qDebug() << "ziziz\n";
        external_player->write((player+" /exit\n").toAscii());
        external_player->write("exit\n");
        external_player->terminate();
        break;
    }
    ps = next_state;
}

void MainWindow::on_btnScore_clicked() {
    double res;
    evaluate(res, music_list[selected_item]+".Model", music_list[selected_item]+".info", recorder_file);
    if (!res) {
        QMessageBox::warning(this, "error!", "没有音乐数据！");
        return ;
    }
    std::stringstream ss;
    ss << res;
    std::string s;
    ss >> s;
    QString ts = s.c_str();
    ts.remove(ts.lastIndexOf("."), INT_MAX);
    ts += "%";
    ui->lbScore->setText(ts);
    QMessageBox::warning(this, tr("评价结果"), tr("与原歌曲的相似度是 ")+ts);
}

bool MainWindow::evaluate(double& score, const QString &model, const QString &info, const QString &recorder) {
    bool res ;
    qDebug() << selected_item << music_list[selected_item];
    qDebug() << QString(model);
    qDebug() << QString(info);
    qDebug() << recorder;
    const static size_t BUFFER_SIZE = 1000;
    char bf1[BUFFER_SIZE];
    char bf2[BUFFER_SIZE];
    char bf3[BUFFER_SIZE];
    char bf4[BUFFER_SIZE] = "__result.txt";

    strncpy(bf1, model.toStdString().c_str(), BUFFER_SIZE);
    strncpy(bf2, info.toStdString().c_str(), BUFFER_SIZE);
    strncpy(bf3, recorder.toStdString().c_str(), BUFFER_SIZE);

    res = STester(score, bf1, bf2, bf3, bf4);

    if (res!=false) return false;

    return true;
}

void MainWindow::setLyric(const QString &music_name) {
    QFile fs(music_name+".lrc");
    if (! fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->lbLyric->setText(tr("找不到歌词"));
        return ;
    }
    // file must in the form
    /*
       present_name = filename(without suffix)
     */
    QTextStream text(&fs);
    QString line;
    QString full;

    while (! text.atEnd()) {
        line = text.readLine();
        full += line+"\n";
    }
    ui->lbLyric->setText(full);
    return ;
}

void MainWindow::on_btnAddMusic_clicked() {
     QString tmp = QFileDialog::getOpenFileName(this, tr("Choose Midi File"), ".", tr("Midi File (*.mid)"));
     if (tmp != "")
        addMusic(tmp);
}

void MainWindow::addMusic(const QString &music_name) {
    QString path = music_name;
    path.remove(".mid");
    path.replace("\\", "/");
    // find last ocurrence
    int lastIndex = music_name.lastIndexOf("/");
    QString name = path;
    path.remove(lastIndex+1, INT_MAX);
    name.remove(0, lastIndex+1);
    qDebug() << "path:" << path << "name:" << name;
    music_list.insert(name, path+name);
    addMusicList(name);
    createDatabase(path, name);
    // key: music_name without suffix and path
    // value: music_name without suffix but with full path
    // build model and info
}

void MainWindow::createDatabase(const QString &path, const QString &name) {
    std::string filename = (path+name).toStdString()+".mid";
    std::string tmptxt_name = (path+name).toStdString()+".txt";
    qDebug() << "filename = " << path+name ;
    system(std::string("echo "+filename+" > " +tmptxt_name).c_str());
    QStringList argus;
    argus << QString(tmptxt_name.c_str()) << path;
    QProcess * pro = new QProcess(this);
    qDebug() << argus;
    pro->start(build_model_exe, argus);
}

void MainWindow::addMusicList(const QString &music_name) {
    ui->musicList->addItem(music_name);
}

///////////////////////////////////////////////////////////
/****** 下面这团是代码的关键部分(也就是该被无视的部分) *********/
/****************** 所以，请无视它吧! **********************/
///////////////////////////////////////////////////////////

/* both frame-based and note-based melody feature are extracted */
int SMelodyFeatureExtraction(char* filename, float*& pFeaBuf,int &nFeaLen, SNote *&Query, int &nNoteLen){
    /* 0. pitch extraction */
    SPitchContourExtraction(filename,pFeaBuf,nFeaLen);

    /* 1. five point median filtering */
    SMedianFilter(pFeaBuf,nFeaLen);
    if(nFeaLen<20){
        printf("Too short! please try again\n");
        if(NULL!=pFeaBuf){
            delete[] pFeaBuf;
            pFeaBuf=NULL;
        }
        return ERROR_CODE_TOO_SHORT_INPUT;
    }

    /* 2. post-processing the pitch sequence and re-sampling the pitch sequence */
    SProcessQuery(pFeaBuf,nFeaLen);
    if(nFeaLen<20){
        printf("Too short! please try again\n");
        if(NULL!=pFeaBuf){
            delete[] pFeaBuf;
            pFeaBuf=NULL;
        }
        return ERROR_CODE_TOO_SHORT_INPUT;
    }

    /* 3. note transcription */
    STranscribeQueryNote(pFeaBuf,nFeaLen, Query, nNoteLen);
    if(nFeaLen<20){
        printf("Too short! please try again\n");
        if(NULL!=pFeaBuf){
            delete[] pFeaBuf;
            pFeaBuf=NULL;
        }
        return ERROR_CODE_TOO_SHORT_INPUT;
    }

    return 0;
}

/* search one query */
int STester(double& score, char* szModel, char* szModelinfo, char* szWav, char* szOut){
    int i=0;

    //0, Load Model
    SModel *SQBHModels=NULL;
    char **szModelInfoStrs=NULL;
    int nModels=0;
    int nTotalModel=SLoadModel(szModel, SQBHModels, nModels);
    if(nTotalModel<=0){
        printf("Error on loading model!\n");
        return -1;
    }
    int nTotalSongs=SReadMelodyDBInfoFile(szModelinfo,szModelInfoStrs);
    if(nTotalModel<=0){
        printf("Error on loading modelinfo!\n");
        return -1;
    }
    //1, Feature Extraction
    float *pFeaBuf = NULL;
    int nFeaLen = 0;
    SNote *QueryNotes= NULL;
    int nNoteLen=0;
    SMelodyFeatureExtraction(szWav,pFeaBuf,nFeaLen,QueryNotes,nNoteLen);

    //2, Melody Search
    NoteBasedResStru *myEMDResStru=new NoteBasedResStru[nTotalModel];
    SNoteBasedMatch(SQBHModels, nModels, QueryNotes, nNoteLen,myEMDResStru,nFeaLen);

    // due to a sf, this function is removed
    /*
    FrameBasedResStru *myDTWResStru=new FrameBasedResStru[20];
    SFrameBasedMatch(SQBHModels, nModels, pFeaBuf, nFeaLen, myEMDResStru, 20, myDTWResStru);
    */
    //3, Finalize and print the result
    FILE *OutFile=fopen(szOut,"a+t");
    fprintf(OutFile,"%s ",szWav);
    for (i = 0; i < nTotalModel; ++i) {
        fprintf(OutFile,"%d: %s, %f; ",myEMDResStru[i].nModelID+1,szModelInfoStrs[myEMDResStru[i].nModelID], myEMDResStru[i].fScore);
        printf("%d: %s, %f\n",myEMDResStru[i].nModelID+1,szModelInfoStrs[myEMDResStru[i].nModelID], myEMDResStru[i].fScore);
    }
    score = myEMDResStru[0].fScore;
    /*
    for(i=0;i<20;i++){
        fprintf(OutFile,"%d: %s, %f; ",myDTWResStru[i].nModelID+1,szModelInfoStrs[myDTWResStru[i].nModelID], myDTWResStru[i].fScore);
        printf("%d: %s, %f\n",myDTWResStru[i].nModelID+1,szModelInfoStrs[myDTWResStru[i].nModelID], myDTWResStru[i].fScore);
    }
    */
    fprintf(OutFile,"\n");
    fclose(OutFile);

    for(i=0;i<nTotalSongs;i++){
        if(NULL!=SQBHModels[i].PhrasePos){
            delete[] SQBHModels[i].PhrasePos;
            SQBHModels[i].PhrasePos=NULL;
        }
        if(NULL!=SQBHModels[i].sNotes){
            delete[] SQBHModels[i].sNotes;
            SQBHModels[i].sNotes=NULL;
        }
    }

    for(i=0;i<nTotalSongs;i++){
        if(NULL != szModelInfoStrs[i]){
            delete[] szModelInfoStrs[i];
            szModelInfoStrs[i]=NULL;
        }
    }
    if(NULL!=myEMDResStru){
        delete[] myEMDResStru;
        myEMDResStru=NULL;
    } /*
    if(NULL!=myDTWResStru){
        delete[] myDTWResStru;
        myDTWResStru=NULL;
    } */
    if(NULL!=SQBHModels){
        delete[] SQBHModels;
        SQBHModels=NULL;
    }
    if(NULL!=szModelInfoStrs){
        delete[] szModelInfoStrs;
        szModelInfoStrs=NULL;
    }
    if(NULL!=QueryNotes){
        delete[] QueryNotes;
        QueryNotes=NULL;
    }
    if(NULL!=pFeaBuf){
        delete[] pFeaBuf;
        pFeaBuf=NULL;
    }
    return 0;
}
