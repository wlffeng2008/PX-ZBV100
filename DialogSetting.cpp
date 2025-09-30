#include "DialogSetting.h"
#include "ui_DialogSetting.h"

DialogSetting::DialogSetting(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

}

DialogSetting::~DialogSetting()
{
    delete ui;
}
