#ifndef MEDIAMETADATAEXTRACTOR_H
#define MEDIAMETADATAEXTRACTOR_H

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QEventLoop>
#include <QTimer>
#include <QVariantMap>
#include <QDebug>

class MediaMetadataExtractor : public QObject
{
    Q_OBJECT
public:
    explicit MediaMetadataExtractor(QObject *parent = nullptr);

    static QVariantMap extractMetadata(const QUrl &url, int timeout = 5000);

private:
    static bool validateUrl(const QUrl &url);

    static QList<QMediaMetaData::Key> getTargetMetadataKeys();
    static QString metaDataKeyToString(QMediaMetaData::Key key);

signals:
};

#endif // MEDIAMETADATAEXTRACTOR_H
