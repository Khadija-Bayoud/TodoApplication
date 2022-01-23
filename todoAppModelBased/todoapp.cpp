#include "todoapp.h"
#include "ui_todoapp.h"
#include <QMessageBox>
#include <task.h>

todoApp::todoApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::todoApp)
{
    ui->setupUi(this);


    //Définir le model
    todayTaskModel = new QSqlQueryModel;
    pendingTaskModel = new QSqlQueryModel;
    finishedTaskModel = new QSqlQueryModel;

    //se connecter à la base de données
    connectDB();

    //Afficher le contenu de la base de données
    showDB();

    //Make connections
    connect(ui->todayTask, &QTableView::doubleClicked, this, &todoApp::editTaskSlot);

}

todoApp::~todoApp()
{
    delete ui;
    delete todayTaskModel;
    delete pendingTaskModel;
    delete finishedTaskModel;
}

void todoApp::connectDB()
{
    //Add the database with the SQLITE driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    //Set the database path
    db.setDatabaseName(this->filename);

    //check if the database is opened
    if(!db.open())
        QMessageBox::critical(this, "FAILED", "DB is not opened");

    //définir la requete sur la base de données
    auto todayTaskQuery = QSqlQuery(db);

    //Create a table named todayTasks
    QString createTodayTask{"CREATE TABLE IF NOT EXISTS todaytasks"
                            "(description VARCHAR(80),"
                            "date DATE, tag VARCHAR(10), finished BOOLEAN)"};

    //Exécuter la requete create
    if(!todayTaskQuery.exec(createTodayTask))
        QMessageBox::critical(this,"Info","Cannot create the table");

    //create the pendingtasks table
    auto  pendingTaskQuery = QSqlQuery(db);
    QString createPendingTask{"CREATE TABLE IF NOT EXISTS pendingtasks"
                              "(description VARCHAR(80),date DATE,"
                              "tag VARCHAR(10),finished BOOLEAN)"};
    if(!pendingTaskQuery.exec(createPendingTask))
        QMessageBox::critical(this,"Info","Cannot create the table");

    //create the pendingtasks table
    auto  finishedTaskQuery = QSqlQuery(db);
    QString createfinishedTask{"CREATE TABLE IF NOT EXISTS finishedtasks"
                               "(description VARCHAR(80),date DATE,"
                               "tag VARCHAR(10),finished BOOLEAN)"};
    if(!finishedTaskQuery.exec(createfinishedTask))
        QMessageBox::critical(this,"Info","Cannot create the table");
}

void todoApp::showDB()
{
    //Show today tasks
        auto todayQuery = QSqlQuery(db);
        QString today{"SELECT * FROM todaytasks"};
        todayQuery.exec(today);
        todayTaskModel->setQuery(todayQuery);
        ui->todayTask->setModel(todayTaskModel);

    //Show pending tasks
         auto pendingQuery = QSqlQuery(db);
         QString pending{"SELECT * FROM pendingtasks"};
         pendingQuery.exec(pending);
         pendingTaskModel->setQuery(pendingQuery);
         ui->pendingTask->setModel(pendingTaskModel);



    //Show finished tasks
        auto finishedQuery = QSqlQuery(db);
        QString finished{"SELECT * FROM finishedtasks"};
        finishedQuery.exec(finished);
        finishedTaskModel->setQuery(finishedQuery);
        ui->finishedTask->setModel(finishedTaskModel);

}

void todoApp::on_actionNew_Task_triggered()
{
    //create the dialog
    Task T;

    //Execute the dialog
    auto reply = T.exec();

    //if the the dialog is accepted, show the task in the appropriate table view
    if(reply == Task::Accepted)
    {
        addEnty(T.getDesc(), T.getDate(), T.getTag(), T.getStatus());

        if(T.getDate() == QDate::currentDate())
        {
            //définir la requete sur la base de données
            auto  query = QSqlQuery(db);
            QString view{"SELECT * FROM todaytasks"};
            query.exec(view);
            todayTaskModel->setQuery(query);
            ui->todayTask->setModel(todayTaskModel);
        }
        else if(T.getDate() > QDate::currentDate())
        {
            //définir la requete sur la base de données
            auto  query = QSqlQuery(db);
            QString view{"SELECT * FROM pendingtasks"};
            query.exec(view);
            pendingTaskModel->setQuery(query);
            ui->pendingTask->setModel(pendingTaskModel);
        }
        else
        {
            //définir la requete sur la base de données
            auto  query = QSqlQuery(db);
            QString view{"SELECT * FROM finishedtasks"};
            query.exec(view);
            finishedTaskModel->setQuery(query);
            ui->finishedTask->setModel(finishedTaskModel);
        }

    }

}

