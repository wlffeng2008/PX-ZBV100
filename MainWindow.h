#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DialogSetting.h"
#include "DialogConfig.h"

#include <QTableView>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

static int g_clicked = -1 ;
class ColorDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ColorDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
    }
    void setImage(const QString&text){ m_strImage = text ; } ;
    void setTableView(QTableView *pView){ tableView = pView ;} ;

private:
    QString m_strImage ;
    QTableView *tableView = nullptr ;
    int m_nSelect = -1 ;

signals:
    void onClicked(int col);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        if(g_clicked == index.row())
        {
            QStyledItemDelegate::paint(painter,option,index) ;
            return ;
        }

        painter->save() ;

        QStandardItemModel *pModel = (QStandardItemModel *)index.model() ;
        QStandardItem *item = pModel->item(index.row(),index.column()) ;
        QString strText = item->text() ;

        QRect rect = option.rect ;
        QFont font = item->data(Qt::FontRole).value<QFont>() ;
        if(m_nSelect == index.row())
        {
            static QImage img(m_strImage) ;
            painter->drawImage(rect,img);
            //font.setPointSize(font.pointSize()*1.3) ;
        }
        else
        {
            QBrush br0 = item->data(Qt::BackgroundRole).value<QBrush>();
            painter->fillRect(rect,br0);
        }

        painter->setFont(font) ;
        QBrush br1 = item->data(Qt::ForegroundRole).value<QBrush>();
        QPen pen(br1.color()) ;
        painter->setPen(pen) ;
        painter->drawText(rect,Qt::AlignCenter,strText);

        painter->restore() ;
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override
    {
        if(event->type() == QEvent::MouseButtonRelease && tableView)
        {
            m_nSelect = index.row() ;
            g_clicked = index.row() ;
            tableView->update() ;
            emit onClicked(index.row()) ;
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *m_pModel1 = nullptr ;
    QStandardItemModel *m_pModel2 = nullptr ;

    DialogSetting *m_pDlgSet = nullptr ;
    DialogConfig  *m_pDlgCfg = nullptr ;
};
#endif // MAINWINDOW_H
