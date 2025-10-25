#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>
#include <QStandardItemModel>

#include "gencomport.h"

namespace Ui {
class DialogConfig;
}

class DialogConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfig(QWidget *parent = nullptr);
    ~DialogConfig();

private:
    Ui::DialogConfig *ui;
    QStandardItemModel *m_pModel1 = nullptr ;
    QStandardItemModel *m_pModel2 = nullptr ;
    QStandardItemModel *m_pModel3 = nullptr ;

    GenComport *m_pCOM1 = nullptr;
    GenComport *m_pCOM2 = nullptr;

    void updateCOMStatus();
};

#endif // DIALOGCONFIG_H
