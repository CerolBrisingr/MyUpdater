#include <io/fileinteractions.h>
#include <unistd.h>

namespace Updater2::IO {

    bool createProcess(const std::filesystem::path& executable, const stringList& args) {

        // Argumente vorbereiten
        std::vector<std::string> sArgs;
        sArgs.push_back(executable.string());
        for (const auto& a : args) sArgs.push_back(a);

        std::vector<char*> cArgs;
        for (auto& s : sArgs) cArgs.push_back(const_cast<char*>(s.c_str()));
        cArgs.push_back(nullptr);

        // Pfad extrahieren, bevor wir forken
        const char* path = cArgs[0];

        // Linux: Klassisch fork/exec
        pid_t pid = fork();
        if (pid < 0) return false;
        if (pid > 0) return true; // Elternprozess

        setsid();

        // Standard-IO trennen
        close(0); close(1); close(2);

        execv(path, cArgs.data());
        _exit(1);
    }

} // namespace Updater2::IO