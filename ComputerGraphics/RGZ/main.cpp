#include <QApplication>
#include <QPushButton>
#include <QGraphicsView>
#include <QtDebug>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QRandomGenerator>
#include <QDateTime>
#include <QLayout>
#include <QLabel>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QThread>
#include <QTimer>
#include <QSpinBox>
#include <QObject>

// Создаём объекты
QWidget             *inputWindow;
QWidget             *graphicsWindow;
QHBoxLayout         *inputLayout;
QHBoxLayout         *graphicsLayout;
QLabel              *inputLabel;
QSpinBox            *inputSpinBox;
QPushButton         *inputEnterButton;
QGraphicsScene      *scene;
QPen                *pen;
QBrush              *brush;
QGraphicsView       *view;

// Создание произвольного цвета
QColor randColor() {
    QRandomGenerator    *red = new QRandomGenerator,
                        *green = new QRandomGenerator,
                        *blue = new QRandomGenerator;
    red->seed(QDateTime::currentMSecsSinceEpoch());
    green->seed(QDateTime::currentMSecsSinceEpoch() % 7 * 10);
    blue->seed(QDateTime::currentMSecsSinceEpoch() % 21 * 5);
    QColor color(QRandomGenerator::global()->bounded(255), QRandomGenerator::global()->bounded(255), QRandomGenerator::global()->bounded(255));
    //qDebug() << "Red value is " << color.red() << "\nBlue value is " << color.blue();
    delete red; delete green; delete blue;
    return color;
}

void draw()
{
    graphicsWindow->show();
    brush->setColor(randColor());
    scene->setSceneRect(0, 0, view->width() - 5, view->height() - 5);
    scene->addRect(0, 0, scene->width(), scene->height(), *pen, *brush);

    int minX = 1, minY = 1, dif, lastDif = 100000;
    float generalAttitude = (float)scene->width() / (float)scene->height(),
          attitude, lastAttitude = 0;

    qDebug() << "Global attitude is " << generalAttitude;

    for (int x = 1; x < inputSpinBox->value() + 1; x++) {
        for (int y = 1; y < inputSpinBox->value() + 1; y++) {
            dif = x * y - (inputSpinBox->value() - 1);
            if (dif < 0) continue;
            if (dif <= lastDif || dif < (int)sqrt(inputSpinBox->value() - 1)) {
                attitude = (float)x / (float)y;
                if (abs(generalAttitude - attitude) < abs(generalAttitude - lastAttitude)) {
                    minX = x;
                    minY = y;
                    lastAttitude = attitude;
                }
                qDebug() << "Found grid (" << x << ", " << y << ") " << attitude;
                lastDif = dif;
            }
        }
    }

    int marginX = scene->width() / minX / 20,
        marginY = scene->height() / minY / 20;

    qDebug() << "The best grid is (" << minX << ", " << minY << ")";

    for (int i = 0; i < (inputSpinBox->value() - 1); i++) {
        int x = i % minX;
        int y = i / minX;
        brush->setColor(randColor());
        scene->addRect(marginX + x * scene->width() / minX, marginY + y * scene->height() / minY,\
                       scene->width() / minX - 2 * marginX, scene->height() / minY - 2 * marginY, *pen, *brush);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаём объекты
    inputWindow         = new QWidget;
    graphicsWindow      = new QWidget;
    inputLayout         = new QHBoxLayout;
    graphicsLayout      = new QHBoxLayout;
    inputLabel          = new QLabel("The number of rectangles ");
    inputSpinBox        = new QSpinBox;
    inputEnterButton    = new QPushButton("Draw");
    scene               = new QGraphicsScene(0, 0, 400, 400);
    view                = new QGraphicsView(scene);
    pen                 = new QPen;
    brush               = new QBrush;

    // Применяем настройки к объектам
    inputSpinBox->setMinimum(1);

    // Добавляем виджеты на окно
    inputLayout->setMargin(20); // Убираем отступы
    graphicsLayout->setMargin(0); // Убираем отступы
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputSpinBox);
    inputLayout->addWidget(inputEnterButton);
    graphicsLayout->addWidget(view);
    inputWindow->setLayout(inputLayout);
    graphicsWindow->setLayout(graphicsLayout);
    pen->setWidth(1);
    brush->setStyle(Qt::SolidPattern);

    // Подключаем слоты
    QObject::connect(inputEnterButton, &QPushButton::clicked, draw);

    inputWindow->show();
    return a.exec();
}
