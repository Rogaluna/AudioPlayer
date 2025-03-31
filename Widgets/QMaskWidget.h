#ifndef QMASKWIDGET_H
#define QMASKWIDGET_H

#include <QWidget>

class QMaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMaskWidget(QWidget *parent = nullptr);

    void setMaskColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    // 添加鼠标事件处理
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_maskColor = QColor(0, 0, 0, 120);

signals:
    void clicked();
};

#endif // QMASKWIDGET_H
