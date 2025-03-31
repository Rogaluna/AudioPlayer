#ifndef ALBUMLOADDIALOG_H
#define ALBUMLOADDIALOG_H

#include <QWidget>

namespace Ui {
class AlbumLoadDialog;
}

class AlbumLoadDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumLoadDialog(QWidget *parent = nullptr);
    ~AlbumLoadDialog();

private:
    Ui::AlbumLoadDialog *ui;
};

#endif // ALBUMLOADDIALOG_H
