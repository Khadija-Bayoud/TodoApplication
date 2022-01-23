#ifndef TODOAPP_H
#define TODOAPP_H

#include <QMainWindow>
#include <QListWidget>
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
    void saveContent(QString filename);
    void openContent(QString filename);

private slots:
    void on_exit_triggered();

    void on_todayTaskAct_triggered();

    void on_pendingTaskAct_triggered();

    void on_finishedTaskAct_triggered();

    void on_newTask_triggered();

    void on_Save_triggered();

    void on_Open_triggered();

    void editTaskSlot();

    void on_actionDelete_Task_triggered();


private:
    Ui::todoApp *ui;

    //specify the dir of the file
    QString filename = QDir::homePath() + "/tasks.txt";;

};
#endif // TODOAPP_H
