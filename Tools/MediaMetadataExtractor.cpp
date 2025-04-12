#include "MediaMetadataExtractor.h"

#include <QFileInfo>

MediaMetadataExtractor::MediaMetadataExtractor(QObject *parent)
    : QObject{parent}
{}

QVariantMap MediaMetadataExtractor::extractMetadata(const QUrl &url, int timeout)
{
    QVariantMap metadata;

    if (!validateUrl(url)) {
        return metadata;
    }

    QMediaPlayer player;
    QEventLoop eventLoop;
    QTimer timeoutTimer;

    timeoutTimer.setSingleShot(true);

    QObject::connect(&player, &QMediaPlayer::metaDataChanged, &player, [&](){
        if (!timeoutTimer.isActive()) return;
        eventLoop.quit();
    });

    QObject::connect(&timeoutTimer, &QTimer::timeout, &player, [&](){
        qWarning() << "Metadata extraction timed out for:" << url;
        eventLoop.quit();
    });

    player.setSource(url);
    timeoutTimer.start(timeout);
    eventLoop.exec();

    if (!timeoutTimer.isActive()) {
        return metadata;
    }
    timeoutTimer.stop();

    const QMediaMetaData mediaData = player.metaData();
    const auto keys = getTargetMetadataKeys();

    for (const auto &key : keys) {
        QVariant value = mediaData.value(key);
        if (!value.isNull()) {
            metadata[metaDataKeyToString(key)] = value;
            continue;
        }
    }

    if (metadata[metaDataKeyToString(QMediaMetaData::Title)].isNull())   // 当取标题失败的时候
    {
        // 从文件字符串中解析
        QString path = url.path();
        QString filenameWithExtension = QFileInfo(path).fileName();
        // 去除扩展名
        QString filename = QFileInfo(filenameWithExtension).completeBaseName();
        // 分割作者和标题
        QStringList parts = filename.split(" - ");
        QString title;
        if (parts.size() >= 2) {
            parts.removeFirst();
            title = parts.first().trimmed();
        } else {
            // 无法分割时，整个文件名作为标题
            title = filename.trimmed();
        }

        metadata[metaDataKeyToString(QMediaMetaData::Title)] = title;
    }

    if (metadata[metaDataKeyToString(QMediaMetaData::Author)].isNull())  // 当取作者失败的时候
    {
        // 从贡献艺术家中复制
        if (!metadata[metaDataKeyToString(QMediaMetaData::ContributingArtist)].isNull())
        {
            metadata[metaDataKeyToString(QMediaMetaData::Author)] = metadata[metaDataKeyToString(QMediaMetaData::ContributingArtist)];

        }
        // 从专辑作者中复制
        else if (!mediaData.value(QMediaMetaData::AlbumArtist).isNull())
        {
            metadata[metaDataKeyToString(QMediaMetaData::Author)] = metadata[metaDataKeyToString(QMediaMetaData::AlbumArtist)];
        }

        else
        {
            // 从文件字符串中解析
            QString path = url.path();
            QString filenameWithExtension = QFileInfo(path).fileName();
            // 去除扩展名
            QString filename = QFileInfo(filenameWithExtension).completeBaseName();
            // 分割作者和标题
            QStringList parts = filename.split(" - ");
            QString artist;
            if (parts.size() >= 2) {
                artist = parts.first().trimmed();
            } else {
                // 无法分割时，使用默认值
                artist = "Unknown";
            }

            metadata[metaDataKeyToString(QMediaMetaData::Author)] = artist;
        }
    }

    if (player.duration() > 0) {
        metadata.insert("Duration", player.duration());
    }

    metadata.insert("Url", url);

    return metadata;
}

bool MediaMetadataExtractor::validateUrl(const QUrl &url)
{
    if (!url.isLocalFile()) {
        qWarning() << "Remote URLs are not supported:" << url;
        return false;
    }

    const QString filePath = url.toLocalFile();
    if (!QFile::exists(filePath)) {
        qWarning() << "File not found:" << filePath;
        return false;
    }

    return true;
}

QList<QMediaMetaData::Key> MediaMetadataExtractor::getTargetMetadataKeys()
{
    QList<QMediaMetaData::Key> keys;
    keys << QMediaMetaData::Title
         << QMediaMetaData::Author
         << QMediaMetaData::Comment
         << QMediaMetaData::Description
         << QMediaMetaData::Genre
         << QMediaMetaData::Date
         << QMediaMetaData::Language
         << QMediaMetaData::Publisher
         << QMediaMetaData::Copyright
         << QMediaMetaData::Url
         << QMediaMetaData::Duration
         << QMediaMetaData::MediaType
         << QMediaMetaData::FileFormat
         << QMediaMetaData::AudioBitRate
         << QMediaMetaData::AudioCodec
         << QMediaMetaData::VideoBitRate
         << QMediaMetaData::VideoCodec
         << QMediaMetaData::VideoFrameRate
         << QMediaMetaData::AlbumTitle
         << QMediaMetaData::AlbumArtist
         << QMediaMetaData::ContributingArtist
         << QMediaMetaData::TrackNumber
         << QMediaMetaData::Composer
         << QMediaMetaData::LeadPerformer
         << QMediaMetaData::ThumbnailImage
         << QMediaMetaData::CoverArtImage
         << QMediaMetaData::Orientation
         << QMediaMetaData::Resolution;
    return keys;
}

QString MediaMetadataExtractor::metaDataKeyToString(QMediaMetaData::Key key)
{
    switch (key) {
    // 基础信息
    case QMediaMetaData::Title:        return "Title";
    case QMediaMetaData::Author:       return "Author";
    case QMediaMetaData::Comment:      return "Comment";
    case QMediaMetaData::Description:  return "Description";
    case QMediaMetaData::Genre:        return "Genre";
    case QMediaMetaData::Date:         return "Date";

        // 版权信息
    case QMediaMetaData::Language:     return "Language";
    case QMediaMetaData::Publisher:    return "Publisher";
    case QMediaMetaData::Copyright:    return "Copyright";
    case QMediaMetaData::Url:          return "Url";

        // 媒体属性
    case QMediaMetaData::Duration:     return "Duration";
    case QMediaMetaData::MediaType:    return "MediaType";
    case QMediaMetaData::FileFormat:   return "FileFormat";

        // 音频属性
    case QMediaMetaData::AudioBitRate: return "AudioBitRate";
    case QMediaMetaData::AudioCodec:   return "AudioCodec";

        // 视频属性
    case QMediaMetaData::VideoBitRate: return "VideoBitRate";
    case QMediaMetaData::VideoCodec:   return "VideoCodec";
    case QMediaMetaData::VideoFrameRate: return "VideoFrameRate";

        // 专辑信息
    case QMediaMetaData::AlbumTitle:   return "AlbumTitle";
    case QMediaMetaData::AlbumArtist:  return "AlbumArtist";
    case QMediaMetaData::ContributingArtist: return "ContributingArtist";
    case QMediaMetaData::TrackNumber:  return "TrackNumber";
    case QMediaMetaData::Composer:     return "Composer";
    case QMediaMetaData::LeadPerformer: return "LeadPerformer";

        // 图像信息
    case QMediaMetaData::ThumbnailImage: return "ThumbnailImage";
    case QMediaMetaData::CoverArtImage:  return "CoverArtImage";

        // 其他
    case QMediaMetaData::Orientation:  return "Orientation";
    case QMediaMetaData::Resolution:   return "Resolution";

        // 默认处理未知键
    default: return QString("UnknownKey_%1").arg(static_cast<int>(key));
    }
}
