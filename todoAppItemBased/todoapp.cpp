#include "todoapp.h"
#include "ui_todoapp.h"
#include <task.h>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QRegExp>
#include <QDropEvent>
#include <QDebug>

todoApp::todoApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::todoApp)
{
    ui->setupUi(this);

    //set the window size

    //open content
    openContent(this->filename);

    //Connect the items of each list with the edit slot
    connect(ui->todayTask, &QListWidget::itemDoubleClicked, this, &todoApp::editTaskSlot);
    connect(ui->pendingTask, &QListWidget::itemDoubleClicked, this, &todoApp::editTaskSlot);
    connect(ui->finishedTask, &QListWidget::itemDoubleClicked, this, &todoApp::editTaskSlot);
}

todoApp::~todoApp()
{
    delete ui;
}


/*Save content function*/
void todoApp::saveContent(QString filename){

    //Create the file
    QFile file(filename);

    //if the file is opened in write role
    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        //Write into the file
        QTextStream in(&file);

        //Save all the items in todayTask list
        for(int i=0; i<ui->todayTask->count(); i++)
        {
            in << ui->todayTask->item(i)->text();

            //séparer les éléments de la liste par une ligne
            in << "\n";
        }

        //Séparer entre les lists par une une ligne
        in << "\n";

        //Save all the items in pendingTask list
        for(int i=0; i<ui->pendingTask->count(); i++)
        {
            in << ui->pendingTask->item(i)->text();
            in << "\n";
        }

        in << "\n";

        //Save all the items in finishedTask list
        for(int i=0; i<ui->finishedTask->count(); i++)
        {
            in << ui->finishedTask->item(i)->text();
            in << "\n";
        }
    }

    file.close();
}


/*Open content function*/
void todoApp::openContent(QString filename){

    //Create the file
    QFile file(filename);

    //if the file is opened in read Mode
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        //Read the file
        QTextStream out(&file);

        while(!out.atEnd())
        {
            //read a line
            QString line = out.readLine();

            if(!line.isEmpty())
            {
                    //Extract the status
                    auto splitLine = line.split("\t");
                    QString status = splitLine[3].remove(0,10);

                    //Extract the date of the format DD/MM/YYYY using QRegExp
                    QRegExp exp("(\\d{2})[/](\\d{2})[/](\\d{4})");
                    exp.indexIn(line);
                    auto dateString = exp.capturedTexts()[0].trimmed();

                     if(status == "No")
                     {
                            //Split the date according to "/" char
                            auto dateList = dateString.split("/");

                            //Construire la date
                            auto date = QDate(dateList.at(2).toInt(), dateList.at(1).toInt(), dateList.at(0).toInt());

                            //Set the item into the corresponding listWidget
                            if(date == QDate::currentDate())
                                 ui->todayTask->addItem(line);
                            else if(date > QDate::currentDate())
                                 ui->pendingTask->addItem(line);
                            else
                                 ui->finishedTask->addItem(line);
                       }
                       else
                            ui->finishedTask->addItem(line);
           }
    }

      file.close();
    }
}


/*File Menu Actions*/
void todoApp::on_newTask_triggered()
{
    //Create the taskDialog
    Task T;

    //Execute the taskDialog
    auto reply = T.exec();

    //If the dialog is accepted, set the information to the appropriate view
    if(reply == Task::Accepted)
    {
        if(T.getStatus() == "No")
        {
            if(T.getDate() == QDate::currentDate())
                ui->todayTask->addItem(T.getTask());
            else if(T.getDate() > QDate::currentDate())
                ui->pendingTask->addItem(T.getTask());
            else
                ui->finishedTask->addItem(T.getTask());
        }
        else
            ui->finishedTask->addItem(T.getTask());
    }

    //save content
    saveContent(this->filename);
}

void todoApp::on_Open_triggered()
{
    //Open a fileDialog to choose the Dir where the file is saved
    filename = QFileDialog::getOpenFileName();

    //load content
    openContent(filename);
}

void todoApp::on_Save_triggered()
{
    //Open a fileDialog to choose the Dir for saving
    filename = QFileDialog::getSaveFileName();

    //saveContent
    saveContent(filename);

}

void todoApp::on_exit_triggered()
{
    QApplication::quit();
}


/*Options Menu actions*/
void todoApp::on_todayTaskAct_triggered()
{
    if(ui->todayTask->isVisible() && ui->todayLabel->isVisible())
    {
        ui->todayTask->setVisible(false);
        ui->todayLabel->setVisible(false);
    }
    else
    {
        ui->todayTask->setVisible(true);
        ui->todayLabel->setVisible(true);
    }
}

