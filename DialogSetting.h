#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

private:
    Ui::DialogSetting *ui;

    QStandardItemModel *m_pModel1 = nullptr ;
    QStandardItemModel *m_pModel2 = nullptr ;
    QStandardItemModel *m_pModel3 = nullptr ;
    QStandardItemModel *m_pModel4 = nullptr ;
};

#endif // DIALOGSETTING_H
