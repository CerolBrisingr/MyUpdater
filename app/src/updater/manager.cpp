#include "Updater2/updater/manager.h"

namespace Updater2
{
    Manager::Manager()
    {

    }

    void Manager::entryRequest()
    {
        Entry& newEntry = m_entries.emplace_back(++m_counter);
        emit entryCreated(newEntry);
    }

} // namespace Updater2