#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QListWidget>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <QProcess>

class QAction;
class QTableWidget;
class QLCDNumber;

namespace Ui {
class MainWindow;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    enum PlayerState {none, play, pause, stop, exit};
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    // return music list
    QStringList Musics(void) { return QStringList(music_list.keys()); }
    /*
     * @impl: read config.txt and set musicpath curresponding to the value.
     */
    bool setDefaultMusicPath(void);
    /*
     * @impl: read music list from musicpath
     *        the list is in the form:
     *          display_name = filename
     * @return: false if failed
     */
    bool loadDefaultMusic(const QString& filename);
    /*
     * @impl: set MusicList, this function communicates with ui->musicList directly
     */
    void setDefaultMusicList(void);
    /*
     * @params: the present name of the music
     * @impl: set related labels and related values
     * @return: false if the name is not in the list
     */
    bool setCurrentMusic(const QString& music_name);
    /*
     * @params: set RecordFile full path with suffix
     * @impl: do not validate the existance of the file
     */
    void loadRecordFile(const QString& recorder_name);
    /*
     * @params: the full path of the music without suffix
     * @return: true if succeed
     */
    void addMusic(const QString& music_name);
    /*
     * @params: the music name without suffix and pathInformation
     */
    void addMusicList(const QString& music_name);
    /*
     * @params: the fullname with path of the model, info, with the full path of the recorder.
     * @return: the score. false if failed
     */
    bool evaluate(double& result, const QString& model, const QString& info, const QString& recorder);
    // the full name in the filesystem without suffix
    void setLyric(const QString& music_name);
    void playerController(enum MainWindow::PlayerState next_state, const QString& args);
public slots:
    void readoutput(void);
    void destroyProcess(void);
    void on_musicList_itemClicked(QListWidgetItem * item);
    void on_btnLoad_clicked(void);
    void on_btnScore_clicked(void);
    void on_btnAddMusic_clicked(void);
    void on_btnStart_clicked(void);
    void on_btnPause_clicked(void);
    void on_btnStop_clicked(void);
private:
    /*
     * @params: path: path to the music. name: pure name
     */
    void createDatabase(const QString& path, const QString& name);
private:
    Ui::MainWindow *ui;
    // note: shall be a class inherited from QListWdgit.
    // so that it can maintain the data itself.
    QMap<QString, QString> music_list;
    QString musicpath;
	QString build_model_exe;
    QString player;
    QString selected_item;
    QString recorder_file;
    QProcess * external_player; // currently foobar
};

#endif // MAINWINDOW_HPP
