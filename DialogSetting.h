#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QStandardItemModel>

#include "DialogConfig.h"
namespace Ui {
class DialogSetting;
}

#include <QTimer>
#include <QSettings>

#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionHeader>
#include <QStyle>

class ZeroBasedHeader : public QHeaderView {
    Q_OBJECT
public:
    explicit ZeroBasedHeader(QWidget *parent = nullptr)
        : QHeaderView(Qt::Vertical, parent) {}

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override {
        painter->save();
        painter->fillRect(rect,painter->background());
        painter->setPen(Qt::gray) ;
        painter->drawLine(rect.bottomLeft(),rect.bottomRight()) ;
        painter->drawLine(rect.topRight(),rect.bottomRight()) ;
        painter->setPen(Qt::black) ;
        painter->setFont(parentWidget()->font()) ;
        painter->drawText(rect.adjusted(0,0,-5,0),QString::number(logicalIndex),QTextOption(Qt::AlignVCenter|Qt::AlignRight));

        //qDebug() << painter->background() ;
        //QHeaderView::paintSection(painter, rect, logicalIndex);

        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.text = QString::number(logicalIndex);
        opt.rect = rect;

        //style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);
        painter->restore();
    }
};



class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

    void startTest();
    void toTheEnd() ;

signals:
    void onTestOption(int item,bool enable);
    void onTestToEnd();

private:
    Ui::DialogSetting *ui;
    QSettings *m_pSet = nullptr;

    QStandardItemModel *m_pModel1 = nullptr ;

    DialogConfig *m_cfg ;

    void addTestItem(const QString&text,bool option=false) ;

    int m_nTestIndex = -1 ;
    QTimer m_TMTest ;
};

#endif // DIALOGSETTING_H
