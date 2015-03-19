#include <QtGui>
#include <QGraphicsScene>
#include <QScopedPointer>


#include "mainview.h"
#include "maincontainer.h"
#include "system.h"

int main(int argc, char *argv[])
{
    
#ifdef Q_OS_SYMBIAN
    QApplication::setAttribute (Qt::AA_S60DontConstructApplicationPanes);
#endif

    
    QApplication a(argc, argv);


    QGraphicsScene scene;
    MainContainer* container = new MainContainer;
    MainView  view (*container) ;

    view.setScene(&scene);

    QObject::connect(QApplication::desktop(), SIGNAL(resized(int)), &view, SLOT(resized(int)));

    view.adjustScreenSize();

    scene.addItem(container); // scene now owns the container
    System::setViewMode(&view, System::PortraitMode);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    view.showFullScreen();
#else
    view.show();
#endif
    return a.exec();
}
