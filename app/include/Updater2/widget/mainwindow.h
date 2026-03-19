#pragma once

#include <QtWidgets>

namespace Updater2::ui::widget {

	inline int runMainwindow() {

        //QCoreApplication::addLibraryPath("F:/Qt/6.10.2/msvc2022_64/plugins");
        static int argc = 1;
        static char* argv[] = { (char*)"Updater2", nullptr };

		QApplication app(argc, argv);

        QWidget window;
        window.resize(320, 240);
        window.show();
        window.setWindowTitle(
            QApplication::translate("toplevel", "Top-level widget"));

		return app.exec();

	}

} // Updater2::ui::widget