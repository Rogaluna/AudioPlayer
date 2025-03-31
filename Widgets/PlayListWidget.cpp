#include "PlayListWidget.h"
#include "ui_PlayListWidget.h"

#include <QPainter>
#include <QTime>
#include <QTimer>

PlayListWidget::PlayListWidget(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::PlayListWidget)
{
    ui->setupUi(this);

    QListView *listView = ui->listView;
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_model = new QStandardItemModel(listView);

    // for(int i = 0; i < 100; i++)
    // {
    //     QStandardItem* item = new QStandardItem(QString("Item %1").arg(i));
    //     m_model->appendRow(item);
    // }

    listView->setModel(m_model);

    connect(listView, &QListView::doubleClicked, this, &PlayListWidget::onItemDoubleClicked);
}

PlayListWidget::~PlayListWidget()
{
    delete ui;
}

void PlayListWidget::addMediaEntry(const QVariantMap &metadata)
{
    QString mediaString = QString("%1 - %2").arg(metadata["Title"].toString(), metadata["ContributingArtist"].toString());
    QStandardItem* item = new QStandardItem(mediaString);

    item->setData(metadata, Qt::UserRole);
    m_model->appendRow(item);
}

void PlayListWidget::addMediaEntries(const QVector<QVariantMap> &metadataList)
{
    for (const auto &metadata : metadataList) {
        addMediaEntry(metadata);
    }
}

void PlayListWidget::clearEntries()
{
    m_model->clear();
}

void PlayListWidget::onItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        // QString text = m_model->itemFromIndex(index)->text();
        QVariantMap metadata = m_model->itemFromIndex(index)->data(Qt::UserRole).toMap();

        emit entryClicked(metadata);
    }
}
