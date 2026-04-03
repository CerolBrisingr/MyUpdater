#pragma once

#include <Updater2/logic/interface.h>
#include <QtWidgets>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QObject>

#include <QSplitter>
#include <QScrollArea>
#include <memory>

namespace Updater2::ui::widget {

    void buildLayout(QWidget& window, Core::Interface& eventHandler);
    void buildSplitLayout(QWidget& window, Core::Interface& eventHandler);

	inline int runMainwindow(Core::Interface& eventHandler) {

        //QCoreApplication::addLibraryPath("F:/Qt/6.10.2/msvc2022_64/plugins");
        static int argc = 1;
        char program_name[] { "Updater2" };
        static char* argv[] = { program_name, nullptr};

		QApplication app(argc, argv);

        QWidget window;
        window.resize(500, 240);
        window.show();
        window.setWindowTitle(
            QApplication::translate("toplevel", "Top-level widget"));

        buildSplitLayout(window, eventHandler);

		return app.exec();

	}

    inline void buildSplitLayout(QWidget& window, Core::Interface& eventHandler) {
        auto mainFrame = new QHBoxLayout(&window);
        auto splitter = new QSplitter(Qt::Horizontal);
        mainFrame->addWidget(splitter);

        auto updaterList{ std::make_unique<QScrollArea>() };
        auto uiFrame{ std::make_unique<QWidget>() };
        auto fixedUI{ std::make_unique<QVBoxLayout>() };

        auto buttonAdd{ std::make_unique<QPushButton>("Add Item") };
        QObject::connect(buttonAdd.get(), &QPushButton::clicked,
            &eventHandler, &Core::Interface::clickButton1);

        auto buttonRemove{ std::make_unique<QPushButton>("Remove Item") };
        QObject::connect(buttonRemove.get(), &QPushButton::clicked,
            &eventHandler, &Core::Interface::clickButton2);

        splitter->addWidget(updaterList.release());

        fixedUI->addWidget(buttonAdd.release());
        fixedUI->addWidget(buttonRemove.release());
        fixedUI->addStretch();
        uiFrame->setLayout(fixedUI.release());
        splitter->addWidget(uiFrame.release());
    }

    inline void buildLayout(QWidget &window, Core::Interface& eventHandler) {
        auto verticalSplit{ new QHBoxLayout{} };

        auto updaterList{ new QVBoxLayout{} };
        auto fixedUI{ new QVBoxLayout{} };

        auto buttonAdd{ new QPushButton("Add Item") };
        QObject::connect(buttonAdd, &QPushButton::clicked,
            &eventHandler, &Core::Interface::clickButton1);

        auto buttonRemove{ new QPushButton("Remove Item") };
        QObject::connect(buttonRemove, &QPushButton::clicked,
            &eventHandler, &Core::Interface::clickButton2);

        window.setLayout(verticalSplit);
        verticalSplit->addLayout(updaterList, 4);
        verticalSplit->addLayout(fixedUI, 1);
        fixedUI->addWidget(buttonAdd);
        fixedUI->addWidget(buttonRemove);
        fixedUI->addStretch();
    }

} // Updater2::ui::widget