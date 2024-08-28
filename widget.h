#ifndef WIDGET_H
#define WIDGET_H

#include <QGridLayout>
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QDebug>

#include "listener/listener.hpp"
#include "backend/runtime/runtime.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};



class FieldGui : public QWidget {
    Q_OBJECT

    QGridLayout m_grid;
    Cell prev;
    Cell exit;
    Listener m_listener;
public:

    FieldGui (const std::string &string, QWidget *main);


signals:
    void changed(Orient dir, Cell cur, Cell changed, int weight);

private slots:
    void update(Orient dir, Cell cur, Cell changed, int weight) {

        QFont font("Lucida Console", 20);

        if (changed != std::pair{0, 0}) {
            auto Changed =
                m_grid.itemAtPosition(changed.first, changed.second)->widget();
            ((QLabel*) Changed)->setFont(font);

            if (weight) {
                ((QLabel*) Changed)->setStyleSheet(QString("background-color: %1;").arg("orange"));
                ((QLabel*) Changed)->setText(QString("%1").arg(weight));
            }
            else {
                ((QLabel*) Changed)->setStyleSheet(QString("background-color: %1;").arg("yellow"));
                ((QLabel*) Changed)->setText("");
            }
            return;
        }

        auto Old = m_grid.itemAtPosition(prev.first, prev.second)->widget();

        ((QLabel*) Old)->setFont(font);

        switch (dir) {
            case Orient::NORTH:
                ((QLabel*) Old)->setText("⇑");
                break;
            case Orient::NORTH_EAST:
                ((QLabel*) Old)->setText("⇗");
                break;
            case Orient::SOUTH_EAST:
                ((QLabel*) Old)->setText("⇘");
                break;
            case Orient::SOUTH:
                ((QLabel*) Old)->setText("⇓");
                break;
            case Orient::SOUTH_WEST:
                ((QLabel*) Old)->setText("⇙");
                break;
            case Orient::NORTH_WEST:
                ((QLabel*) Old)->setText("⇖");
                break;
        }

        ((QLabel*) Old)->setAlignment(Qt::AlignCenter);

        auto New = m_grid.itemAtPosition(cur.first, cur.second)->widget();

        ((QLabel*) New)->setFont(font);
        ((QLabel*) New)->setText("R");
        ((QLabel*) New)->setAlignment(Qt::AlignCenter);
        prev = cur;
        if (prev == exit)
            ((QLabel*) New)->setText("W");
    }
};





#endif // WIDGET_H
