#include <QApplication>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLayout>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QThread>
#include <QTimer>
#include <QObject>

// Создаём объекты
QWidget             *window;
QGraphicsScene      *scene;
QGraphicsView       *view;
QVBoxLayout         *layout;
QPen                *pen;
QPen                *pen2;
QTimer              *timer;

int size, smileEyes,
    smileX, smileY,
    smileEyeX, smileEyeY,
    smileLeftEyeCenter, smileRightEyeCenter,
    smileMouthWidth, smileMouthHeigth;

void nextFrame() {
    static int frame = 0;
    scene->clear();
    scene->addEllipse(smileX, smileY, size, size, QPen(Qt::yellow), QBrush(Qt::yellow));
    switch(frame) {
        case 0:
            //Add happy mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 32 * 19, smileMouthWidth, smileMouthHeigth, *pen2);
            scene->addRect(smileX + (size - smileMouthWidth - pen2->width()) / 2, smileY + size / 32 * 19 - pen2->width(),
                           smileMouthWidth + pen2->width() * 2, smileMouthHeigth / 2 + pen2->width(), QPen(Qt::yellow), QBrush(Qt::yellow));
            //Add circle eyes
            scene->addEllipse(smileLeftEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            scene->addEllipse(smileRightEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            break;
        case 1:
            //Add happy mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 32 * 19, smileMouthWidth, smileMouthHeigth, *pen2);
            scene->addRect(smileX + (size - smileMouthWidth - pen2->width()) / 2, smileY + size / 32 * 19 - pen2->width(),
                           smileMouthWidth + pen2->width() * 2, smileMouthHeigth / 2 + pen2->width(), QPen(Qt::yellow), QBrush(Qt::yellow));
            //Add left rect eye and right circle eye
            scene->addRect(smileLeftEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            scene->addEllipse(smileRightEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            break;
        case 2:
            //Add happy mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 32 * 19, smileMouthWidth, smileMouthHeigth, *pen2);
            scene->addRect(smileX + (size - smileMouthWidth - pen2->width()) / 2, smileY + size / 32 * 19 - pen2->width(),
                           smileMouthWidth + pen2->width() * 2, smileMouthHeigth / 2 + pen2->width(), QPen(Qt::yellow), QBrush(Qt::yellow));
            //Add left circle eye and right rect eye
            scene->addEllipse(smileLeftEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            scene->addRect(smileRightEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            break;
        case 3:
            //Add happy mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 32 * 19, smileMouthWidth, smileMouthHeigth, *pen2);
            scene->addRect(smileX + (size - smileMouthWidth - pen2->width()) / 2, smileY + size / 32 * 19 - pen2->width(),
                           smileMouthWidth + pen2->width() * 2, smileMouthHeigth / 2 + pen2->width(), QPen(Qt::yellow), QBrush(Qt::yellow));
            //Add rect eyes
            scene->addRect(smileLeftEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            scene->addRect(smileRightEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            break;
        case 4:
            //Add circle mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 8 * 5, smileMouthWidth, smileMouthHeigth, *pen2);
            //Add rect eyes
            scene->addRect(smileLeftEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            scene->addRect(smileRightEyeCenter - smileEyeX / 2, smileY + size / 3, smileEyeX, pen->width(), *pen);
            break;
        case 5:
            //Add circle mouth
            scene->addEllipse(smileX + (size - smileMouthWidth) / 2, smileY + size / 8 * 5, smileMouthWidth, smileMouthHeigth, *pen2);
            //Add circle eyes
            scene->addEllipse(smileLeftEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            scene->addEllipse(smileRightEyeCenter - smileEyeX / 2, smileY + (size - smileEyeY) / 3, smileEyeX, smileEyeY, *pen);
            break;
    }
    scene->addText("Frame: " + QString::number(frame));
    if (frame >= 5) frame = 0;
    else frame++;
    scene->update(0, 0, view->width(), view->height());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаём объекты
    window = new QWidget;
    scene  = new QGraphicsScene(0, 0, 1200, 1000);
    view   = new QGraphicsView(scene);
    layout = new QVBoxLayout;
    pen    = new QPen();
    pen2   = new QPen();
    timer  = new QTimer();

    layout->setMargin(0); // Убираем отступы
    layout->addWidget(view);
    window->setLayout(layout);
    timer->setInterval(200);
    // Подключаем таймер к функции nextFrame()
    QObject::connect(timer, &QTimer::timeout, nextFrame);

    if (scene->width() > scene->height())
        size = scene->height();
    else
        size = scene->width();

    pen->setColor(Qt::darkBlue);
    pen->setWidth(size / 40);
    pen2->setColor(Qt::darkRed);
    pen2->setWidth(size / 40);

    smileX = (scene->width() - size) / 2,
    smileY = (scene->height() - size) / 2,
    smileEyeX = size / 4,
    smileEyeY = size / 4,
    smileEyes = size / 8,
    smileLeftEyeCenter = smileX  + size / 2 - smileEyeX / 2 - smileEyes / 2,
    smileRightEyeCenter = smileX  + size / 2 + smileEyeX / 2 + smileEyes / 2,
    smileMouthWidth = size / 2,
    smileMouthHeigth = size / 4;

    window->show();
    timer->start();
    return a.exec();
}
