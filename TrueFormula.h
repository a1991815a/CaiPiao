#ifndef __TrueFormula__
#define __TrueFormula__
#include "Formula.h"
#include <QtCore>
#include "QSqlQuery"
#include "QMap"

struct Lotter;
class TrueFormula;

struct Formula_Value
{
	Formula_Value(){};
	Formula_Value(int v1, QDate v2, int v3){ index_id = v1; the_date = v2; predict = v3; };
	Formula_Value(int v1, QDate v2){ index_id = v1; the_date = v2; };
	int index_id;
	QDate the_date;
	int predict;
	bool operator==(Formula_Value fv);
	TrueFormula* the_formula;
};

class TrueFormula{
public:
	void parse();

	int getMax();
	int getMin();
	int getValue(Lotter);
	QString getText();

	int getRedBall(Lotter);
	int getGreenBall(Lotter);
	
	bool isLegal();
	bool CompareFormula(TrueFormula*);

	bool releaseThis();

	inline int getRecentContinue() const { return recent_continue; }
	inline int getMaxContinue() const { return max_continue; }

	inline int getIndexId() const { return indexId; }
	inline void setIndexId(int val) { indexId = val; }

private:
	TrueFormula(Element*);
	~TrueFormula();
	Formula* _formula;
	int _min, _max;
	const QList<Lotter>* const lotter_list;
	int* const _index;

	int max_continue, recent_continue;
	int indexId;

public:
	static TrueFormula* RandGenerate(int);
	static QList<TrueFormula*> RandGenerate(int, int);
	static TrueFormula* CreateByString(QString);
	static TrueFormula* createBy3V(QString, int, int);
	static TrueFormula* createBy5V(QString, int, int, int, int);
	static TrueFormula* createBy6V(QString, int, int, int, int, int);
	static TrueFormula* createByQuery(QSqlQuery);
	QSqlQuery insertIntoDB();
	void InitIndexId();
};

class FormulaList{
public:
	FormulaList();
	void push_back(QString);
	void push_back(TrueFormula*);
	inline int size(){ return _list.size(); };
	inline TrueFormula* get(int index){ return _list.at(index); };
	
	void writeToFile();
	QList<TrueFormula*> getList(){ return _list; };

	void releaseAll();
	void releaseOnly();

	void addList(FormulaList*);
	bool containsByText(TrueFormula*);
private:
	~FormulaList(){};
	QList<TrueFormula*> _list;
	int PageCount;
	int CurrentPage;

public:
	static FormulaList* RandGenerate(int, int);
	static FormulaList* GenerateFromFile();
	static FormulaList* createByQuery(QSqlQuery);
	static FormulaList* createByDatabase(int,int);
	void insertIntoDB();

};

#endif
