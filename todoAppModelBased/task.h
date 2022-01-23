#ifndef TASK_H
#define TASK_H

#include <QDialog>

namespace Ui {
class Task;
}

class Task : public QDialog
{
    Q_OBJECT

public:
    explicit Task(QWidget *parent = nullptr);
    ~Task();

    //Getters
    QString getDesc(); //Get the description
    QString getStatus(); //Get the status of the task, finished or not
    QString getTag(); //Get the tag, Work, Life, Other
    QDate getDate(); //Get the date

    //Setters
    void setDesc(QString);
    void setStatus(QString);
    void setTag(QString);
    void setDate(QString date);

private:
    Ui::Task *ui;
};

#endif // TASK_H
