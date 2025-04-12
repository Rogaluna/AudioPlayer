#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Tools/AlbumManager.h"
#include "Tools/QMediaPlayList.h"
#include "Widgets/QSlidePanel.h"
#include "Widgets/PlayListWidget.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QButtonGroup>
#include <QHotkey>
#include <QSettings>
#include <QSystemTrayIcon>

#define CONFIG_FILE_NAME "config.ini"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void initApplication();

    void initWidgets();
    void initMedia();
    void initHotKeys();

    void initConfigs();

    void postInitialize();

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_quitAction;

    QAudioOutput* m_audioOutput;
    QMediaPlayer* m_mediaPlayer;

    QMediaPlayList* m_mediaPlayList;
    AlbumManager* m_albumManager;

    QSlidePanel *m_slidePanel;
    PlayListWidget *m_playListWidget;

    QButtonGroup *m_group;

private:
    QHotkey *m_playHotkey;
    QHotkey *m_previousHotkey;
    QHotkey *m_nextHotkey;

private:
    bool m_bPlayState;

    QSettings* m_settings;

private:

    uint8_t m_bInitPlayList : 1; // 初始化播放列表

private:
    // 重载播放列表数据
    void reloadPlayList(const QVector<QVariantMap>& entries);

private slots:
    void openAudioFile();
    void onQuit();

    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void onStateChanged(QMediaPlayer::PlaybackState state);
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void onMediaStateChanged(QMediaPlayer::MediaStatus state);

    void onVolumeChanged(int pos);

    void onAlbumChanged(const QVariantMap &album);
    void onMetadataListChanged();
    void onCurrentMediaChanged();
    void onMediaClicked(const QVariantMap& metadata);
    void onPlayStateClicked();
    void onPreviousMediaClicked();
    void onNextMediaClicked();

    void onRadioGroupClicked(QAbstractButton *btn);
    void onPlayProgressChanged(int value);
    void onPlayProgressPressed();
    void onPlayProgressReleased();

    void showPlayList();

private slots:
    // 快捷键触发槽
    void onPlayShortcutActivated();
    void onPreviousShortcutActivated();
    void onNextShortcutActivated();
};
#endif // MAINWINDOW_H
