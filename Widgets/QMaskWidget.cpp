#include "QMaskWidget.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

QMaskWidget::QMaskWidget(QWidget *parent)
    : QWidget{parent}
{
    if(parent) {
        parent->installEventFilter(this);
        setGeometry(parent->rect());
    }

    hide();
}

void QMaskWidget::setMaskColor(const QColor &color)
{
    m_maskColor = color;
    update();
}

void QMaskWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_maskColor);

    QWidget::paintEvent(event);
}

void QMaskWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if(parentWidget()) {
        setGeometry(parentWidget()->rect());
    }
}

bool QMaskWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize) {
        if(QWidget *parent = qobject_cast<QWidget*>(watched)) {
            setGeometry(parent->rect());
        }
    }
    return QWidget::eventFilter(watched, event);
}

void QMaskWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        emit clicked();
        event->accept(); // 接受事件
    } else {
        QWidget::mousePressEvent(event);
    }
}