void todoApp::addEnty(QString desc, QDate date, QString tag, QString finished)
{

    if(date == QDate::currentDate())
    {
        //définir la requete sur la base de données
        auto query = QSqlQuery(db);

        //Ajouter les champs du dialogue à la base de données
        QString addInfo{"INSERT INTO todaytasks (description, date, tag, finished)"
                        " VALUES('%1','%2', '%3', '%4')"};

        //Executer la requete
        if(!query.exec(addInfo.arg(desc).arg(date.toString()).arg(tag).arg(finished)))
            QMessageBox::critical(this,"Info","Cannot add the Entry");

        //Connecter le modèle à la requete
        todayTaskModel->setQuery(query);
    }
    else if(date > QDate::currentDate())
    {
        auto query = QSqlQuery(db);
        QString addInfo{"INSERT INTO pendingtasks VALUES('%1','%2', '%3', '%4')"};
        if(!query.exec(addInfo.arg(desc).arg(date.toString()).arg(tag).arg(finished)))
            QMessageBox::critical(this,"Info","Cannot add the Entry");
        pendingTaskModel->setQuery(query);
    }
    else
    {
        auto query = QSqlQuery(db);
        QString addInfo{"INSERT INTO finishedtasks VALUES('%1','%2', '%3', '%4')"};
        if(!query.exec(addInfo.arg(desc).arg(date.toString()).arg(tag).arg(finished)))
            QMessageBox::critical(this,"Info","Cannot add the Entry");
        finishedTaskModel->setQuery(query);
    }
}

void todoApp::on_action_Exit_triggered()
{
    QApplication::quit();
}

void todoApp::on_actionToday_Task_triggered()
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

void todoApp::on_actionPending_Task_triggered()
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

void todoApp::on_actionFinished_Tasks_triggered()
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

void todoApp::on_actionDelete_Tasks_triggered()
{
    //Get the selected rows in todayTasks
    QModelIndexList selectToday = ui->todayTask->selectionModel()->selectedRows();

    //Remove the selected rows from todayTaskModel
    for(int i=0; i<selectToday.count(); i++)
    {
        //Define a query on the DB
        auto query = QSqlQuery(db);

        //Define the body of the query
        QString del{"DELETE FROM todaytasks WHERE description="
                    "'"+selectToday[i].data(Qt::EditRole).toString()+"'"};

        //Execute the query
        if(!query.exec(del))
            qDebug() << "Cannot delete row!";

        //set the query to the model
        todayTaskModel->setQuery(query);

        //submit changes
        todayTaskModel->submit();

        //show changes
        auto todayQuery = QSqlQuery(db);
        QString today{"SELECT * FROM todaytasks"};
        todayQuery.exec(today);
        todayTaskModel->setQuery(todayQuery);
        ui->todayTask->setModel(todayTaskModel);

    }

    //Get the selected rows in todayTasks
    QModelIndexList selectPending = ui->pendingTask->selectionModel()->selectedRows();

    //Remove the selected rows from todayTaskModel
    for(int i=0; i<selectPending.count(); i++)
    {
        //Define a query on the DB
        auto query = QSqlQuery(db);

        //Define the body of the query
        QString del{"DELETE FROM pendingtasks WHERE description="
                    "'"+selectPending[i].data(Qt::EditRole).toString()+"'"};

        //Execute the query
        if(!query.exec(del))
            qDebug() << "Cannot delete row!";

        //set the query to the model
        pendingTaskModel->setQuery(query);

        //submit changes
        pendingTaskModel->submit();

        //show changes
        auto pendingQuery = QSqlQuery(db);
        QString pending{"SELECT * FROM pendingtasks"};
        pendingQuery.exec(pending);
        pendingTaskModel->setQuery(pendingQuery);
        ui->pendingTask->setModel(pendingTaskModel);

    }

    //Get the selected rows in todayTasks
    QModelIndexList selectFinished = ui->finishedTask->selectionModel()->selectedRows();

    //Remove the selected rows from todayTaskModel
    for(int i=0; i<selectFinished.count(); i++)
    {
        //Define a query on the DB
        auto query = QSqlQuery(db);

        //Define the body of the query
        QString del{"DELETE FROM finishedtasks WHERE description="
                    "'"+selectFinished[i].data(Qt::EditRole).toString()+"'"};

        //Execute the query
        if(!query.exec(del))
            qDebug() << "Cannot delete row!";

        //set the query to the model
        finishedTaskModel->setQuery(query);

        //submit changes
        finishedTaskModel->submit();

        //show changes
        auto finishedQuery = QSqlQuery(db);
        QString finished{"SELECT * FROM finishedtasks"};
        finishedQuery.exec(finished);
        finishedTaskModel->setQuery(finishedQuery);
        ui->finishedTask->setModel(finishedTaskModel);

    }
}

void todoApp::editTaskSlot()
{
//    auto currRow = ui->todayTask->currentIndex().row();

//    qDebug() << currRow;

//    Task T;
//    auto reply = T.exec();

//    if(reply == Task::Accepted)
//    {
//        auto query = QSqlQuery(db);

//        //Ajouter les champs du dialogue à la base de données
//        QString updateInfo{"UPDATE todaytasks (description, date, tag, finished)"
//                        " VALUES('%1','%2', '%3', '%4') WHERE id =" + currRow};

////        //Executer la requete
//        if(!query.exec(updateInfo.arg(T.getDesc()).arg(T.getDate().toString()).arg(T.getTag()).arg(T.getStatus())))
//            QMessageBox::critical(this,"Info","Cannot Update the Entry");

//        //Connecter le modèle à la requete
//        todayTaskModel->setQuery(query);
//    }

}

//id INTEGER PRIMARY KEY,
