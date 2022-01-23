# TodoApplication
QMainWindow based application with MVC


## Table of Content
* [About the homework](#about-the-homework)
* [TodoApplication](#todoapplication)
  * [ItemBased Approach](#itembased-approach)
  * [ModelBased Approach](#modelbased-approach)

## About the homework
The goal of this homework is to create an application called "ToDoApp" to manage user's tasks.
This application has features of a mainwindow such as actions, menus and toolbar.
It is also capable of storing an archieve of pending and finished tasks.
To implement this feature, we used, in the first approach, [QListWidget](https://doc.qt.io/qt-5/qlistwidget.html) which is an ItemBased container. In the second approach,
we used [QTableView](https://doc.qt.io/qt-5/qtableview.html) with [QSqlQueryModel](https://doc.qt.io/qt-5/qsqlquerymodel.html).

## ItemBased Approach
To store an archieve of all tasks, we used [QFile](https://doc.qt.io/qt-5/qfile.html) and [QTextStream](https://doc.qt.io/qt-5/qtextstream.html).</br>
```saveContent function```
```cpp
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
```

```openContent function```
```cpp
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

```
For deleting tasks, we showed just the example of todayTask. </br>
``` DeleteTask ```
``` cpp
//Get the selected items in todayTasks
    QList<QListWidgetItem *> todayItems = ui->todayTask->selectedItems();

    //Remove the selected items
    if(!todayItems.isEmpty())
    {
        for(auto item : todayItems)
            ui->todayTask->takeItem(ui->todayTask->row(item));
    }
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
            
        }
    }

    file.close();

    //save content
    saveContent(this->filename);

```


## ModelBased Approach
To store an archieve of all tasks, we created a database with [QSqlDatabase](https://doc.qt.io/qt-5/qsqldatabase.html).

``` ConnectToDatabase ```
``` cpp
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
```

``` FillDataBase ```
``` cpp
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
```
For deleting tasks, we showed just the example of todayTask. </br>
``` DeleteTask ```
``` cpp
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
  }
```

Here is a video showing the functionality of the application </br>
https://user-images.githubusercontent.com/72691265/150692442-5d8b4500-7cd0-4480-8529-d4e2ec39b8ef.mp4



