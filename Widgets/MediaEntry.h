#ifndef MEDIAENTRY_H
#define MEDIAENTRY_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MediaEntry;
}
QT_END_NAMESPACE

class MediaEntry : public QWidget
{
    Q_OBJECT

public:
    explicit MediaEntry(QWidget *parent = nullptr);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

public:
    void setEntry(const QVariantMap& metadata);

    void update();

private:
    Ui::MediaEntry *ui;

    QVariantMap m_metadata;

signals:
    void clicked(const QVariantMap&);
};

#endif // MEDIAENTRY_H
