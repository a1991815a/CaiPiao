#ifndef FORMULAUI_H
#define FORMULAUI_H

#include <QDialog>
#include "TrueFormula.h"
#include "mainwindow.h"

struct Lotter;

namespace Ui {
class FormulaUI;
}

class FormulaUI : public QDialog
{
	Q_OBJECT

public:
	explicit FormulaUI(QWidget *parent = 0);
	~FormulaUI();

	inline void initFormula(FormulaList* in, MainWindow* m){ fl = in; mainPoint = m; emit initFunc(); };

	Lotter getLotterByQuery(QSqlQuery);

private:
	QList<Lotter> getLByMC(int,int);
	QList<Lotter> getLByRC(int,int);
	QList<Lotter> getLByMRC(int,int,int);

signals:
	void FormulaChanged(TrueFormula*);
	void initFunc();

private slots:
void generateFormulate();

void predictNextLotter();
void clearAllLotter();

void ChangedFormula(TrueFormula*);

void initFuncSlot();

private:
	Ui::FormulaUI *ui;
	FormulaList* fl;
	MainWindow* mainPoint;



private:
	void addTableItem(QString);
	void addTableItem(QString,int,int);

	void updateTableItem(int, int, int);
	void updateTableItem(QString, int, int);

	void addLotterItem(Lotter);
	void clearLotterItem();
	QList<Lotter> getLottersFromItem();
};

#endif // FORMULAUI_H
