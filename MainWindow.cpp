#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTableView>
#include <QHeaderView>
#include <QWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->checkBoxOntop,&QCheckBox::clicked,this,[=](bool checked){
        QWindow *pWin = windowHandle() ;
        if(checked)
            pWin->setFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            pWin->setFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    });

    m_pDlgSet = new DialogSetting(this);
    m_pDlgCfg = new DialogConfig(this) ;

    connect(ui->pushButtonSetting,&QPushButton::clicked,this,[=]{
        m_pDlgSet->show();
    });

    connect(ui->pushButtonConfig,&QPushButton::clicked,this,[=]{
        m_pDlgCfg->show();
    });

    m_pModel1 = new QStandardItemModel(ui->tableView1);
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    m_pModel1->setHorizontalHeaderLabels(QString("通道号,静态电流(mA),吸烟电流(mA),充电电流(mA),充满电压(V),输出电压(V),启动电压(V),测试结果").split(','));
    for(int i=0; i<12; i++)
    {
        QStandardItem *item0 = new QStandardItem(QString::number(i+1)) ;
        QStandardItem *item1 = new QStandardItem("0") ;
        QStandardItem *item2 = new QStandardItem("0") ;
        QStandardItem *item3 = new QStandardItem("0") ;
        QStandardItem *item4 = new QStandardItem("--") ;
        QStandardItem *item5 = new QStandardItem("--") ;
        QStandardItem *item6 = new QStandardItem("--") ;
        QStandardItem *item7 = new QStandardItem("--") ;
        QList<QStandardItem *>items{item0,item1,item2,item3,item4,item5,item6,item7} ;
        for(QStandardItem *item:items)
            item->setTextAlignment(Qt::AlignCenter) ;
        item0->setBackground(QBrush(Qt::lightGray)) ;
        m_pModel1->appendRow(items);
    }
    pHeader->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed);
    pHeader->resizeSection(0,50) ;

    for(int i=0; i<pHeader->count(); i++)
    {
        ColorDelegate *test = new ColorDelegate(ui->tableView1);
        test->setTableView(ui->tableView1) ;
        ui->tableView1->setItemDelegateForColumn(i,test) ;
    }
    m_pModel1->item(1,1)->setBackground(QBrush(Qt::green)) ;
    m_pModel1->item(1,7)->setBackground(QBrush(Qt::green)) ;
    m_pModel1->item(4,1)->setBackground(QBrush(Qt::red)) ;
    m_pModel1->item(4,7)->setBackground(QBrush(Qt::red)) ;
    m_pModel1->item(4,7)->setBackground(QBrush(Qt::transparent)) ;
    ui->tableView1->update();

    m_pModel2 = new QStandardItemModel(ui->tableView2);
    ui->tableView2->setModel(m_pModel2);
    m_pModel2->setHorizontalHeaderLabels({"",""});
    m_pModel2->appendRow({new QStandardItem("测试总数"),new QStandardItem("0")});
    m_pModel2->appendRow({new QStandardItem("通过总数"),new QStandardItem("0")});
    m_pModel2->appendRow({new QStandardItem("通过比例"),new QStandardItem("0")});
    ui->tableView2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView2->horizontalHeader()->setVisible(false) ;
    {
        ColorDelegate *test = new ColorDelegate(ui->tableView2);
        ui->tableView2->setItemDelegateForColumn(0,test) ;
    }
    m_pModel2->item(0,0)->setBackground(QBrush(Qt::lightGray)) ;
    m_pModel2->item(1,0)->setBackground(QBrush(Qt::lightGray)) ;
    m_pModel2->item(2,0)->setBackground(QBrush(Qt::lightGray)) ;

    ui->tableView1->setStyleSheet("font: bold 12px 微软雅黑;") ;
    ui->tableView2->setStyleSheet("font: bold 12px 微软雅黑;") ;
    ui->tableView2->setStyleSheet("QTableView { border-right: none; border-bottom: none;}") ;
    ui->labelStatus->setStyleSheet("QLabel{border:2px solid gray;}") ;
}

MainWindow::~MainWindow()
{
    delete ui;
}
