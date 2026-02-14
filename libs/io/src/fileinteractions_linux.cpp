#include <io/fileinteractions.h>
#include <unistd.h>

namespace Updater2::IO {

    bool createProcess(const std::filesystem::path& executable, const stringList& args) {

        // Linux: Klassisch fork/exec
        pid_t pid = fork();
        if (pid < 0) return false;
        if (pid > 0) return true; // Elternprozess

        setsid();

        // Argumente vorbereiten
        std::vector<std::string> sArgs;
        sArgs.push_back(executable.string());
        for (const auto& a : args) sArgs.push_back(a);

        std::vector<char*> cArgs;
        for (auto& s : sArgs) cArgs.push_back(const_cast<char*>(s.c_str()));
        cArgs.push_back(nullptr);

        // Standard-IO trennen
        close(0); close(1); close(2);

        execv(cArgs[0], cArgs.data());
        _exit(1);

        return true;
    }

} // namespace Updater2::IO