#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DialogSetting.h"
#include "DialogConfig.h"

#include <QApplication>
#include <QProxyStyle>
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

class CustomStyle : public QProxyStyle {
public:
    void drawControl(ControlElement element, const QStyleOption* option,
                     QPainter* painter, const QWidget* widget) const override {
        if (element == CE_CheckBox)
        {
            painter->save();
            painter->setRenderHint(QPainter::SmoothPixmapTransform) ;
            QRect rectBox = option->rect ;
            rectBox.adjust(4,4,-4,-4) ;
            rectBox.setWidth(rectBox.height()) ;
            if (option->state & State_On)
                painter->drawImage(rectBox,QImage(":/images/BoxChecked.png"));
            else
                painter->drawImage(rectBox,QImage(":/images/BoxUncheck.png"));

            painter->restore();
        } else {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};



class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ColorDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) { }
    void setTableView(QTableView *pView){ tableView = pView; } ;

private:
    QTableView *tableView = nullptr ;

signals:
    void onClicked(int row, int col);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStandardItemModel *pModel = (QStandardItemModel *)index.model() ;
        QStandardItem *item = pModel->itemFromIndex(index) ;
        if(!item->isEnabled())
        {
            QStyledItemDelegate::paint(painter, option, index) ;
            return ;
        }

        painter->save() ;

        QRect rect = option.rect ;
        int col = index.column() ;
        int result = item->data(Qt::UserRole+1).value<int>() ;
        QString text = item->text() ;
        QBrush bkBrush = item->data(Qt::BackgroundRole).value<QBrush>() ;
        QColor frColor = item->data(Qt::ForegroundRole).value<QBrush>().color() ;

        if(col == 7) text = "待测试" ;
        if(result == 1){bkBrush = QBrush(0x00A000); frColor = Qt::white; if(col == 7) text = "PASS";}
        if(result == 2){bkBrush = QBrush(Qt::red);  frColor = Qt::white; if(col == 7) text = "NG";  }

        painter->setFont(item->data(Qt::FontRole).value<QFont>()) ;
        painter->setPen(frColor) ;
        painter->fillRect(rect,bkBrush);
        if(item->isCheckable())
        {
            painter->setRenderHint(QPainter::SmoothPixmapTransform) ;
            QRect rectBox = rect.adjusted(option.rect.right() - option.rect.height(),0,0,0);
            rectBox.setWidth(rectBox.height()) ;
            while(rectBox.height()>18) rectBox.adjust(1,1,-1,-1) ;
            painter->drawImage(rectBox,QImage(item->checkState() == Qt::Checked ? ":/images/BoxChecked.png":":/images/BoxUncheck.png"));
            rect.adjust(50,0,0,0) ;
        }
        rect.adjust(5,0,-5,0) ;
        painter->drawText(rect,item->textAlignment()|Qt::AlignVCenter,text);

        painter->restore() ;
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override
    {
        if(event->type() == QEvent::MouseButtonRelease && tableView)
        {
            QStandardItemModel *pModel = (QStandardItemModel *)index.model() ;
            QStandardItem *item = pModel->itemFromIndex(index) ;
            tableView->update() ;
            emit onClicked(index.row(),index.column()) ;
            QPoint clp = tableView->viewport()->mapFromGlobal(QCursor::pos())  ;

            // qDebug() << option.rect << clp;
            if(item->isCheckable())
            {
                if(index.column()==0)
                    return true ;
                if(clp.x() > 70 && clp.x() <100)
                item->setCheckState(item->checkState()== Qt::Unchecked ? Qt::Checked:Qt::Unchecked) ;
                return true;
            }
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

    bool m_colEnable[10]={true} ;

    QTimer m_TMCheck ;

    void setItemResult(int row,int col,int result=0) ;
    void setRowResult(int row,int result=0) ;
    bool isRowPassed(int row) ;
    void setResult(int result=0) ;

    void setItemColor(QStandardItem *item,int result=0) ;
};
#endif // MAINWINDOW_H
