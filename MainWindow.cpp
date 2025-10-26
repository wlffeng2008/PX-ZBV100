#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTableView>
#include <QHeaderView>
#include <QTimer>
#include <QWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    qApp->setStyle(new CustomStyle);

    connect(ui->checkBoxOntop,&QCheckBox::clicked,this,[=](bool checked){
        QWindow *pWin = windowHandle() ;
        if(checked)
            pWin->setFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            pWin->setFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    });

    m_pDlgCfg = new DialogConfig(this) ;
    m_pDlgSet = new DialogSetting(this);

    connect(ui->pushButtonSetting,&QPushButton::clicked,this,[=]{
        m_pDlgSet->show();
    });

    connect(ui->pushButtonConfig,&QPushButton::clicked,this,[=]{
        m_pDlgCfg->show();
    });

    m_pModel1 = new QStandardItemModel(ui->tableView1);
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    m_pModel1->setHorizontalHeaderLabels(QString("通道号(启用),静态电流(uA),吸烟电流(mA),充电电流(mA),充满电压(mV),输出电压(mV),启动电压(mV),测试结果").split(','));
    for(int i=0; i<m_pDlgCfg->getChanCount(); i++)
    {
        QStandardItem *item0 = new QStandardItem(QString::number(i+1)) ;
        QStandardItem *item1 = new QStandardItem("0") ;
        QStandardItem *item2 = new QStandardItem("0") ;
        QStandardItem *item3 = new QStandardItem("0") ;
        QStandardItem *item4 = new QStandardItem("0") ;
        QStandardItem *item5 = new QStandardItem("0") ;
        QStandardItem *item6 = new QStandardItem("0") ;
        QStandardItem *item7 = new QStandardItem("待测试") ;
        QList<QStandardItem *>items{item0,item1,item2,item3,item4,item5,item6,item7} ;
        for(QStandardItem *item:items)
            item->setTextAlignment(Qt::AlignCenter) ;
        item0->setBackground(QBrush(QColorConstants::Svg::skyblue)) ;
        item0->setCheckable(true) ;
        item0->setCheckState(Qt::Checked) ;
        item0->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft) ;
        m_pModel1->appendRow(items);
    }
    pHeader->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed);
    pHeader->resizeSection(0,100) ;
    //ui->tableView1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch) ;

    for(int i=0; i<pHeader->count(); i++)
    {
        ColorDelegate *test = new ColorDelegate(ui->tableView1);
        test->setTableView(ui->tableView1) ;
        ui->tableView1->setItemDelegateForColumn(i,test) ;
    }
    // m_pModel1->item(1,1)->setBackground(QBrush(Qt::green)) ;
    // m_pModel1->item(1,7)->setBackground(QBrush(Qt::green)) ;
    // m_pModel1->item(4,2)->setBackground(QBrush(Qt::red)) ;
    // m_pModel1->item(4,6)->setBackground(QBrush(Qt::red)) ;

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
    m_pModel2->item(0,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
    m_pModel2->item(1,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
    m_pModel2->item(2,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;

    ui->tableView2->setStyleSheet("QTableView { border-right: none; border-bottom: none; font: bold 12px 微软雅黑;}") ;
    ui->labelStatus->setStyleSheet("QLabel{border:2px solid gray;}") ;

    connect(ui->pushButtonRun,&QPushButton::clicked,this,[=]{
        setRowResult(1,1) ;
        setRowResult(3,1) ;
        setItemResult(2,1,1);
        setItemResult(2,2,1);
        setItemResult(2,4,2);

        setItemResult(0,1,1);
        setItemResult(0,3,2);
        QTimer::singleShot(5000,this,[=]{setResult(0);});
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isRowPassed(int row)
{
    for(int col=1; col<7; col++)
    {
        int result = m_pModel1->item(row,col)->data(Qt::UserRole+1).value<int>() ;
        if(result != 1) return false ;
    }
    return true;
}

void MainWindow::setItemResult(int row, int col, int result)
{
    m_pModel1->item(row,col)->setData(result,Qt::UserRole+1) ;
    if(result != 0)
        m_pModel1->item(row,7)->setData(isRowPassed(row)?1:2,Qt::UserRole+1) ;
}

void MainWindow::setRowResult(int row, int result)
{
    for(int col=1; col<7; col++)
    {
        setItemResult(row,col,result) ;
    }
}

void MainWindow::setResult(int result)
{
    for(int row=0; row<m_pModel1->rowCount(); row++)
    {
        for(int col=1; col<8; col++)
        {
            setItemResult(row,col,result) ;
        }
    }
}
