#ifndef DIALOGTESTLOG_H
#define DIALOGTESTLOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class DialogTestLog;
}

class DialogTestLog : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTestLog(QWidget *parent = nullptr);
    ~DialogTestLog();

private:
    Ui::DialogTestLog *ui;

    QStandardItemModel *m_pModel1 = nullptr ;
    QStandardItemModel *m_pModel2 = nullptr ;
    int m_total = 0 ;
    int m_good = 0 ;
    QString m_strLogFile ;
    void addItem(QString strLine) ;
};

#endif // DIALOGTESTLOG_H
