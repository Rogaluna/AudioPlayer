#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QMimeType>
#include <QMimeDatabase>
#include <QSystemTrayIcon>
#include "Tools/MediaMetadataExtractor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

    , m_bInitPlayList(false)
{
    ui->setupUi(this);

    initApplication();

    initMedia();
    initWidgets();
    initHotKeys();

    initConfigs();

    postInitialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initApplication()
{
    // 创建托盘图标
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":images/res/app_icon.ico")); // 设置图标
    m_trayIcon->setToolTip(qApp->applicationName());

    // 创建右键菜单
    m_trayMenu = new QMenu(this);
    m_quitAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::SystemLogOut), "退出", this);
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::onQuit);

    m_trayMenu->addAction(m_quitAction);
    m_trayIcon->setContextMenu(m_trayMenu);

    // 连接托盘图标激活信号
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);

    m_trayIcon->show();

    qApp->setQuitOnLastWindowClosed(false);
}

void MainWindow::initWidgets()
{
    m_group = new QButtonGroup(this);
    m_group->addButton(ui->radioBtn_list, 0);    // ID 0
    m_group->addButton(ui->radioBtn_loop, 1);  // ID 1
    m_group->addButton(ui->radioBtn_random, 2);   // ID 2

    ui->slider_volume->setSliderPosition(m_audioOutput->volume()*100);
    ui->label_volume->setText(QString::number(m_audioOutput->volume()*100));

    // // 创建侧滑面板
    m_slidePanel = new QSlidePanel(this);
    m_slidePanel->setSlideDirection(QSlidePanel::Right);
    m_slidePanel->setPanelSize(300);
    m_slidePanel->setTopOffset(0);

    // 添加侧边栏内容
    m_playListWidget = new PlayListWidget;

    // 设置到侧滑面板
    m_slidePanel->setContentWidget(m_playListWidget);

    connect(ui->actionopenAudioFile, &QAction::triggered, this, &MainWindow::openAudioFile);
    connect(ui->slider_volume, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    connect(ui->btn_playList, &QPushButton::clicked, this,&MainWindow::showPlayList);
    connect(m_playListWidget, &PlayListWidget::closeRequested, m_slidePanel, &QSlidePanel::hidePanel);
    connect(m_playListWidget, &PlayListWidget::entryClicked, this, &MainWindow::onMediaClicked);
    connect(ui->btn_playState, &QPushButton::clicked, this, &MainWindow::onPlayStateClicked);
    connect(ui->btn_previousMedia, &QPushButton::clicked, this, &MainWindow::onPreviousMediaClicked);
    connect(ui->btn_nextMedia, &QPushButton::clicked, this, &MainWindow::onNextMediaClicked);
    connect(m_group, &QButtonGroup::buttonClicked, this, &MainWindow::onRadioGroupClicked);
    connect(ui->slider_playProgress, &QSlider::sliderMoved, this, &MainWindow::onPlayProgressChanged);
    connect(ui->slider_playProgress, &QSlider::sliderPressed, this, &MainWindow::onPlayProgressPressed);
    connect(ui->slider_playProgress, &QSlider::sliderReleased, this, &MainWindow::onPlayProgressReleased);
}

void MainWindow::initMedia()
{
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayList = new QMediaPlayList(this);
    m_albumManager = new AlbumManager(this);

    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1);

    m_mediaPlayList->setPlaybackMode(QMediaPlayList::Loops);

    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStateChanged);
    connect(m_albumManager, &AlbumManager::currentAlbumChanged, this, &MainWindow::onAlbumChanged);
    connect(m_mediaPlayList, &QMediaPlayList::metadataListChanged, this, &MainWindow::onMetadataListChanged);
    connect(m_mediaPlayList, &QMediaPlayList::currentMediaChanged, this, &MainWindow::onCurrentMediaChanged);
}

void MainWindow::initHotKeys()
{
    m_playHotkey = new QHotkey(QKeySequence("Ctrl+Alt+F5"), true);
    m_previousHotkey = new QHotkey(QKeySequence("Ctrl+Alt+Left"), true);
    m_nextHotkey = new QHotkey(QKeySequence("Ctrl+Alt+Right"), true);

    connect(m_playHotkey, &QHotkey::activated, this, &MainWindow::onPlayShortcutActivated);
    connect(m_previousHotkey, &QHotkey::activated, this, &MainWindow::onPreviousShortcutActivated);
    connect(m_nextHotkey, &QHotkey::activated, this, &MainWindow::onNextShortcutActivated);
}

