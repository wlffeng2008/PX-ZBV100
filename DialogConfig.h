#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>
#include <QStandardItemModel>

#include "gencomport.h"

namespace Ui {
class DialogConfig;
}

#include <QSettings>

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

    void setLED_R(int chanId,bool on=true);
    void setLED_G(int chanId,bool on=true);

    void setGSwitch(int type,bool on=true);

    void readGroup(int type);

    void setGroupSwitch(int type,bool on=true) ;

    void doTesting(int index);

    Qt::CheckState  getChanState(int chanId) ;
    static DialogConfig *instance() ;

signals:
    void onChanState(int chanId, Qt::CheckState state) ;
    void onAutoTestAcq() ;
    void onReadBack(int chanId,int col, float value,bool ok=true) ;

private:
    Ui::DialogConfig *ui;

    QSettings *m_pSet = nullptr ;
    QStandardItemModel *m_pModelTGroup = nullptr ;
    QStandardItemModel *m_pModelCOM = nullptr ;
    QStandardItemModel *m_pModelChan = nullptr ;
    QStandardItemModel *m_pModelGSwitch = nullptr ;

    GenComport *m_pCOM1 = nullptr;
    GenComport *m_pCOM2 = nullptr;

    QByteArray m_buf1 ;
    QByteArray m_buf2 ;

    int m_readType = 0;
    int m_readPort = 0;
    float m_max = 10000 ;
    float m_min = -10000 ;

    void updateCOMStatus();
};


#endif // DIALOGCONFIG_H
