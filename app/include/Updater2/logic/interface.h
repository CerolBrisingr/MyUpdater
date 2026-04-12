#pragma once

#include <QObject>
#include <iostream>

#include <QDebug>

namespace Updater2::Core {

	class Interface : public QObject {
		Q_OBJECT

	public:
		Interface() {};
		~Interface() {};

		Interface(Interface& rhs) = delete;
		Interface(Interface&& rhs) = delete;

		Interface& operator=(Interface& rhs) = delete;
		Interface& operator=(Interface&& rhs) = delete;

	public slots:
		void clickButton1(bool) {
			emit addMessage("Button 1");
		}
		void clickButton2() {
			emit addMessage("Button 2");
		}

	signals:
		void addMessage(const QString& message = "");
	};


} // namespace Updater2::Core