void MainWindow::initConfigs()
{
    if (!QFile::exists(CONFIG_FILE_NAME)) {
        qDebug() << "Config file not found, creating default...";
        QSettings initSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        initSettings.setValue("VolumnValue", 100);
        initSettings.setValue("PlayMode", static_cast<int>(QMediaPlayList::List));
        initSettings.setValue("AlbumUrl", "");
        initSettings.setValue("LastAudioUrl", "");
        initSettings.setValue("LastOpenDir", "");
        initSettings.sync();
    }

    m_settings = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
}

void MainWindow::postInitialize()
{
    // 初始化音量大小
    onVolumeChanged(m_settings->value("VolumnValue", 100).toInt());
    // 初始化播放模式
    onRadioGroupClicked(
        m_group->button(m_settings->value("PlayMode", static_cast<int>(QMediaPlayList::List)).toInt())
    );
    // 初始化播放列表
    m_albumManager->loadAlbum(m_settings->value("AlbumUrl", "").toString());

    m_bInitPlayList = true;

    // 载入音乐
    QString lasturl = m_settings->value("LastAudioUrl", "").toString();
    m_mediaPlayList->setMediaByUrl(lasturl);
}

void MainWindow::reloadPlayList(const QVector<QVariantMap>& entries)
{
    m_playListWidget->clearEntries();
    m_playListWidget->addMediaEntries(entries);
}

void MainWindow::openAudioFile()
{
    // 打开音频文件，置入播放列表和当前播放
    QStringList audioMimeFilters;
    const QList<QMimeType> mimes = QMimeDatabase().allMimeTypes();
    foreach (const QMimeType &mime, mimes) {
        if (mime.name().startsWith("audio/")) {
            audioMimeFilters += mime.globPatterns();
        }
    }

    // 生成过滤器字符串
    QString filter = tr("Supported Audio Files") + " (";
    filter += audioMimeFilters.join(" ") + ");;" +
              tr("All Files") + " (*)";

    QUrl fileUrl = QFileDialog::getOpenFileUrl(this,
        tr("Open Audio File"),
        m_settings->value("LastOpenDir", QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()))).toUrl(),
        filter);

    if (!fileUrl.isEmpty()) {
        // 如果成功打开，设置当前专辑为【临时专辑】,并将专辑内（当前音频包括在内）的音频同步导入到播放列表中
        m_albumManager->loadAlbum(fileUrl);

        if (!fileUrl.isLocalFile()) {
            qWarning() << "URL is not a local directory";
            return;
        }

        QString originalPath = fileUrl.toLocalFile();
        QFileInfo fileInfo(originalPath);

        // 如果是文件路径，自动修正为所在目录
        QString dirPath = fileInfo.isDir() ? originalPath : fileInfo.dir().absolutePath();

        m_settings->setValue("LastOpenDir", dirPath);
    }
}

void MainWindow::onQuit()
{
    m_trayIcon->hide(); // 移除托盘图标
    qApp->quit();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick) {
        showNormal();
        activateWindow();
    }
}

void MainWindow::onStateChanged(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlaybackState::PausedState:
    {
        ui->label_tips->setText("暂停中...");
        ui->btn_playState->setText("播放");
    }
        break;
    case QMediaPlayer::PlaybackState::PlayingState:
    {
        ui->label_tips->setText("正在播放：");
        ui->btn_playState->setText("暂停");
    }
        break;
    case QMediaPlayer::PlaybackState::StoppedState:
    {
        ui->label_mediaName->setText("");
        ui->label_tips->setText("等待播放...");
        ui->btn_playState->setText("播放");
    }
        break;
    default:
        break;
    }
}

void MainWindow::onPositionChanged(qint64 pos)
{
    if (!ui->slider_playProgress->isSliderDown())
    {
        ui->slider_playProgress->setValue(pos);

        qint64 currentSeconds = pos / 1000;
        QTime time = QTime(0, 0, 0).addSecs(currentSeconds);
        ui->label_currentDuration->setText((time.hour() > 0) ? time.toString("HH:mm:ss") : time.toString("mm:ss"));
    }
}

void MainWindow::onDurationChanged(qint64 dur)
{
    ui->slider_playProgress->setRange(0, dur);

    qint64 totalSeconds = dur / 1000;
    QTime time = QTime(0, 0, 0).addSecs(totalSeconds);
    ui->label_totleTime->setText((time.hour() > 0) ? time.toString("HH:mm:ss") : time.toString("mm:ss"));
}

