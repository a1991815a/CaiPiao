#include "formulaui.h"
#include "ui_formulaui.h"
#include "ValueModelBase.h"
#include "MysqlUtils.h"
#include <QMessageBox>
#include "DataManager.h"
#include <QtDebug>

FormulaUI::FormulaUI(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FormulaUI)
{
	ui->setupUi(this);

	connect(ui->GenerateFormula, SIGNAL(clicked()), this, SLOT(generateFormulate()));
	connect(ui->predictBT, SIGNAL(clicked()), this, SLOT(predictNextLotter()));
	connect(this, SIGNAL(FormulaChanged(TrueFormula*)), this, SLOT(ChangedFormula(TrueFormula*)));
	connect(this, SIGNAL(initFunc()), this, SLOT(initFuncSlot()));

	ui->FormulaListUI->setColumnWidth(0, 527);
	ui->FormulaListUI->setShowGrid(false);
	ui->FormulaListUI->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->FormulaListUI->setEditTriggers(QTableWidget::NoEditTriggers);
	ui->FormulaListUI->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui->LotterList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->LotterList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	
}

FormulaUI::~FormulaUI()
{
	delete ui;
}

void FormulaUI::generateFormulate()
{
	clearLotterItem();
	int eleInt = ui->eleEdit->text().toInt();
	int forInt = ui->FormulaEdit->text().toInt();
	if (eleInt <= 0 || forInt <= 0)
	{
		return;
	}

	FormulaList* tmpList = FormulaList::RandGenerate(eleInt, forInt);
	fl->addList(tmpList);

	for (auto& obj: tmpList->getList())
	{
		emit FormulaChanged(obj);
	}
	
}

void FormulaUI::ChangedFormula(TrueFormula* ret)
{
	int cmax = ret->getMaxContinue();
	int crecent = ret->getRecentContinue();

	qDebug() << cmax << crecent;
	addTableItem(ret->getText(),cmax,crecent);
}

void FormulaUI::initFuncSlot()
{
	for (auto& obj: fl->getList())
	{
		int cmax = obj->getMaxContinue();
		int crecent = obj->getRecentContinue();

		qDebug() << cmax << crecent;
		addTableItem(obj->getText(), cmax, crecent);
	}
	updateTableItem(0, 5, 6);
}

void FormulaUI::addTableItem(QString text, int maxContinue, int recentContinue)
{
	auto flUI = ui->FormulaListUI;
	int count = flUI->rowCount();
	flUI->setRowCount(count + 1);
	flUI->setItem(count, 0, new QTableWidgetItem(text));
	if (maxContinue > -1 && recentContinue > -1)
	{
		flUI->setItem(count, 1, new QTableWidgetItem(QString::number(maxContinue)));
		flUI->setItem(count, 2, new QTableWidgetItem(QString::number(recentContinue)));
	}
}

void FormulaUI::addTableItem(QString text)
{
	addTableItem(text, -1, -1);
}

void FormulaUI::updateTableItem(int index, int maxContinue, int recentContinue)
{
	auto flUI = ui->FormulaListUI;
	int count = flUI->rowCount();
	if (index >= count)
	{
		qDebug() << "index is out range!";
		return;
	}
	
	if (maxContinue >= 0 && recentContinue >= 0)
	{
		flUI->setItem(index, 1, new QTableWidgetItem(QString::number(maxContinue)));
		flUI->setItem(index, 2, new QTableWidgetItem(QString::number(recentContinue)));
	}

}

void FormulaUI::updateTableItem(QString text, int maxContinue, int recentContinue)
{
	auto flUI = ui->FormulaListUI;
	int count = flUI->rowCount();
	int index = -1;
	for (size_t i = 0; i < count; i++)
	{
		auto obj = flUI->item(i, 0)->text();
		if (text == obj)
		{
			index = i;
			break;
		}
	}
	
	if (index >=0)
	{
		if (maxContinue >= 0 && recentContinue >= 0)
		{
			flUI->setItem(index, 1, new QTableWidgetItem(QString::number(maxContinue)));
			flUI->setItem(index, 2, new QTableWidgetItem(QString::number(recentContinue)));
		}
	}
	else
	{
		qDebug() << "is not exist this formula: " + text;
	}
}

