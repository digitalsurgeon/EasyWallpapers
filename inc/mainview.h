#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

class MainContainer;
class MainView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainView(MainContainer&);
    void adjustScreenSize();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void resized(int screen);

private:
    MainContainer&  iContainer;

};

#endif // MAINVIEW_H