void todoApp::on_pendingTaskAct_triggered()
{
    if(ui->pendingTask->isVisible() && ui->pendingLabel->isVisible())
    {
        ui->pendingTask->setVisible(false);
        ui->pendingLabel->setVisible(false);
    }
    else
    {
        ui->pendingTask->setVisible(true);
        ui->pendingLabel->setVisible(true);
    }
}

void todoApp::on_finishedTaskAct_triggered()
{
    if(ui->finishedTask->isVisible() && ui->finishedLabel->isVisible())
    {
        ui->finishedTask->setVisible(false);
        ui->finishedLabel->setVisible(false);
    }
    else
    {
        ui->finishedTask->setVisible(true);
        ui->finishedLabel->setVisible(true);
    }
}


/*Edit task Slot*/
void todoApp::editTaskSlot(){

    //Get the identity of the sender
    QListWidget *listWidget = dynamic_cast<QListWidget*>(sender());

    if(listWidget)
    {
        //create the dialog
        Task T;

        //get the content of selected item
        auto item = listWidget->currentItem()->text();

        //Split the item into fields : description, date, tag and finished
        auto splitItem = item.split("\t");

        //Fill the fields of the dialog
        T.setDesc(splitItem[0].remove(0, 13));
        T.setDate(splitItem[1].remove(0,5));
        T.setTag(splitItem[2].remove(0,5));
        T.setStatus(splitItem[3].remove(0,10));

        //Execute the dialog
        auto reply = T.exec();

        //If the task is edited
        if(reply == Task::Accepted)
        {
            if(T.getStatus() == "No")
            {
                    if(T.getDate() == QDate::currentDate())
                    {
                        //Remove the item from it previous list
                        listWidget->takeItem(listWidget->row(listWidget->currentItem()));

                        //put the item on the new list
                        ui->todayTask->addItem(T.getTask());
                    }
                    else if(T.getDate() > QDate::currentDate())
                    {
                        //Remove the item from it previous list
                        listWidget->takeItem(listWidget->row(listWidget->currentItem()));

                        //put the item on the new list
                        ui->pendingTask->addItem(T.getTask());
                    }
                    else
                    {
                        //Remove the item from it previous list
                        listWidget->takeItem(listWidget->row(listWidget->currentItem()));

                        //put the item on the new list
                        ui->finishedTask->addItem(T.getTask());
                    }
            }
            else
            {
                //Remove the item from it previous list
                listWidget->takeItem(listWidget->row(listWidget->currentItem()));

                //put the item on the new list
                ui->finishedTask->addItem(T.getTask());
            }

        }


    }

    //save content
    saveContent(this->filename);
}


/*Delete actions*/
void todoApp::on_actionDelete_Task_triggered()
{
    //Get the selected items in todayTasks
    QList<QListWidgetItem *> todayItems = ui->todayTask->selectedItems();

    //Remove the selected items
    if(!todayItems.isEmpty())
    {
        for(auto item : todayItems)
            ui->todayTask->takeItem(ui->todayTask->row(item));
    }


    //Get the selected items in pendingTasks
    QList<QListWidgetItem *> pendingItems = ui->pendingTask->selectedItems();

    //Remove the selected items
    if(!pendingItems.isEmpty())
    {
        for(auto item :pendingItems)
            ui->pendingTask->takeItem(ui->pendingTask->row(item));
    }


    //Get the selected items in finishedTasks
    QList<QListWidgetItem *> finishedItems = ui->finishedTask->selectedItems();

    //Remove the selected items
    if(!finishedItems.isEmpty())
    {
        for(auto item : finishedItems)
            ui->finishedTask->takeItem(ui->finishedTask->row(item));
    }

    //delete the items from the file

    //Create the file
    QFile file(this->filename);

    //if the file is opened in read Mode
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        //Read the file
        QTextStream out(&file);

        while(!out.atEnd())
        {
            //read a line
            QString line = out.readLine();

            //if I delete a today task, delete it from the file
            if(!todayItems.isEmpty())
            {
                for(int i=0; i<todayItems.size(); i++)
                {
                    if(line == todayItems[i]->text())
                    {
                        file.remove(line);
                    }

                }
            }

            //if I delete a pending task, delete it from the file
            if(!pendingItems.isEmpty())
            {
                for(int i=0; i<pendingItems.size(); i++)
                {
                    if(line == pendingItems[i]->text())
                    {
                        file.remove(line);
                    }

                }
            }

            //if I delete a finished task, delete it from the file
            if(!finishedItems.isEmpty())
            {
                for(int i=0; i<finishedItems.size(); i++)
                {
                    if(line == finishedItems[i]->text())
                    {
                        file.remove(line);
                    }

                }
            }

        }
    }

    file.close();

    //save content
    saveContent(this->filename);
}


