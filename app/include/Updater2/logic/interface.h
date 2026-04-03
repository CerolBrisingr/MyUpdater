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
			qDebug() << "Clicked Button 1\n";
		}
		void clickButton2() {
			qDebug() << "Clicked Button 2\n";
		}
	};

} // namespace Updater2::Core