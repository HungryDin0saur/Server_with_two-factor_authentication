#include <QtWidgets>
#include "MyServer.h"
#define DPy_BUILD_CORE_BUILTIN
#include <Python.h>

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MyServer     server(2323);

    server.show();

    return app.exec();
}
