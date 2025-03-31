#include "AlbumLoadDialog.h"
#include "ui_AlbumLoadDialog.h"

AlbumLoadDialog::AlbumLoadDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlbumLoadDialog)
{
    ui->setupUi(this);
}

AlbumLoadDialog::~AlbumLoadDialog()
{
    delete ui;
}
