#include "selectui.h"
#include "ui_selectui.h"
#include <QFileDialog>
#include <QFile>
#include <QRegExp>
#include  <QMessageBox>
#include <QTextStream>
#include <QIcon>
selectUI::selectUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::selectUI)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->horizontalLayout_5);
    this->setWindowTitle("S5GSDmaker");
    QIcon icon(":/icon.bmp");
    this->setWindowIcon(icon);
}

selectUI::~selectUI()
{
    delete ui;
}

vector<QString> selectUI::getGSDFile()
{
    QString fileName = QFileDialog::getOpenFileName(
                     this,
                     tr("GSD File"),
                    NULL,// ".",
                     tr("*.*"));
    QFile f(fileName);

    vector<QString> lines;

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this,"sdf","can't open",QMessageBox::Yes);
        f.close();
        return lines;
    }

    QString temp;
    QString t2;
    bool flag=0;
    int i=0;
    while(!f.atEnd()){
        temp = QString(f.readLine());

        if (!temp.isEmpty()){
            for(i=0;i<temp.size();i++){
                if(temp.at(i)=='\"'){
                    if(!flag){
                        flag=1;
                    }else{
                        flag=0;
                    }
                }
                if(temp.at(i)==';'&&!flag){
                    t2 = temp.mid(0,i);
                    lines.push_back(t2);
                    break;
                }
            }
            if(i==temp.size()){
                lines.push_back(temp);
            }
        }
    }
    f.close();
    return lines;
}

vector<vector<int> > selectUI::findMods(vector<QString> &src, QString start, QString end)
{
    int i,j;
    vector<vector<int> > mods;
    QString pat1 ="\\b" + start + "\\s*=";
    QString pat2 ="\\b" + end + "\\b";
    QRegExp refRegExp(pat1);
    QRegExp refRegExp2(pat2);

    vector<int> modIndex;
    QString temp;
    for (i=0;i<src.size();i++){
        temp = src.at(i);
        if (refRegExp.indexIn(temp)>-1){
            for(j=i+1;j<src.size();j++){
                temp = src.at(j);
                if (refRegExp2.indexIn(temp)>-1){
                    modIndex.clear();
                    modIndex.push_back(i);
                    modIndex.push_back(j);
                    mods.push_back(modIndex);
                    i = j+1;
                    break;
                }
            }
        }
    }
    return mods;
}

vector<vector<int> > selectUI::refFinder(vector<QString> &src, std::vector<vector<int> > & ori,vector<int> &ids)
{
    int i,j,k;
    vector<vector<int> > refs;
    QRegExp refRegExp("(Ext_User_Prm_Data_Ref\\(\\d+\\)|Prm_Text_Ref)\\s*=\\s*(\\d+)");

    vector<int> ref;
    vector<int> refOri;
    QString temp;
    for (i=0;i<ori.size();i++){
        refOri = ori[i];
        ref.clear();
        for (j=refOri[0]+1;j<refOri[1];j++){
            temp = src.at(j);
            if (refRegExp.indexIn(temp)>-1){
                for (k=0;k<ids.size();k++){
                    if(ids.at(k)==refRegExp.cap(2).toInt()){
                        ref.push_back(k);
                        break;
                    }
                }
            }
        }
        refs.push_back(ref);

    }
    return refs;
}

QList<QString> selectUI::idFinder(vector<QString> &src, std::vector<vector<int> > & ori)
{
    int i;
    QList<QString> ids;
    QRegExp idRegExp("=\\s*\"(.*)\"");

    vector<int> refOri;

    for (i=0;i<ori.size();i++){
        refOri = ori[i];
        if (idRegExp.indexIn(src.at(refOri[0]))>-1){
            ids.push_back(idRegExp.cap(1));
        }
    }
    return ids;
}

vector<int> selectUI::numFinder(vector<QString> &src, std::vector<vector<int> > & ori)
{
    int i;
    vector<int> nums;
    QRegExp idRegExp("=\\s*(\\d+)");
    vector<int> refOri;
    for (i=0;i<ori.size();i++){
        refOri = ori[i];
        if (idRegExp.indexIn(src.at(refOri[0]))>-1){
            nums.push_back(idRegExp.cap(1).toInt());
        }
    }
    return nums;
}

void selectUI::on_pushButton_clicked()
{

    src = getGSDFile();

    modOri = findMods(src,"Module","EndModule");
    euOri = findMods(src,"ExtUserPrmData","EndExtUserPrmData");
    ptOri = findMods(src,"PrmText","EndPrmText");

    modIDs = idFinder(src,modOri);
    selMods.clear();
    euIDs = numFinder(src,euOri);
    ptIDs = numFinder(src,ptOri);

    modRefs = refFinder(src,modOri,euIDs);
    euRefs = refFinder(src,euOri,ptIDs);

    ui->modList->clear();
    ui->modList->addItems(modIDs);

    ui->modSel->clear();
    ui->num->setText(QString::number(modIDs.size(), 10));
}

void selectUI::on_modList_currentRowChanged(int currentRow)
{
    cRow = currentRow;
}

