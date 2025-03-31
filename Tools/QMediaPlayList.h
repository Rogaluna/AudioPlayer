#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QVariant>

class QMediaPlayList : public QObject
{
    Q_OBJECT
public:
    enum class EPlayMode{
        List,    // 列表播放
        Loops,    // 循环播放
        Rand,    // 随机播放
    };

    using enum EPlayMode;

public:
    explicit QMediaPlayList(QObject *parent = nullptr);

    EPlayMode getPlaybackMode() const;
    void setPlaybackMode(EPlayMode mode);

    void append(const QVariantMap& metadata);
    void append(const QVector<QVariantMap>& metadataList);

    void setPlayList(const QVector<QVariantMap>& metadataList);

    QVariantMap getCurrentMediaValue();
    QVector<QVariantMap>::ConstIterator getCurrentMediaIterator();
    const QVector<QVariantMap>& getMetadataList() const;

    void setCurrentMedia(QVector<QVariantMap>::ConstIterator it);

    // 设置音频，不一定会播放
    void setNextMedia();
    void setPreviousMedia();

private:
    QVector<QVariantMap> m_metadataList;
    QVector<QVariantMap>::ConstIterator m_currentMedia;
    QVector<QVector<QVariantMap>::iterator> m_randomMediaList;

    EPlayMode m_playbackMode;

private:
    void updateCurrentMedia();
    void shuffleIterators();

signals:
    void metadataListChanged();
    void currentMediaChanged();
};

#endif // QMEDIAPLAYLIST_H
