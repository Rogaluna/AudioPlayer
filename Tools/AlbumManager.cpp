#include "AlbumManager.h"

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QUrl>
#include <QVariantList>

AlbumManager::AlbumManager(QObject *parent)
    : QObject{parent}
{
    // 初始化历史记录存储路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_historySavePath = dir.filePath("album_history.json");

    loadHistoryFromFile();
}

void AlbumManager::loadAlbum(const QUrl &url)
{
    // 使用小写协议名称进行统一判断
    const QString scheme = url.scheme().toLower();

    static const QSet<QString> networkSchemes{
        "http", "https", "ftp", "ftps", "sftp"
    };

    // 判断逻辑分层处理
    if (networkSchemes.contains(scheme)) {          // 网络协议判断
        loadNetworkAlbum(url);
    } else if ( scheme == "file" ||
               (scheme.isEmpty() && QFile::exists(url.toLocalFile()))) { // 本地协议判断
        loadLocalAlbum(url);
    } else {                                // 异常情况处理
        qWarning().nospace()
            << "Unsupported URL scheme ["
            << url.scheme()
            << "] for: "
            << url.toString();
    }
}

void AlbumManager::loadNetworkAlbum(const QUrl &url)
{
    if (!url.isLocalFile()) {
        qWarning() << "URL is not a local file";
        return;
    }

    QString filePath = url.toLocalFile();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return;
    }

    // 解析 JSON 元数据
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return;
    }

    QVariantMap albumData = doc.toVariant().toMap();
    QString uid = albumData.value("uid").toString();
    if (uid.isEmpty()) {
        qWarning() << "Missing UID in album file";
        return;
    }

    // 更新历史记录
    m_historyAlbums[uid] = albumData;
    m_albumsUpdate[uid] = QDateTime::currentSecsSinceEpoch();

    // 保存更新后的历史记录
    saveHistoryToFile();

    // 更新当前专辑并触发信号
    m_currentAlbum = albumData;
    emit currentAlbumChanged(m_currentAlbum);
}

void AlbumManager::loadLocalAlbum(const QUrl &url)
{
    if (!url.isLocalFile()) {
        qWarning() << "URL is not a local directory";
        return;
    }

    QString originalPath = url.toLocalFile();
    QFileInfo fileInfo(originalPath);

    // 如果是文件路径，自动修正为所在目录
    QString dirPath = fileInfo.isDir() ? originalPath : fileInfo.dir().absolutePath();
    QDir dir(dirPath);
    if (!dir.exists()) {
        qWarning() << "Directory does not exist:" << dirPath;
        return;
    }

    QString dirName = dir.dirName();

    // 获取音频文件列表（示例扩展名）
    QStringList audioFiles;
    QMimeDatabase mimeDatabase; // MIME 类型检测器

    QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        // 检测是否是音频类型（根据 MIME 类型前缀）
        if (mimeType.name().startsWith("audio/")) {
            QUrl fileUrl = QUrl::fromLocalFile(filePath);
            audioFiles.append(fileUrl.toString());
        }
    }

    // 构建临时专辑数据
    QVariantMap localAlbum;
    localAlbum["name"] = dirName;
    localAlbum["desc"] = "";
    localAlbum["tracks"] = audioFiles;
    localAlbum["uid"] = dirPath; // 以文件夹路径做标识
    localAlbum["url"] = "file:///" + dirPath;

    // 更新历史记录
    m_historyAlbums[dirPath] = localAlbum;
    m_albumsUpdate[dirPath] = QDateTime::currentSecsSinceEpoch();

    // 保存更新后的历史记录
    saveHistoryToFile();

    // 更新当前专辑并触发信号
    m_currentAlbum = localAlbum;
    emit currentAlbumChanged(m_currentAlbum);
}

QMap<QString, QVariantMap> AlbumManager::getSortedHistoryAlbums()
{
    // 按时间戳排序的 UID 列表
    QList<QPair<int, QString>> sortedEntries;
    for (auto it = m_albumsUpdate.constBegin(); it != m_albumsUpdate.constEnd(); ++it) {
        sortedEntries.append(qMakePair(it.value(), it.key()));
    }

    // 降序排序（最新在前）
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const QPair<int, QString>& a, const QPair<int, QString>& b) {
                  return a.first > b.first;
              });

    // 将排序后的结果存入 QMap（注意：QMap 按键排序，此处仅为兼容接口）
    QMap<QString, QVariantMap> result;
    for (const auto& entry : sortedEntries) {
        QString uid = entry.second;
        result[uid] = m_historyAlbums.value(uid);
    }

    return result;
}

void AlbumManager::loadHistoryFromFile()
{
    QFile file(m_historySavePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isObject()) {
        QJsonObject root = doc.object();
        QJsonArray historyArray = root["history"].toArray();

        for (const QJsonValue& val : historyArray) {
            QJsonObject obj = val.toObject();
            QString uid = obj["uid"].toString();
            QVariantMap data = obj["data"].toObject().toVariantMap();
            int timestamp = obj["timestamp"].toInt();
            m_historyAlbums[uid] = data;
            m_albumsUpdate[uid] = timestamp;
        }
    }
}

void AlbumManager::saveHistoryToFile()
{
    QJsonArray historyArray;
    for (auto it = m_historyAlbums.begin(); it != m_historyAlbums.end(); ++it) {
        QJsonObject obj;
        obj["uid"] = it.key();
        obj["data"] = QJsonObject::fromVariantMap(it.value());
        obj["timestamp"] = m_albumsUpdate.value(it.key());
        historyArray.append(obj);
    }

    QJsonObject root;
    root["history"] = historyArray;
    QJsonDocument doc(root);

    QFile file(m_historySavePath);
    if (file.open(QIODevice::WriteOnly)) {    
        file.write(doc.toJson());
    }
}
