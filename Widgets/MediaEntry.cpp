#include "MediaEntry.h"
#include "ui_MediaEntry.h"

MediaEntry::MediaEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaEntry)
{
    ui->setupUi(this);
}

void MediaEntry::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void MediaEntry::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(m_metadata);

    QWidget::mouseReleaseEvent(event);
}

void MediaEntry::enterEvent(QEnterEvent *event)
{
    setCursor(Qt::PointingHandCursor);
    QWidget::enterEvent(event);
}

void MediaEntry::leaveEvent(QEvent *event)
{
    unsetCursor();
    QWidget::leaveEvent(event);
}

void MediaEntry::setEntry(const QVariantMap &metadata)
{
    m_metadata = metadata;

    update();
}

void MediaEntry::update()
{
    ui->label_mediaName->setText(m_metadata["Title"].toString());
    ui->label_author->setText(m_metadata["Author"].toString());
    ui->label_time->setText(m_metadata["Duration"].toString());
}
