#include "widget.h"
#include "ui_widget.h"
#include <fstream>
#include <sstream>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

FieldGui::FieldGui (const std::string &string, QWidget *main) :
    m_grid(QGridLayout()), QWidget(main) {

    std::ifstream fs(string);
    if (!fs.is_open()) {
        throw std::runtime_error("Error: Unable to open the file.\n");
    }

    m_grid.setSpacing(0);
    QVector<QLabel*> labels;

    std::string token_str;
    int cur_cell_idx = 0;
    int depth = 0;
    int width = 1;

    while (std::getline(fs, token_str)) {
        std::istringstream strstream(token_str);
        std::string cell_str;

        while (std::getline(strstream, cell_str, ' ')) {

            labels.push_back(new QLabel());
            QFont font("Lucida Console", 40);

            switch (cell_str[0]) {
                case 'R':
                    labels[cur_cell_idx]->setStyleSheet(QString("background-color: %1;").arg("yellow"));
                    labels[cur_cell_idx]->setText("R");
                    labels[cur_cell_idx]->setAlignment(Qt::AlignCenter);
                    labels[cur_cell_idx]->setFont(font);
                    prev = Cell(depth, width);
                    break;
                case 'B':
                    labels[cur_cell_idx]->setStyleSheet(QString("background-color: %1;").arg("orange"));
                    labels[cur_cell_idx]->setText(QString("%1").arg(cell_str.c_str()+1));
                    labels[cur_cell_idx]->setAlignment(Qt::AlignCenter);
                    labels[cur_cell_idx]->setFont(font);
                    break;
                case 'Q':
                    labels[cur_cell_idx]->setStyleSheet(QString("background-color: %1;").arg("green"));
                    exit = Cell(depth, width);
                    break;
                case 'E':
                    labels[cur_cell_idx]->setStyleSheet(QString("background-color: %1;").arg("yellow"));
                    break;
                case 'W':
                    labels[cur_cell_idx]->setStyleSheet(QString("background-color: %1;").arg("blue"));
                    break;
                default:
                    break;
            }

            m_grid.addWidget(labels[cur_cell_idx], depth, width);

            width+=2;
            cur_cell_idx++;
        }
        depth++;
        width = (width%2) ? 0 : 1;
    }

    main->setLayout(&m_grid);
    connect(this, &FieldGui::changed, this, &FieldGui::update);

    auto* m_listener = new Listener(this);
    m_listener->openServer("sock.sock");
    connect(m_listener, SIGNAL(newStateReceived(Orient, Cell)), this, SLOT(update(Orient, Cell)));
    fs.close();
}

