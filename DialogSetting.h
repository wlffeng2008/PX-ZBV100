#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QStandardItemModel>

#include "DialogConfig.h"
namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

    void setLED_R(int chanId,bool on=true);
    void setLED_G(int chanId,bool on=true);

private:
    Ui::DialogSetting *ui;

    QStandardItemModel *m_pModel1 = nullptr ;
    QStandardItemModel *m_pModelDelay = nullptr ;
    QStandardItemModel *m_pModelGSwitch = nullptr ;
    QStandardItemModel *m_pModel4 = nullptr ;

    DialogConfig *m_cfg ;
};

#endif // DIALOGSETTING_H
