#pragma once

#include <Updater2/logic/interface.h>
#include <QtWidgets>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QObject>
#include <QTextEdit>

#include <QSplitter>
#include <QScrollArea>
#include <memory>

namespace Updater2::ui {

    class WidgetUI : public QObject{

        Q_OBJECT

    public:
        WidgetUI(Core::Interface& eventHandler) {
            static int argc = 1;
            char program_name[]{ "Updater2" };
            static char* argv[] = { program_name, nullptr };

            m_app = std::make_unique<QApplication>(argc, argv);

            m_window = std::make_unique<QWidget>();
            QWidget window;
            m_window->resize(500, 240);
            m_window->show();
            m_window->setWindowTitle(
                QApplication::translate("toplevel", "Top-level widget"));

            buildSplitLayout(eventHandler);
        }
        int run() {
            return m_app->exec();
        }

    public slots:
        void receiveMessage(const QString& message) {
            // Relay all messages through here for potential deactivation/changes
            m_textField->append(message);
        }

    private:
        std::unique_ptr<QApplication> m_app;
        std::unique_ptr<QWidget> m_window;
        QTextEdit* m_textField{ nullptr };

        void buildSplitLayout(Core::Interface& eventHandler) {
            auto mainFrame = new QHBoxLayout(m_window.get());
            auto splitter = new QSplitter(Qt::Horizontal);
            mainFrame->addWidget(splitter);

            auto updaterList{ std::make_unique<QScrollArea>() };
            auto uiFrame{ std::make_unique<QWidget>() };
            auto fixedUI{ std::make_unique<QVBoxLayout>() };

            auto buttonAdd{ std::make_unique<QPushButton>("Add Item") };
            QObject::connect(buttonAdd.get(), &QPushButton::clicked,
                &eventHandler, &Core::Interface::requestNewElement);

            auto buttonRemove{ std::make_unique<QPushButton>("Remove Item") };
            QObject::connect(buttonRemove.get(), &QPushButton::clicked,
                &eventHandler, &Core::Interface::clickButton2);

            auto debugLine{ std::make_unique<QTextEdit>("Debug Window") };
            m_textField = debugLine.get();
            debugLine->setReadOnly(true);
            QObject::connect(&eventHandler, &Core::Interface::addMessage,
                this, &WidgetUI::receiveMessage);

            splitter->addWidget(updaterList.release());

            fixedUI->addWidget(buttonAdd.release());
            fixedUI->addWidget(buttonRemove.release());
            fixedUI->addWidget(debugLine.release());
            fixedUI->addStretch();
            uiFrame->setLayout(fixedUI.release());
            splitter->addWidget(uiFrame.release());
        }
    };

} // Updater2::ui