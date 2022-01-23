#ifndef TODOAPP_H
#define TODOAPP_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlQuery>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class todoApp; }
QT_END_NAMESPACE

class todoApp : public QMainWindow
{
    Q_OBJECT

public:
    todoApp(QWidget *parent = nullptr);
    ~todoApp();

    void connectDB(); //Se connecter à la base de données
    void addEnty(QString, QDate, QString, QString); //Remplir la base de données
    void showDB(); //Afficher le contenu de la base de données

private slots:
    void on_actionNew_Task_triggered();

    void on_actionDelete_Tasks_triggered();

    void on_action_Exit_triggered();

    void on_actionToday_Task_triggered();

    void on_actionPending_Task_triggered();

    void on_actionFinished_Tasks_triggered();

    void editTaskSlot();

private:
    Ui::todoApp *ui;
    QSqlDatabase db;
    QSqlQueryModel *todayTaskModel;
    QSqlQueryModel *pendingTaskModel;
    QSqlQueryModel *finishedTaskModel;
    QString filename = QDir::homePath()+ "/tasks.sqlite";
};
#endif // TODOAPP_H
