#include "Updater2/updater/entry.h"

#include <QObject>

namespace Updater2 {
	namespace {

	} // namespace

	class Manager : public QObject {
		Q_OBJECT

	public slots:
		void entryRequest();

	signals:
		void entryCreated(Entry& data);

	public:

	private:

	};

} // namespace Updater2