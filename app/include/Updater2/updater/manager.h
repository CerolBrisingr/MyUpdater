#pragma once
#include "Updater2/updater/entry.h"

#include <QObject>

#include <vector>

namespace Updater2 {
	namespace {

	} // namespace

	class Manager : public QObject {
		Q_OBJECT

	public slots:
		void entryRequest();

	signals:
		void entryCreated(Updater2::Entry data);

	public:
		Manager();

	private:
		int m_counter {-1};
		std::vector<Entry> m_entries {};

	};

} // namespace Updater2