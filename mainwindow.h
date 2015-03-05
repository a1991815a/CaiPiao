#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "TrueFormula.h"
#include <QSystemTrayIcon>
#include "qevent.h"

#define CHANGE_TASK(A,B,C) MainWindow::getInstance()->changeTask(A,B,C)

namespace Ui {
class MainWindow;
}

struct Lotter
{
	Lotter(QList<int> red, int green, QDate date){ redBall = red; greenBall = green; this->date = date; };
	Lotter(QList<int> red, int green, QString date){
		redBall = red; greenBall = green;
		auto& obj = date.split('-');
		int year = obj.at(0).toInt();
		int month = obj.at(1).toInt();
		int day = obj.at(2).toInt();
		this->date = QDate(year, month, day);
	};
	Lotter(){};

	bool operator==(Lotter in){
		return date == in.date;
	};

	QList<int> redBall;
	int greenBall;
	QDate date;
	
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	void changeTask(int, int, bool);
	static QList<Lotter> allLotters;
	inline static MainWindow* getInstance(){ return _instance; };

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void Test1();

	QNetworkAccessManager* net;

	inline QList<Lotter> getrecent_lotter() const { return allLotters; }

	void exportCSV(QList<Lotter>);

	QList<Lotter> getLotterFromCSV();
	QList<Lotter> getLotter(QList<Lotter>);
	QList<Lotter> getFromDB();

	static int currentListIndex;

	static QString genPath;
signals:
void processChange(int);

private slots:
void changeProcess(int);

void replyFinished(QNetworkReply*);

void TEST_1();

void RowChanged(int);

void formulaBT();

void exitAction();

void onSystemTrayIconClicked(QSystemTrayIcon::ActivationReason);

private:
	Ui::MainWindow *ui;
	
	QList<Lotter> getLottersFromJson(QString);

	FormulaList* formula_list;

	QSystemTrayIcon* trayicon;

	friend class ValueModelBase;
	friend class FormulaTest;
	friend class TrueFormula;
	friend class FormulaList;
	friend class ExtractUtils;

protected:
	void closeEvent(QCloseEvent *event);
	QDate last_record;
	static MainWindow* _instance;
};

#endif // MAINWINDOW_H
