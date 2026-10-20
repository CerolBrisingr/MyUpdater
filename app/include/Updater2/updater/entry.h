#pragma once
#include <QObject>
#include <string>

namespace Updater2 {
	namespace {

	} // namespace

	class State {
	public:
		explicit State(int id);
		State(const State& rhs) = default;

		[[nodiscard]] std::string getMessage() const;
	private:
		int m_id;
		bool m_toggled{false};
	};

	class Entry : public QObject {
		Q_OBJECT

	signals:
		void stateChanged();

	public:
		explicit Entry(int id);
		Entry(const Entry&);
		Entry(Entry&&) noexcept;
		Entry& operator=(const Entry&);
		Entry& operator=(Entry&&) noexcept;

		std::string getName() const;
		const State& getState() const;

	private:
		State m_state;
		int m_id;

	};

} // namespace Updater2