void selectUI::on_pushButton_2_clicked()
{
    if (!modIDs.isEmpty()){
        if(!selMods.contains(cRow)){
            ui->modSel->addItem(modIDs.at(cRow));
            selMods.push_back(cRow);
            ui->numS->setText(QString::number(selMods.size(), 10));
        }
    }
}

void selectUI::on_pushButton_3_clicked()
{
    if (!selMods.isEmpty()){
        if(selMods.size()>1){
            ui->modSel->takeItem(scRow);
            selMods.removeAt(scRow);
            ui->numS->setText(QString::number(selMods.size(), 10));
        }else if (selMods.size()==1){
            ui->modSel->clear();
            selMods.clear();
            ui->numS->setText(QString::number(selMods.size(), 10));
        }
    }
}

void selectUI::on_modSel_currentRowChanged(int currentRow)
{
    scRow = currentRow;
}

void selectUI::on_pushButton_4_clicked()
{
    if (!selMods.isEmpty()){
        dst = src;
        int modSel[modIDs.size()] = {0};
        int euSel[euIDs.size()] = {0};
        int ptSel[ptIDs.size()] = {0};

        int i=0;
        int j=0;
        int k=0;

        int sm = 0;

        int se = 0;
        vector<int> modRef;

        int sp = 0;
        vector<int> euRef;

        vector<int> temp;
        for (i=0;i<selMods.size();i++){
            sm = selMods.at(i);
            modSel[sm] = 1;

            modRef = modRefs.at(sm);
            for (j=0;j<modRef.size();j++){
                se = modRef.at(j);
                euSel[se] = 1;

                euRef = euRefs.at(se);
                for (k=0;k<euRef.size();k++){
                    sp = euRef.at(k);
                    ptSel[sp] = 1;
                }
            }
        }

        for (i=0;i<modOri.size();i++){
            if(modSel[i]==0){
                temp = modOri.at(i);
                for(j=temp.at(0);j<=temp.at(1);j++){
                    dst[j] = "";
                }
            }
        }
//        for (i=0;i<euOri.size();i++){
//            if(euSel[i]==0){
//                temp = euOri.at(i);
//                for(j=temp.at(0);j<=temp.at(1);j++){
//                    dst[j] = "";
//                }
//            }
//        }
//        for (i=0;i<ptOri.size();i++){
//            if(ptSel[i]==0){
//                temp = ptOri.at(i);
//                for(j=temp.at(0);j<=temp.at(1);j++){
//                    dst[j] = "";
//                }
//            }
//        }


        QString fileName = QFileDialog::getSaveFileName(this,
                tr("save GSD"),
                "",
                tr("GSD (*.gse)"));
        if (!fileName.isNull()){
            QFile file(fileName);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
               QMessageBox::warning(this,"warning","can't open",QMessageBox::Yes);
               return ;
            }
            QTextStream in(&file);
            for (int i=0;i<dst.size();i++){
                QString temp = dst.at(i);
                temp = temp.trimmed();
                if(!temp.isEmpty()){
                    in<<temp<<endl;
                }
            }
            file.close();
        }
    }

}

void selectUI::on_modList_doubleClicked(const QModelIndex &index)
{
    cRow = index.row();
    on_pushButton_2_clicked();
}

void selectUI::on_modSel_doubleClicked(const QModelIndex &index)
{
    scRow = index.row();
    on_pushButton_3_clicked();
}

void selectUI::on_pushButton_5_clicked()
{
    if (!modIDs.isEmpty()){
        int euSel[euIDs.size()] = {0};
        int num=0;
        int i=0;
        int j=0;
        int se = 0;
        vector<int> modRef;

        for (i=0;i<modIDs.size();i++){
            modRef = modRefs.at(i);
            for (j=0;j<modRef.size();j++){
                se = modRef.at(j);
                euSel[se] = 1;
            }
        }
        for(i=0;i<euIDs.size();i++){
            num += euSel[i];
        }
        if(num>150){
            QMessageBox::warning(this,"警告","可能不兼容",QMessageBox::Yes);
        }else{
            QMessageBox::information(this,"通知","检测通过，兼容",QMessageBox::Yes);
        }
    }
}

void selectUI::on_pushButton_6_clicked()
{
    if (!selMods.isEmpty()){
        int euSel[euIDs.size()] = {0};
        int num=0;
        int i=0;
        int j=0;

        int sm = 0;

        int se = 0;
        vector<int> modRef;

        for (i=0;i<selMods.size();i++){
            sm = selMods.at(i);

            modRef = modRefs.at(sm);
            for (j=0;j<modRef.size();j++){
                se = modRef.at(j);
                euSel[se] = 1;
            }
        }
        for(i=0;i<euIDs.size();i++){
            num += euSel[i];
        }
        if(num>150){
            QMessageBox::warning(this,"警告","可能不兼容，请少选几个模块",QMessageBox::Yes);
        }else{
            QMessageBox::information(this,"通知","检测通过，兼容",QMessageBox::Yes);
        }
    }
}
