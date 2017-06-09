#ifndef SELECTUI_H
#define SELECTUI_H

#include <QMainWindow>
#include <QString>
using namespace std;
namespace Ui {
class selectUI;
}

class selectUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit selectUI(QWidget *parent = 0);
    ~selectUI();
    vector<QString> getGSDFile();
    vector<QString> src;
    vector<QString> dst;

    vector<vector<int> > findMods(vector<QString> & src,QString start,QString end);
    vector<vector<int> > modOri;
    vector<vector<int> > euOri;
    vector<vector<int> > ptOri;

    vector<vector<int> > refFinder(vector<QString> & src,std::vector<vector<int> > & ori,vector<int> &ids);
    vector<vector<int> > modRefs;
    vector<vector<int> > euRefs;

    QList<QString> idFinder(vector<QString> & src,std::vector<vector<int> > & ori);
    std::vector<int> numFinder(vector<QString> & src,std::vector<vector<int> > & ori);
    QList<QString> modIDs;
    std::vector<int> euIDs;
    std::vector<int> ptIDs;
private slots:
    void on_pushButton_clicked();


    void on_modList_currentRowChanged(int currentRow);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_modSel_currentRowChanged(int currentRow);

    void on_pushButton_4_clicked();

    void on_modList_doubleClicked(const QModelIndex &index);

    void on_modSel_doubleClicked(const QModelIndex &index);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::selectUI *ui;
    int cRow;
    int scRow;
    QList<int> selMods;
};

#endif // SELECTUI_H
