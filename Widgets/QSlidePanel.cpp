#include "QSlidePanel.h"

#include "QMaskWidget.h"

#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

QSlidePanel::QSlidePanel(QWidget *parent)
    : QWidget{parent}
    , m_mask(new QMaskWidget(parent))
    , m_content(new QWidget(parent))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    // 初始化层级关系
    parent->installEventFilter(this);

    // 遮罩层配置
    m_mask->hide();
    connect(m_mask, &QMaskWidget::clicked, this, &QSlidePanel::hidePanel);

    // 内容面板样式
    m_content->setObjectName("slideContent");
    m_content->setStyleSheet("#slideContent { background: white; border: 1px solid #CCC; }");
    m_content->hide();

    // 初始布局更新
    updatePanelGeometry();
}

void QSlidePanel::setSlideDirection(SlideDirection dir)
{
    m_direction = dir;
    updatePanelGeometry();
}

void QSlidePanel::setPanelSize(int width, int height)
{
    m_panelWidth = width;
    m_panelHeight = height;
    updatePanelGeometry();
}

void QSlidePanel::setTopOffset(int offset)
{
    m_topOffset = offset;
    updatePanelGeometry();
}

void QSlidePanel::setContentWidget(QWidget *widget)
{
    QLayout *layout = m_content->layout();
    if(layout) delete layout;

    QVBoxLayout *newLayout = new QVBoxLayout(m_content);
    newLayout->setContentsMargins(0, 0, 0, 0);
    newLayout->addWidget(widget);
}

void QSlidePanel::showPanel()
{
    raise();
    m_mask->show();
    m_content->show();
}

void QSlidePanel::hidePanel()
{
    m_mask->hide();
    m_content->hide();
}

void QSlidePanel::updatePanelGeometry()
{
    if(!parentWidget()) return;

    // 计算内容面板位置
    const int pw = parentWidget()->width();
    const int ph = parentWidget()->height();

    // 设置内容尺寸
    m_content->setFixedWidth(m_panelWidth);
    m_content->setFixedHeight(m_panelHeight > 0 ? m_panelHeight : ph - m_topOffset);

    // 设置位置
    if(m_direction == Right) {
        m_content->move(pw - m_panelWidth, m_topOffset);
    } else {
        m_content->move(0, m_topOffset);
    }

    // 更新遮罩层
    m_mask->setGeometry(parentWidget()->rect());
}

bool QSlidePanel::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize) {
        updatePanelGeometry();
    }
    return QWidget::eventFilter(watched, event);
}

