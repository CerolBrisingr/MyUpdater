#include <QObject>

namespace Updater2 {
	namespace {

	} // namespace

	class State {

	};

	class Entry : public QObject {
		Q_OBJECT

	signals:
		void stateChanged();

	public:
		State& getState();

	private:

	};

} // namespace Updater2