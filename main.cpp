#include "widget.h"
#include <QDebug>
#include <QApplication>
#include <string>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget main;

    FieldGui gui("/home/cracky/MakeRzhakaGreatAgain/backend/rt.field", &main);

    main.resize(600, 600);
    main.show();

    return app.exec();
}

