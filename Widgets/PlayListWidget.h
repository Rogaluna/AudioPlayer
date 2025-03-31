#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class PlayListWidget;
}
QT_END_NAMESPACE

class PlayListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayListWidget(QWidget *parent = nullptr);
    virtual ~PlayListWidget();

    void addMediaEntry(const QVariantMap& metadata);
    void addMediaEntries(const QVector<QVariantMap>& metadataList);

    void clearEntries();
signals:
    void closeRequested();
    void entryClicked(const QVariantMap&);

private:
    Ui::PlayListWidget* ui;

    QStandardItemModel* m_model;

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
};

#endif // PLAYLISTWIDGET_H
