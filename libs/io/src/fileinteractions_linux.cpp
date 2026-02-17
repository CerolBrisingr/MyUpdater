#include <io/fileinteractions.h>
#include <unistd.h>

namespace Updater2::IO {

    bool createProcess(const std::filesystem::path& executable, const stringList& args) {

        // Prepare full argument vector
        std::vector<std::string> sArgs;
        sArgs.push_back(executable.string());
        for (const auto& a : args) sArgs.push_back(a);

        std::vector<char*> cArgs;
        for (auto& s : sArgs) cArgs.push_back(const_cast<char*>(s.c_str()));
        cArgs.push_back(nullptr);

        // Extract path for visibility
        const char* path = cArgs[0];

        // Fork/exec, as slim as we can make it
        pid_t pid = fork();
        if (pid < 0) return false; // Something went wrong
        if (pid > 0) return true; // Parent process

        setsid(); // Own session for fork

        // Seperate standard-IO
        close(0); close(1); close(2);

        execv(path, cArgs.data());
        _exit(1);
    }

} // namespace Updater2::IO