#ifndef ALBUMMANAGER_H
#define ALBUMMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariantMap>
#include <QFile>
#include <QDir>
/**
 * @brief The AlbumManager class
 * 这个类用于管理当前载入的专辑
 *
 * 比如说，打开本地目录下的文件（单个打开），就会使用一个名为【临时】的专辑
 * 从网络中获取的专辑则会传递一个文件，这个文件会记录专辑内包含的所有音频信息，而这个类会在接收到该文件后，将该文件写入历史记录中。
 * 并记录该专辑的uid值，形成映射。每当重新载入这个专辑，都会重新获取一次专辑内包含的音频信息。
 */
class AlbumManager : public QObject
{
    Q_OBJECT
public:
    explicit AlbumManager(QObject *parent = nullptr);

    void loadAlbum(const QUrl& url);

    // 加载网络专辑文件（从文件中解析专辑UID，并记录到历史）
    void loadNetworkAlbum(const QUrl& url);

    // 设置当前专辑为本地专辑（即全是本地音频），本地专辑即是包含音频文件的目录，该函数将会读取此目录内的所有音频文件。
    void loadLocalAlbum(const QUrl& url);

    // 获取当前专辑信息
    QVariantMap getCurrentAlbum() const { return m_currentAlbum; };

    // 获取已排序后的历史专辑列表（仅包含非临时专辑，键为文件中的 UID），以最后一次载入的时间戳进行排序
    QMap<QString, QVariantMap> getSortedHistoryAlbums();

signals:
    // 当前专辑变化信号
    void currentAlbumChanged(const QVariantMap &album);

private:
    QVariantMap m_currentAlbum;                  // 当前专辑数据
    QMap<QString, QVariantMap> m_historyAlbums;  // 历史专辑（UID -> 专辑数据）
    QMap<QString, int> m_albumsUpdate;           // 更新历史 (UID -> 最后一次载入时间戳)
    QString m_historySavePath;                   // 历史记录保存路径

public:
    void loadHistoryFromFile();
    void saveHistoryToFile();
};

#endif // ALBUMMANAGER_H
