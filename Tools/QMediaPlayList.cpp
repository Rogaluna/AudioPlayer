#include "QMediaPlayList.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <random>

QMediaPlayList::QMediaPlayList(QObject *parent)
    : QObject{parent}
{
    // 初始化历史记录存储路径
    QString appDataPath = "./";
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_historySavePath = dir.filePath("play_history.json");

    loadHistoryFromFile();
}

QMediaPlayList::EPlayMode QMediaPlayList::getPlaybackMode() const
{
    return m_playbackMode;
}

void QMediaPlayList::setPlaybackMode(EPlayMode mode)
{
    m_playbackMode = mode;
    if (m_playbackMode == EPlayMode::Rand)
    {
        // 如果主动设置随机模式，生成随机播放列表
        shuffleIterators();
    }
}

void QMediaPlayList::append(const QVariantMap& metadata)
{
    append(QVector<QVariantMap>({metadata}));
}

void QMediaPlayList::append(const QVector<QVariantMap> &metadataList)
{
    m_metadataList.append(metadataList);

    // 更新当前播放的媒体
    updateCurrentMedia();
    if (m_playbackMode == EPlayMode::Rand)
    {
        // 当播放模式为随机且载入了元数据时，生成随机播放列表
        shuffleIterators();
    }
}

void QMediaPlayList::setPlayList(const QVector<QVariantMap> &metadataList)
{
    m_metadataList.clear();

    append(metadataList);
}

QVariantMap QMediaPlayList::getCurrentMediaValue()
{
    QVariantMap metadata = *m_currentMedia;
    return metadata;
}

void QMediaPlayList::setMediaByUrl(const QString &url)
{
    // 如果列表中有对应 url 的音乐，就进行设置，否则不设置
    for (auto it = m_metadataList.begin(); it < m_metadataList.end(); it ++)
    {
        QVariantMap metadata = *it;
        if (metadata["Url"].toString() == url)
        {
            setCurrentMedia(it);
        }
    }

}

QVector<QVariantMap>::ConstIterator QMediaPlayList::getCurrentMediaIterator()
{
    return m_currentMedia;
}

const QVector<QVariantMap> &QMediaPlayList::getMetadataList() const
{
    return m_metadataList;
}

void QMediaPlayList::setCurrentMedia(QVector<QVariantMap>::ConstIterator it)
{
    if (!it->isEmpty())
    {
        m_currentMedia = it;
    }

    emit currentMediaChanged();
}

void QMediaPlayList::setNextMedia()
{
    if (getCurrentMediaIterator() == nullptr) { return ; }

    if (m_playbackMode == EPlayMode::Rand)
    {
        // 如果是处于随机模式下，使用随机列表中的顺序

        // 找到当前歌曲在随机列表中的位置，然后进行加减。
        int i = 0;
        for (; i < m_randomMediaList.size(); i++)
        {
            auto it = m_randomMediaList[i];
            if (getCurrentMediaIterator() == it)
            {
                break;
            }
        }
        if (getCurrentMediaIterator() != *(m_randomMediaList.end() - 1))
        {
            setCurrentMedia(m_randomMediaList[i + 1]);
        }
        else
        {
            setCurrentMedia(*m_randomMediaList.begin());
        }
    }
    else
    {
        if (getCurrentMediaIterator() != getMetadataList().end() - 1)
        {
            setCurrentMedia(getCurrentMediaIterator() + 1);
        }
        else
        {
            setCurrentMedia(getMetadataList().begin());
        }
    }
}

void QMediaPlayList::setPreviousMedia()
{
    if (m_playbackMode == EPlayMode::Rand)
    {
        // 如果是处于随机模式下，使用随机列表中的顺序

        // 找到当前歌曲在随机列表中的位置，然后进行加减。
        int i = 0;
        for (; i < m_randomMediaList.size(); i++)
        {
            auto it = m_randomMediaList[i];
            if (getCurrentMediaIterator() == it)
            {
                break;
            }
        }
        if (getCurrentMediaIterator() != *m_randomMediaList.begin())
        {
            setCurrentMedia(m_randomMediaList[i - 1]);
        }
        else
        {
            setCurrentMedia(*(m_randomMediaList.end()-1));
        }
    }
    else
    {
        if (getCurrentMediaIterator() == nullptr) { return ; }

        if (getCurrentMediaIterator() != getMetadataList().begin())
        {
            setCurrentMedia(getCurrentMediaIterator() - 1);
        }
        else
        {
            setCurrentMedia(getMetadataList().end() - 1);
        }
    }
}

void QMediaPlayList::updateCurrentMedia()
{
    // 如果播放列表没有音频信息，重置空
    if (m_metadataList.isEmpty())
    {
        setCurrentMedia(m_metadataList.end());
        return;
    }

    // 当播放列表中有值

    // 如果当前播放信息为空，设置当前播放信息为首个
    if (m_currentMedia == nullptr || m_currentMedia == m_metadataList.end())
    {
        setCurrentMedia(m_metadataList.begin());
    }

    // 如果当前播放信息不为空

    // // 当更新后的列表中还存在当前指向的播放信息，直接返回
    // if (m_vmediaInfo.contains(*m_pcurrentMedia))
    // {
    //     return;
    // }

    // // 否则需要重设
    // else
    // {
    //     m_pcurrentMedia = &m_vmediaInfo.front();
    // }

    QVariantMap currentMediaMetadata = *m_currentMedia;
    // 更新历史记录
    m_historyMedia[currentMediaMetadata["Url"].toString()] = currentMediaMetadata;
    m_mediaUpdate[currentMediaMetadata["Url"].toString()] = QDateTime::currentSecsSinceEpoch();

    saveHistoryFromFile();

    emit metadataListChanged();
}

void QMediaPlayList::shuffleIterators()
{
    m_randomMediaList.clear();

    // 步骤1：填充原始迭代器
    for (auto it = m_metadataList.begin(); it != m_metadataList.end(); ++it) {
        m_randomMediaList.append(it);
    }

    // 步骤2：Fisher-Yates Shuffle
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = m_randomMediaList.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<> distrib(0, i);
        int j = distrib(gen);
        qSwap(m_randomMediaList[i], m_randomMediaList[j]);
    }
}

void QMediaPlayList::loadHistoryFromFile()
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
            QString url = obj["url"].toString();
            QVariantMap data = obj["data"].toObject().toVariantMap();
            int timestamp = obj["timestamp"].toInt();
            m_historyMedia[url] = data;
            m_mediaUpdate[url] = timestamp;
        }
    }
}

void QMediaPlayList::saveHistoryFromFile()
{
    QJsonArray historyArray;

    for (auto it = m_historyMedia.begin(); it != m_historyMedia.end(); ++it) {
        QJsonObject obj;
        obj["url"] = it.key();
        obj["data"] = QJsonObject::fromVariantMap(it.value());
        obj["timestamp"] = m_mediaUpdate.value(it.key());
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


