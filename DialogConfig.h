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

    void setWitch(quint8 addr,bool on=true) ;
    void SendTextCmd(const QString&text) ;
    void getMeter(quint8 id) ;
    int  getChanCount() ;

    Qt::CheckState  getChanState(int chanId) ;
    static DialogConfig *instance() ;

signals:
    void onChanState(int chanId, Qt::CheckState state) ;

private:
    Ui::DialogConfig *ui;
    QStandardItemModel *m_pModelTGroup = nullptr ;
    QStandardItemModel *m_pModelCOM = nullptr ;
    QStandardItemModel *m_pModelChan = nullptr ;

    QStandardItemModel *m_pModelDelay = nullptr ;
    QStandardItemModel *m_pModelGSwitch = nullptr ;

    GenComport *m_pCOM1 = nullptr;
    GenComport *m_pCOM2 = nullptr;

    QByteArray m_buf1 ;
    QByteArray m_buf2 ;

    void updateCOMStatus();
};


#endif // DIALOGCONFIG_H
