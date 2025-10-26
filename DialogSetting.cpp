#include "DialogSetting.h"
#include "ui_DialogSetting.h"

#include <QHeaderView>

DialogSetting::DialogSetting(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    m_cfg = DialogConfig::instance() ;

    m_pModel1 = new QStandardItemModel(this);
    m_pModel1->setHorizontalHeaderLabels(QString("读取项,485端口地址,数据类型,单位").split(','));
    ui->tableView1->setModel(m_pModel1);
    QHeaderView *pHeader = ui->tableView1->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableView1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModel1->appendRow({new QStandardItem("静态电流"),new QStandardItem("1"), new QStandardItem("HEX浮点数"),new QStandardItem("uA")}) ;
    m_pModel1->appendRow({new QStandardItem("吸烟电流"),new QStandardItem("7"), new QStandardItem("HEX浮点数"),new QStandardItem("mA")}) ;
    m_pModel1->appendRow({new QStandardItem("充电电流"),new QStandardItem("7"), new QStandardItem("HEX浮点数"),new QStandardItem("mA")}) ;
    m_pModel1->appendRow({new QStandardItem("充满电压"),new QStandardItem("20"),new QStandardItem("HEX整型数"),new QStandardItem("mV")}) ;
    m_pModel1->appendRow({new QStandardItem("输出电压"),new QStandardItem("30"),new QStandardItem("HEX整型数"),new QStandardItem("mV")}) ;
    m_pModel1->appendRow({new QStandardItem("启动气压"),new QStandardItem("40"),new QStandardItem("HEX浮点数"),new QStandardItem("Pa")}) ;
    for(int i=0; i<6; i++)
    {
        m_pModel1->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter) ;
        for(int j=0; j<4; j++)
        {
            QStandardItem *item = m_pModel1->item(i,j) ;
            if(j != 1) item->setEditable(false);
        }
    }

    m_pModelDelay = new QStandardItemModel(this);
    m_pModelDelay->setHorizontalHeaderLabels(QString("控制项,时间(ms)").split(','));
    ui->tableViewDelay->setModel(m_pModelDelay);
    pHeader = ui->tableViewDelay->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,60) ;
    ui->tableViewDelay->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModelDelay->appendRow({new QStandardItem("上电后等待时间"),new QStandardItem("6000")});
    m_pModelDelay->appendRow({new QStandardItem("开气泵后等待时间"),new QStandardItem("500")});
    m_pModelDelay->appendRow({new QStandardItem("吸烟时间"),new QStandardItem("500")});
    m_pModelDelay->appendRow({new QStandardItem("吸烟结束等待时间"),new QStandardItem("200")});
    m_pModelDelay->appendRow({new QStandardItem("开始充电等待时间"),new QStandardItem("1000")});
    m_pModelDelay->appendRow({new QStandardItem("充电时间"),new QStandardItem("2000")});
    for(int i=0; i<6; i++)
    {
        m_pModelDelay->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelDelay->item(i,0)->setEditable(false);
    }

    m_pModelGSwitch = new QStandardItemModel(this);
    m_pModelGSwitch->setHorizontalHeaderLabels(QString("名称,地址").split(','));
    ui->tableViewGSwitch->setModel(m_pModelGSwitch);
    pHeader = ui->tableViewGSwitch->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,60) ;
    ui->tableViewGSwitch->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控1"),new QStandardItem("20")});
    m_pModelGSwitch->appendRow({new QStandardItem("静态电流总控2"),new QStandardItem("22")});
    m_pModelGSwitch->appendRow({new QStandardItem("吸烟气泵开关"),new QStandardItem("10")});
    for(int i=0; i<3; i++)
    {
        m_pModelGSwitch->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModelGSwitch->item(i,0)->setEditable(false);
        m_pModelGSwitch->item(i,1)->setCheckable(true);
    }

    connect(m_pModelGSwitch,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        if(item->column() != 0)
        {
            m_cfg->setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        }
    });

    m_pModel4 = new QStandardItemModel(this);
    m_pModel4->setHorizontalHeaderLabels(QString("通道号,静态电流开关,切换电流表,充电开关,LED红灯开关,LED绿灯开关").split(','));
    ui->tableView4->setModel(m_pModel4);
    pHeader = ui->tableView4->horizontalHeader() ;
    pHeader->setSectionResizeMode(QHeaderView::Stretch) ;
    ui->tableView4->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_pModel4->appendRow({new QStandardItem("1"),new QStandardItem("30"),new QStandardItem("5C"),new QStandardItem("41"),new QStandardItem("51"),new QStandardItem("50")});
    m_pModel4->appendRow({new QStandardItem("2"),new QStandardItem("31"),new QStandardItem("5D"),new QStandardItem("42"),new QStandardItem("53"),new QStandardItem("52")});
    m_pModel4->appendRow({new QStandardItem("3"),new QStandardItem("32"),new QStandardItem("5E"),new QStandardItem("43"),new QStandardItem("55"),new QStandardItem("54")});
    m_pModel4->appendRow({new QStandardItem("4"),new QStandardItem("33"),new QStandardItem("5F"),new QStandardItem("44"),new QStandardItem("57"),new QStandardItem("56")});
    m_pModel4->appendRow({new QStandardItem("5"),new QStandardItem("34"),new QStandardItem("11"),new QStandardItem("45"),new QStandardItem("59"),new QStandardItem("58")});
    m_pModel4->appendRow({new QStandardItem("6"),new QStandardItem("35"),new QStandardItem("12"),new QStandardItem("46"),new QStandardItem("5B"),new QStandardItem("5A")});
    for(int i=0; i<6; i++)
    {
        m_pModel4->item(i,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        m_pModel4->item(i,0)->setEditable(false);
        for(int j=1;j<=5; j++)
            m_pModel4->item(i,j)->setCheckable(true);
    }

    pHeader->setSectionResizeMode(0,QHeaderView::Fixed) ;
    pHeader->resizeSection(0,45) ;
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed) ;
    pHeader->resizeSection(1,90) ;

    connect(m_pModel4,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        if(item->column() != 0)
        {
            m_cfg->setWitch(item->text().trimmed().toUInt(nullptr,16),item->checkState() == Qt::Checked) ;
        }
    });
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

void DialogSetting::setLED_R(int chanId,bool on)
{
    QStandardItem *item=m_pModel4->item(chanId,4) ;
    item->setCheckState(on ?Qt::Checked:Qt::Unchecked) ;
}

void DialogSetting::setLED_G(int chanId,bool on)
{
    QStandardItem *item=m_pModel4->item(chanId,5) ;
    item->setCheckState(on ?Qt::Checked:Qt::Unchecked) ;
}
