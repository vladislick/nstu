#include <QApplication>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLayout>
#include <QBrush>
#include <QPen>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаём объекты
    QWidget             *window = new QWidget;
    QGraphicsScene      *scene  = new QGraphicsScene(0, 0, 500, 400);
    QGraphicsView       *view   = new QGraphicsView(scene);
    QVBoxLayout         *layout = new QVBoxLayout;
    QFont               *font   = new QFont();
    QGraphicsTextItem   *label  = new QGraphicsTextItem;

    // Делаем предварительную настройку
    font->setPointSize(24);
    label->setPlainText("Франция");
    label->setFont(*font);
    // Размещаем надпись внизу по центру
    label->setPos((scene->width() - label->boundingRect().width()) / 2,
                   scene->height() - label->boundingRect().height());
    layout->setMargin(0); // Убираем отступы
    layout->addWidget(view);
    // Высота графической сцены минус высота надписи
    int height = scene->height() - label->boundingRect().height();

    //Рисуем все объекты в окне
    scene->addRect(0, 0, scene->width() / 3, height, QPen(), QBrush(Qt::darkBlue));
    scene->addRect(scene->width() / 3, 0, scene->width() / 3, height, QPen(), QBrush(Qt::white));
    scene->addRect(scene->width() / 3 * 2, 0, scene->width() / 3, height, QPen(), QBrush(Qt::darkRed));
    scene->addItem(label);

    window->setLayout(layout);
    window->show();
    return a.exec();
}
