#include "TrueFormula.h"
#include "CustomDefine.h"
#include "mainwindow.h"
#include "ValueModelBase.h"
#include "MysqlUtils.h"

TrueFormula::TrueFormula(Element* ele) 
	:_min(-1), _max(-1), max_continue(-1), recent_continue(-1),
	lotter_list(&MainWindow::allLotters), _index(&MainWindow::currentListIndex),
	indexId(-1)
{
	this->_formula = dynamic_cast<Formula*>(ele);
}

TrueFormula::~TrueFormula()
{
	SAFE_DELETE(_formula);
}

TrueFormula* TrueFormula::RandGenerate(int number)
{
	auto ele = Formula::RandGenerate(number);
	TrueFormula* tf = new TrueFormula(ele);
	auto query = tf->insertIntoDB();
	if (query.lastError().text().size()>2)
	{
		tf->releaseThis();
		tf = TrueFormula::RandGenerate(number);
	}
	tf->InitIndexId();
	return tf;
}

QList<TrueFormula*> TrueFormula::RandGenerate(int number, int numberFormula)
{	
	QList<TrueFormula*> formule_list;

	for (size_t i = 0; i < numberFormula; i++)
	{
		while (true)
		{
			auto ele = TrueFormula::RandGenerate(number);
			if (ele->isLegal())
			{
				formule_list.push_back(ele);
				break;
			}
			else
			{
				SAFE_DELETE(ele);
			}
		}

	}

	return formule_list;
}

int TrueFormula::getMin()
{
	if (_min == -1)
	{
		_formula->Calculate(Element::DataMin);
		_min = _formula->getValue();
	}

	return _min;
}

int TrueFormula::getMax()
{
	if (_max == -1)
	{
		_formula->Calculate(Element::DataMax);
		_max = _formula->getValue();
	}
	return _max;
}

int TrueFormula::getValue()
{
	_formula->Calculate(Element::DataNormal);
	return _formula->getValue();
}

bool TrueFormula::isLegal()
{
	if (getMin() > 0 && getMax() > 0 && getMax() - getMin() >= 33 && getMax() - getMin() <= 99999)
	{
		return true;
	}
	return false;
}

bool TrueFormula::releaseThis()
{
	delete this;
	return true;
}

TrueFormula* TrueFormula::CreateByString(QString text)
{
	Formula* ret = new Formula(text);
	ret->Parse();
	return new TrueFormula(ret);
}

QString TrueFormula::getText()
{
	return _formula->getText();
}

bool TrueFormula::CompareFormula(TrueFormula* tf)
{
	if (getText().compare(tf->getText()))
	{
		return true;
	}
	return false;
}

int TrueFormula::getRedBall()
{
	return getValue() % 33 + 1;
}

int TrueFormula::getGreenBall()
{
	return getValue() % 16 + 1;
}

TrueFormula* TrueFormula::createBy3V(QString text, int max_value, int min_value)
{
	auto ret = TrueFormula::CreateByString(text);
	ret->_max = max_value;
	ret->_min = min_value;
	return ret;
}

TrueFormula* TrueFormula::createBy5V(QString text, int max_value, int min_value, int max_continue, int recent_continue)
{
	auto ret = TrueFormula::createBy3V(text, max_value, min_value);
	ret->max_continue = max_continue;
	ret->recent_continue = recent_continue;
	return ret;
}

TrueFormula* TrueFormula::createBy6V(QString text, int max_value, int min_value, int max_continue, int recent_continue, int in_id)
{
	auto ret = TrueFormula::createBy5V(text, max_value, min_value, max_continue, recent_continue);
	ret->setIndexId(in_id);
	return ret;
}

TrueFormula* TrueFormula::createByQuery(QSqlQuery query)
{
	TrueFormula* tf = nullptr;

	QDate tmp_date = query.value("last_record").toDate();
	QString tmp_text = query.value("text").toString();
	int max_value = query.value("max_value").toInt();
	int min_value = query.value("min_value").toInt();
	int max_continues = query.value("max_continues").toInt();
	int recent_continues = query.value("recent_continues").toInt();
	int tmp_id = query.value("formula_id").toInt();
	tf = createBy6V(tmp_text, max_value, min_value, max_continues, recent_continues, tmp_id);
	return tf;
}

QSqlQuery TrueFormula::insertIntoDB()
{
	return _sqlUT->excuteCall("insertFormula('%s',%d,%d)",getText(),getMin(),getMax());
}

void TrueFormula::InitIndexId()
{
	setIndexId(_sqlUT->excuteFunc("getIndexId('%s')", getText()));
}

FormulaList* FormulaList::RandGenerate(int number, int numberFormula)
{
	FormulaList* fl = new FormulaList();
	fl->_list = TrueFormula::RandGenerate(number, numberFormula);
	return fl;
}

void FormulaList::push_back(QString text)
{
	_list.push_back(TrueFormula::CreateByString(text));
}

void FormulaList::push_back(TrueFormula* tf)
{
	_list.push_back(tf);
}

FormulaList* FormulaList::GenerateFromFile()
{
	FormulaList* fl = new FormulaList();

	QFile file(MainWindow::genPath+"FormulaList.txt");
	qDebug() << (MainWindow::genPath + "FormulaList.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open the file!" << endl;
		SAFE_DELETE(fl);
		return nullptr;
	}
	QTextStream in(&file);
	QString in_text = in.readAll();
	file.close(); 
	
	if (in_text.isEmpty())
	{
		SAFE_DELETE(fl);
		return nullptr;
	}

	QList<QString> split_text = in_text.split(',');
	for (auto& obj: split_text)
	{
		fl->push_back(obj);
	}

	return fl;
}

void FormulaList::writeToFile()
{
	QFile file(MainWindow::genPath + "FormulaList.txt");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Can't open the file!" << endl;
	}

	QTextStream out(&file);

	QString out_text = "";
	for (auto& obj: _list)
	{
		out_text.append(',');
		out_text.append(obj->getText());
	}

	out_text.remove(0, 1);
	out << out_text;
	file.close();
}

void FormulaList::addList(FormulaList* list)
{
	for (auto& obj: list->_list)
	{
		if (!containsByText(obj))
		{
			_list.push_back(obj);
		}
	}
//	list->releaseOnly();
}

bool FormulaList::containsByText(TrueFormula* formula)
{
	for (auto& obj: _list)
	{
		if (obj->getText() == formula->getText())
		{
			return true;
		}
	}
	return false;
}

void FormulaList::releaseOnly()
{
	delete this;
}

void FormulaList::releaseAll()
{
	for (auto& obj : _list)
	{
		obj->releaseThis();
	}
	delete this;
}

FormulaList::FormulaList()
{
	int tmp_Count = _sqlUT->excuteFunc("getCountFormula()");
	PageCount = tmp_Count / 100;
	CurrentPage = 0;
}

FormulaList* FormulaList::createByQuery(QSqlQuery query)
{
	FormulaList* fl = new FormulaList();
	while (query.next())
	{
		fl->push_back(TrueFormula::createByQuery(query));
	}
	return fl;
}

FormulaList* FormulaList::createByDatabase(int page, int page_max)
{
	QSqlQuery query = _sqlUT->excuteCall("getFormulaByPage(%d,%d)", page, page_max);
	return FormulaList::createByQuery(query);
}

void FormulaList::insertIntoDB()
{
	for(auto& obj: _list){
		obj->insertIntoDB();
	}
}

bool Formula_Value::operator==(Formula_Value fv)
{
	if (index_id = fv.index_id && the_date == fv.the_date)
	{
		return true;
	}
	return false;
}
