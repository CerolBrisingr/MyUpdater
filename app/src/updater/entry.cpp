#include "Updater2/updater/entry.h"
#include <format>

namespace Updater2
{
    State::State(int id)
        :m_id{id}
    {}

    std::string State::getMessage() const
    {
        return std::format("Button {} state message", this->m_id);
    }

    Entry::Entry(const int id)
        :m_state{id}, m_id{id}
    {}

    Entry::Entry(const Entry& rhs)
    :m_state{rhs.m_state}, m_id{rhs.m_id}
    {}

    Entry::Entry(Entry&& rhs) noexcept
    {}


    std::string Entry::getName() const
    {
        return std::format("Button {}", this->m_id);
    }

    const State& Entry::getState() const
    {
        return this->m_state;
    }
} // namespace Updater2
