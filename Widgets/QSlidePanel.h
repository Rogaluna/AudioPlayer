#ifndef QSLIDEPANELH_H
#define QSLIDEPANELH_H

#include <QPropertyAnimation>
#include <QWidget>

class QMaskWidget;
class QSlidePanel : public QWidget
{
    Q_OBJECT
public:
    enum SlideDirection { Left, Right };

    explicit QSlidePanel(QWidget *parent = nullptr);

    // 配置接口
    void setSlideDirection(SlideDirection dir);
    void setPanelSize(int width, int height = -1);
    void setTopOffset(int offset);

    // 内容管理
    void setContentWidget(QWidget *widget);
    void showPanel();
    void hidePanel();

    QMaskWidget* maskWidget() const { return m_mask; }

protected:
    void updatePanelGeometry();
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QMaskWidget *m_mask;
    QWidget *m_content;
    SlideDirection m_direction = Right;
    int m_panelWidth = 300;
    int m_panelHeight = -1; // -1表示自动高度
    int m_topOffset = 0;
};

#endif // QSLIDEPANELH_H
