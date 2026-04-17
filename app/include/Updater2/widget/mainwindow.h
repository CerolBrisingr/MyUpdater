#pragma once

#include <Updater2/logic/interface.h>
#include <QtWidgets>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QObject>
#include <QTextEdit>
#include <vector>

#include <QSplitter>
#include <QScrollArea>
#include <memory>
#include <atomic>

namespace Updater2::ui {

    class ListEntry : public QObject {

        Q_OBJECT

    public:
        ListEntry(QVBoxLayout& list, const QString& text) {
            auto entryFrame{ std::make_unique<QWidget>() };
            auto layout{ std::make_unique<QVBoxLayout>() };
            auto button{ std::make_unique<QPushButton>(text) };

            layout->addWidget(button.release());
            entryFrame->setLayout(layout.release());
            list.addWidget(entryFrame.release());
        }
    };

    class WidgetUI : public QObject{

        Q_OBJECT

    public:
        WidgetUI(Core::Interface& eventHandler) {
            static int argc = 1;
            char program_name[]{ "Updater2" };
            static char* argv[] = { program_name, nullptr };

            m_app = std::make_unique<QApplication>(argc, argv);

            m_window = std::make_unique<QWidget>();
            m_window->resize(500, 240);
            m_window->show();
            m_window->setWindowTitle(
                QApplication::translate("toplevel", "Top-level widget"));

            buildSplitLayout(eventHandler);
        }

        int run() {
            if (m_executed.exchange(true)) {
                // Initially false but was true -> ran already
                return -1;
            }
            // Initially false and was still false - run this once
            return m_app->exec();
        }

    public slots:
        void displayMessage(const QString& message) {
            // Relay all messages through here for potential deactivation/changes
            m_textField->append(message);
        }
        void addNewField(const QString& title) {
            displayMessage("Trying to add new field");
            if (m_updaterList) {
                displayMessage("About to add new field");
                m_updaters.push_back(std::make_unique<ListEntry>(*m_updaterList, title));
                displayMessage("Now, where is it?");
            }
        }

    private:
        std::atomic<bool> m_executed{ false };  // Making sure to run this interface only once
        std::unique_ptr<QApplication> m_app;
        std::unique_ptr<QWidget> m_window;
        QVBoxLayout* m_updaterList{ nullptr };
        QTextEdit* m_textField{ nullptr };
        std::vector<std::unique_ptr<ListEntry>> m_updaters{};

        void buildSplitLayout(Core::Interface& eventHandler) {
            auto mainFrame = new QHBoxLayout(m_window.get());
            auto splitter = new QSplitter(Qt::Horizontal);
            mainFrame->addWidget(splitter);

            auto updaterScrollField{ std::make_unique<QScrollArea>() };
            auto updaterListContainer{ std::make_unique<QWidget>() };
            auto updaterList{ std::make_unique<QVBoxLayout>() };
            auto uiFrame{ std::make_unique<QWidget>() };
            auto fixedUI{ std::make_unique<QVBoxLayout>() };

            auto buttonAdd{ std::make_unique<QPushButton>("Add Item") };
            QObject::connect(buttonAdd.get(), &QPushButton::clicked,
                &eventHandler, &Core::Interface::requestNewElement);
            QObject::connect(&eventHandler, &Core::Interface::newElement,
                this, &WidgetUI::addNewField);

            auto buttonRemove{ std::make_unique<QPushButton>("Remove Item") };
            QObject::connect(buttonRemove.get(), &QPushButton::clicked,
                &eventHandler, &Core::Interface::clickButton2);

            auto debugLine{ std::make_unique<QTextEdit>("Debug Window") };
            m_textField = debugLine.get();
            debugLine->setReadOnly(true);
            QObject::connect(&eventHandler, &Core::Interface::addMessage,
                this, &WidgetUI::displayMessage);

            // Scrollable updater List
            m_updaterList = updaterList.get();
            m_updaterList->setAlignment(Qt::AlignTop);  // Don't strech individual list entries
            updaterListContainer->setLayout(updaterList.release());
            updaterListContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);  // Adjust size of list as needed
            updaterScrollField->setWidget(updaterListContainer.release());
            updaterScrollField->setWidgetResizable(true);   // Allow size adjustments for list
            splitter->addWidget(updaterScrollField.release());

            // Buttons and text field to the side
            fixedUI->addWidget(buttonAdd.release());
            fixedUI->addWidget(buttonRemove.release());
            fixedUI->addWidget(debugLine.release());
            fixedUI->addStretch();
            uiFrame->setLayout(fixedUI.release());
            splitter->addWidget(uiFrame.release());
        }
    };

} // Updater2::ui