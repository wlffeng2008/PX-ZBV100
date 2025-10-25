#include "DialogConfig.h"
#include "ui_DialogConfig.h"

#include <QTimer>

DialogConfig::DialogConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    m_pModel1 = new QStandardItemModel(this);
    m_pModel1->setHorizontalHeaderLabels(QString("测试项目,下限,上限,单位").split(','));
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,70) ;
    ui->tableView1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModel1->appendRow({new QStandardItem("静态电流"),new QStandardItem("5.00"),new QStandardItem("7.00"),new QStandardItem("uA")}) ;
    m_pModel1->appendRow({new QStandardItem("吸烟电流"),new QStandardItem("1000"),new QStandardItem("3000"),new QStandardItem("mA")}) ;
    m_pModel1->appendRow({new QStandardItem("充电电流"),new QStandardItem("100" ),new QStandardItem("600" ),new QStandardItem("mA")}) ;
    m_pModel1->appendRow({new QStandardItem("充满电压"),new QStandardItem("4150"),new QStandardItem("4250"),new QStandardItem("mV")}) ;
    m_pModel1->appendRow({new QStandardItem("输出电压"),new QStandardItem("3500"),new QStandardItem("3600"),new QStandardItem("mV")}) ;
    m_pModel1->appendRow({new QStandardItem("启动气压"),new QStandardItem("-200"),new QStandardItem("-100"),new QStandardItem("Pa")}) ;
    for(int i=0; i<6; i++)
    {
        m_pModel1->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter) ;
        for(int j=0; j<4; j++)
        {
            QStandardItem *item = m_pModel1->item(i,j) ;
            if(j == 0 || j == 3) item->setEditable(false);
            if(j > 0) item->setTextAlignment(Qt::AlignCenter) ;
        }
    }

    m_pModel2 = new QStandardItemModel(this);
    m_pModel2->setHorizontalHeaderLabels(QString("参数,单板串口,仪表485串口").split(','));
    ui->tableView2->setModel(m_pModel2);
    pHeader = ui->tableView2->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableView2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,70) ;

    m_pModel2->appendRow({new QStandardItem("串口"),new QStandardItem("COM1"),new QStandardItem("COM2")}) ;
    m_pModel2->appendRow({new QStandardItem("波特率"),new QStandardItem("115200"),new QStandardItem("115200")}) ;
    m_pModel2->appendRow({new QStandardItem("数据位"),new QStandardItem("8位"),new QStandardItem("8位")}) ;
    m_pModel2->appendRow({new QStandardItem("校验位"),new QStandardItem("无校验"),new QStandardItem("无校验")}) ;
    m_pModel2->appendRow({new QStandardItem("停止位"),new QStandardItem("1位"),new QStandardItem("1位")}) ;
    m_pModel2->appendRow({new QStandardItem("连接状态"),new QStandardItem("串口未打开"),new QStandardItem("串口未打开")}) ;
    m_pModel2->appendRow({new QStandardItem("控制"),new QStandardItem("打开串口"),new QStandardItem("打开串口")}) ;
    for(int i=0; i<7; i++)
    {
        m_pModel2->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter) ;
        m_pModel2->item(i,0)->setSelectable(false) ;
        for(int j=0; j<3; j++)
        {
            QStandardItem *item = m_pModel2->item(i,j) ;
            item->setEditable(false);
        }
    }

    m_pModel2->item(0,1)->setEditable(true);
    m_pModel2->item(0,2)->setEditable(true);
    m_pModel2->item(1,1)->setEditable(true);
    m_pModel2->item(1,2)->setEditable(true);

    m_pModel2->item(5,1)->setCheckable(true);
    m_pModel2->item(5,2)->setCheckable(true);
    m_pModel2->item(5,1)->setEnabled(false);
    m_pModel2->item(5,2)->setEnabled(false);

    m_pModel2->item(6,1)->setCheckable(true);
    m_pModel2->item(6,2)->setCheckable(true);

    m_pModel3 = new QStandardItemModel(this);
    m_pModel3->setHorizontalHeaderLabels(QString("通道号,开启检查").split(','));
    ui->tableView3->setModel(m_pModel3);
    pHeader = ui->tableView3->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableView3->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,50) ;

    connect(ui->spinBox,&QSpinBox::valueChanged,this,[=](int value){
        m_pModel3->setRowCount(0) ;
        for(int i=0; i<value; i++)
        {
            QStandardItem *item1 = new QStandardItem(QString("%1").arg(i+1));
            QStandardItem *item2 = new QStandardItem("");
            item1->setEditable(false);
            item1->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight) ;
            item2->setEditable(false);
            item2->setCheckable(true);
            item2->setCheckState(Qt::Checked);
            m_pModel3->appendRow({item1,item2});
        }
    }) ;

    ui->spinBox->setValue(1) ;
    QTimer::singleShot(200,[=]{ ui->spinBox->setValue(12); });

    connect(ui->tableView2,&QTableView::clicked,this,[=](const QModelIndex &index){

    });

    connect(m_pModel2,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        int row = item->row();
        int col = item->column();

        if(row == 6)
        {
            bool checked = (item->checkState() == Qt::Checked) ;
            if(col == 1)
            {
                if(m_pCOM1)
                    m_pCOM1->closePort() ;

                if( checked )
                {
                    m_pCOM1 = new GenComport(this) ;
                    m_pCOM1->setPortName(m_pModel2->item(0,1)->text().trimmed()) ;
                    m_pCOM1->setPortParam(m_pModel2->item(1,1)->text().toInt()) ;
                    m_pCOM1->openPort() ;
                }
            }

            if(col == 2)
            {
                if(m_pCOM2)
                    m_pCOM2->closePort() ;

                if( checked )
                {
                    m_pCOM2 = new GenComport(this) ;
                    m_pCOM2->setPortName(m_pModel2->item(0,2)->text().trimmed()) ;
                    m_pCOM2->setPortParam(m_pModel2->item(1,2)->text().toInt()) ;
                    m_pCOM2->openPort() ;
                }
            }

            updateCOMStatus() ;
        }
    });

    m_pCOM1 = new GenComport(this);
    m_pCOM2 = new GenComport(this);

    connect(m_pCOM1,&GenComport::onReceive,this,[=](const QByteArray &data){});
    connect(m_pCOM2,&GenComport::onReceive,this,[=](const QByteArray &data){});
}

DialogConfig::~DialogConfig()
{
    delete ui;
}

void DialogConfig::updateCOMStatus()
{
    m_pModel2->item(5,1)->setCheckState(m_pCOM1->isOpen() ? Qt::Checked : Qt::Unchecked) ;
    m_pModel2->item(5,2)->setCheckState(m_pCOM2->isOpen() ? Qt::Checked : Qt::Unchecked) ;

    m_pModel2->item(5,1)->setText(m_pCOM1->isOpen() ? "串口已打开" : "串口未打开") ;
    m_pModel2->item(5,2)->setText(m_pCOM2->isOpen() ? "串口已打开" : "串口未打开") ;
}