void MainWindow::onMediaStateChanged(QMediaPlayer::MediaStatus state)
{
    if (state == QMediaPlayer::EndOfMedia) {
        switch (m_mediaPlayList->getPlaybackMode()) {
        case QMediaPlayList::Loops:
        {
            m_mediaPlayList->setCurrentMedia(m_mediaPlayList->getCurrentMediaIterator());
            m_mediaPlayer->play();
        }
        break;
        default:
            m_mediaPlayList->setNextMedia();
            m_mediaPlayer->play();
            break;
        }
    }
}

void MainWindow::onVolumeChanged(int pos)
{
    ui->slider_volume->setValue(pos);
    m_audioOutput->setVolume((float)pos/100.f);

    ui->label_volume->setText(QString::number(pos));

    m_settings->setValue("VolumnValue", pos);
}

void MainWindow::onAlbumChanged(const QVariantMap &album)
{
    ui->label_albumName->setText(album["name"].toString());

    // 将专辑中的音频 url 载入到播放列表中
    auto tracks = album["tracks"].toStringList();
    QVector<QVariantMap> metadataList;
    foreach (QString track, tracks) {
        QVariantMap metadata = MediaMetadataExtractor::extractMetadata(track);
        metadataList.append(metadata);
    }

    m_mediaPlayList->setPlayList(metadataList);

    // 保存配置值
    m_settings->setValue("AlbumUrl", album["url"].toString());  // 修改当前专辑值
}

void MainWindow::onMetadataListChanged()
{
    auto metadataList = m_mediaPlayList->getMetadataList();

    m_playListWidget->addMediaEntries(metadataList);
}

void MainWindow::onCurrentMediaChanged()
{
    QVariantMap metadata = m_mediaPlayList->getCurrentMediaValue();

    if (!metadata.isEmpty())
    {
        if (m_mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
        {
            // 如果在切换之前已经播放了媒体，那么依旧保持播放
            m_mediaPlayer->setSource(metadata["Url"].toString());
            m_mediaPlayer->play();
        }
        else
        {
            m_mediaPlayer->setSource(metadata["Url"].toString());
        }

        ui->label_mediaName->setText(metadata["Title"].toString());
    }

    if (m_bInitPlayList)
    {
        m_settings->setValue("LastAudioUrl", metadata["Url"].toString());
    }
}

void MainWindow::onMediaClicked(const QVariantMap &metadata)
{
    // m_mediaPlayer->setSource(metadata["Url"].toString());
    m_mediaPlayList->setMediaByUrl(metadata["Url"].toString());
    m_mediaPlayer->play();
}

void MainWindow::onPlayStateClicked()
{
    QMediaPlayer::PlaybackState state = m_mediaPlayer->playbackState();
    switch (state) {
    case QMediaPlayer::PlaybackState::PausedState:
    {
        if(m_mediaPlayer->source().isValid())
        {
            m_mediaPlayer->play();
        }
    }
    break;
    case QMediaPlayer::PlaybackState::PlayingState:
    {
        m_mediaPlayer->pause();
    }
    break;
    case QMediaPlayer::PlaybackState::StoppedState:
    {
        if(m_mediaPlayer->source().isValid())
        {
            m_mediaPlayer->play();
        }
    }
    break;
    default:
        break;
    }
}

void MainWindow::onPreviousMediaClicked()
{
    m_mediaPlayList->setPreviousMedia();
}

void MainWindow::onNextMediaClicked()
{
    m_mediaPlayList->setNextMedia();
}

void MainWindow::onRadioGroupClicked(QAbstractButton *btn)
{
    btn->setChecked(true);
    int id = m_group->id(btn);
    m_mediaPlayList->setPlaybackMode(static_cast<QMediaPlayList::EPlayMode>(id));

    m_settings->setValue("PlayMode", id);
}

void MainWindow::onPlayProgressChanged(int value)
{
    m_mediaPlayer->setPosition(value);

    qint64 currentSeconds = value / 1000;
    QTime time = QTime(0, 0, 0).addSecs(currentSeconds);
    ui->label_currentDuration->setText((time.hour() > 0) ? time.toString("HH:mm:ss") : time.toString("mm:ss"));
}

void MainWindow::onPlayProgressPressed()
{
    m_bPlayState = m_mediaPlayer->isPlaying();
    if (m_bPlayState)
    {
        m_mediaPlayer->pause();
    }
}

void MainWindow::onPlayProgressReleased()
{
    if (m_bPlayState)
    {
        m_mediaPlayer->play();
    }
}

void MainWindow::onPlayShortcutActivated()
{
    onPlayStateClicked();
}

void MainWindow::onPreviousShortcutActivated()
{
    onPreviousMediaClicked();
}

void MainWindow::onNextShortcutActivated()
{
    onNextMediaClicked();
}

void MainWindow::showPlayList()
{
    m_slidePanel->showPanel();
}
