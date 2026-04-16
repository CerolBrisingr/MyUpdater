#pragma once

#include <QObject>
#include <iostream>

#include <atomic>
#include <QDebug>
#include <QString>

namespace Updater2::Core {

	class Holding {
	public:
		QString getName() {
			return QStringLiteral("Field No") + QString::number(current_id.fetch_add(1));
		}
	private:
		std::atomic<int> current_id{ 0 };
	};

	class Interface : public QObject {
		Q_OBJECT

	public:
		Interface() {};
		~Interface() {};

		Interface(Interface& rhs) = delete;
		Interface(Interface&& rhs) = delete;

		Interface& operator=(Interface& rhs) = delete;
		Interface& operator=(Interface&& rhs) = delete;

		void emitMessage(const QString& message) { emit addMessage(message); };

	public slots:
		void clickButton1(bool) {
			emit addMessage("Button 1");
		}
		void clickButton2() {
			emit addMessage("Button 2");
		}
		void requestNewElement() {
			emit addMessage("New element requested");
			emit newElement(elementGenerator.getName());
			emit addMessage("New element emitted");
		}

	signals:
		void addMessage(const QString& message = "");
		void newElement(const QString& name);

	private:
		Holding elementGenerator{};
	};


} // namespace Updater2::Core