void FormulaUI::addLotterItem(Lotter lotter)
{
	auto llUI = ui->LotterList;
	int count = llUI->rowCount();
	if (lotter.redBall.size() != 6 || lotter.greenBall <= 0 || lotter.greenBall > 16)
	{
		qDebug() << "lotter is error!";
		return;
	}
	llUI->setRowCount(count + 1);

	for (size_t i = 0; i < 6; i++)
	{
		int ball = lotter.redBall.at(i);
		llUI->setItem(count, i, new QTableWidgetItem(QString::number(ball)));
	}
	llUI->setItem(count, 6, new QTableWidgetItem(QString::number(lotter.greenBall)));
}

void FormulaUI::clearLotterItem()
{
	auto llUI = ui->LotterList;
	llUI->clearContents();
	llUI->setRowCount(0);
}

QList<Lotter> FormulaUI::getLottersFromItem()
{
	QList<Lotter> lotters;
	auto llUI = ui->LotterList;
	int count = llUI->rowCount();

	for (size_t i = 0; i < count; i++)
	{
		Lotter lotter;
		for (size_t j = 0; j < 6; j++)
		{
			lotter.redBall.push_back(llUI->item(i, j)->text().toInt());
		}
		lotter.greenBall = llUI->item(i, 6)->text().toInt();
		lotters.push_back(lotter);
	}
	return lotters;
}

void FormulaUI::clearAllLotter()
{
	clearLotterItem();
}

Lotter FormulaUI::getLotterByQuery(QSqlQuery query)
{
	QList<int> redBall;
	int greenBall=0;
	int i = 0;
	while (query.next())
	{
		if (i==6)
		{
			greenBall = query.value(0).toInt() - 100;
			break;
		}
		redBall.push_back(query.value(0).toInt());
		i++;
	}
	if (greenBall != 0)
	{
		return Lotter(redBall, greenBall, QDate::currentDate());
	}
	return Lotter();
}

QList<Lotter> FormulaUI::getLByMC(int v1, int num)
{
	QList<Lotter> ql_out;
	for (int i = 0; i < num; i++)
	{
		Lotter lotter = getLotterByQuery(_sqlUT->excuteCall("predictNextByMax(%d)", v1));
		ql_out.push_back(lotter);
	}
	return ql_out;
}

QList<Lotter> FormulaUI::getLByRC(int v1, int num)
{
	QList<Lotter> ql_out;
	for (int i = 0; i < num; i++)
	{
		Lotter lotter = getLotterByQuery(_sqlUT->excuteCall("predictNextByRecent(%d)", v1));
		ql_out.push_back(lotter);
	}
	return ql_out;
}

QList<Lotter> FormulaUI::getLByMRC(int v1, int v2, int num)
{
	QList<Lotter> ql_out;
	for (int i = 0; i < num; i++)
	{
		Lotter lotter = getLotterByQuery(_sqlUT->excuteCall("predictNextByMR(%d,%d)", v1,v2));
		ql_out.push_back(lotter);
	}
	return ql_out;
}

void FormulaUI::predictNextLotter()
{
	QList<Lotter> qlotter_list;
	QString error_out;
	int num = ui->PredictNumberEdit->text().toInt();
	int mc = ui->MaxContinue->text().toInt();
	int rc = ui->RecentContinue->text().toInt();
	qDebug() << num << mc << rc;
	if (mc < 0 || rc < 0 || (mc == 0 && rc == 0))
	{
		QMessageBox box;
		box.setText("请输入正整数");
		box.exec();
		return;
	}

	int sum_predict = 0;
	if (mc > 0 && rc == 0)
	{
		sum_predict = _sqlUT->excuteFunc("getMCNum(%d)");
	}
	else if (rc > 0 && mc == 0)
	{
		sum_predict = _sqlUT->excuteFunc("getRCNum(%d)");
	}
	else if (mc > 0 && rc > 0){
		sum_predict = _sqlUT->excuteFunc("getMRCNum(%d,%d)");
	}

	qDebug() << sum_predict;

	if (sum_predict < 50)
	{
		
		QMessageBox box;
		box.setText("样本过少，请更改条件");
		box.exec();
		return;
	}

	if (mc > 0 && rc==0)
	{
		qlotter_list = getLByMC(mc, num);
	}
	else if (rc > 0 && mc == 0)
	{
		qlotter_list = getLByRC(rc, num);
	}
	else if (mc > 0 && rc > 0){
		qlotter_list = getLByMRC(mc, rc , num);
	}

	if (qlotter_list.size() > 0)
	{
		for(auto& obj: qlotter_list){
			addLotterItem(obj);
		}
	}
}