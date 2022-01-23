#include "task.h"
#include "ui_task.h"

Task::Task(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Task)
{
    ui->setupUi(this);

    //Set window title
    this->setWindowTitle("New Task");

    //Set the current date in the DateEdit
    ui->dateEdit->setDate(QDate::currentDate());
}

Task::~Task()
{
    delete ui;
}

QString Task::getDesc(){
    return "Description: " + ui->lineEdit->text();
}

QString Task::getStatus(){

    if(ui->finished->isChecked())
        return "Finished: Yes";
    else
        return "Finished: No";

}

QString Task::getTag(){
    return "Tag: " + ui->Tag->currentText();
}

QDate Task::getDate(){
    return ui->dateEdit->date();
}


void Task::setDesc(QString desc){
    ui->description->setText(desc);
}

void Task::setTag(QString currText){
    ui->Tag->setCurrentText(currText);
}

void Task::setDate(QString str){

    //Split the str according to "/" char
    auto dateList = str.split("/");

    //Construire la date
    if(!dateList.isEmpty())
    {
        auto date = QDate(dateList.at(2).toInt(), dateList.at(1).toInt(), dateList.at(0).toInt());
        ui->dateEdit->setDate(date);
    }
}

void Task::setStatus(QString status){
    if(status == "Yes")
        ui->finished->setChecked(true);
    else
        ui->finished->setChecked(false);
}
