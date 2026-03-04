#include <io/fileinteractions.h>
#include <unistd.h>

namespace Updater2::IO {

    bool createProcessWaiting(const std::filesystem::path& executable, const char* path, std::vector<char*>& cArgs) 
    {
        // Fork/exec, as slim as we can make it
        pid_t pid = fork();
        if (pid < 0) return false; // Something went wrong
        if (pid > 0) {
            // Parent process
            int status{};
            if (waitpid(pid, &status, 0) == -1) return false;
            return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
        }
        // Child

        // Make sure relative paths are found
        std::error_code ec;
        std::filesystem::current_path(executable.parent_path(), ec);

        // Detach
        setsid(); // Own session for fork
        close(0); close(1); close(2); // Seperate standard-IO

        execvp(path, cArgs.data());

        _exit(1); // Only reached in case of error
    }

    bool createProcessNotWaiting(const std::filesystem::path& executable, const char* path, std::vector<char*>& cArgs)
    {
        // Fork/exec, as slim as we can make it
        pid_t pid = fork();
        if (pid == 0) { 
            // Child 1
            pid_t grandchild = fork();
            if (grandchild > 0) {
                // Child 1
                _exit(0); // terminates, collected by parent
            }
            if (grandchild == 0) {
                // grandchild (child 2)

                // Make sure relative paths are found
                std::error_code ec;
                std::filesystem::current_path(executable.parent_path(), ec);

                // Run and detach
                setsid();
                close(0); close(1); close(2); // Seperate standard-IO
                execvp(path, cArgs.data());
                _exit(1);
            }
            // Child 1 but second fork failed. Error
            _exit(1);
        }
        else if(pid < 0) {
            return false;
        }
        else { 
            // Parent
            int status{};
            waitpid(pid, &status, 0); // Child 1 will not end up as zombie
            return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
        }
    }

    bool createProcess(const std::filesystem::path& executable, const stringList& args, bool waiting) 
    {

        // Prepare full argument vector
        std::vector<std::string> sArgs;
        sArgs.push_back(executable.string());
        for (const auto& a : args) sArgs.push_back(a);

        std::vector<char*> cArgs;
        for (auto& s : sArgs) cArgs.push_back(const_cast<char*>(s.c_str()));
        cArgs.push_back(nullptr);

        // Extract path for visibility
        const char* path = cArgs[0];

        if (waiting) {
            return createProcessWaiting(executable, path, cArgs);
        }
        else {
            return createProcessNotWaiting(executable, path, cArgs);
        }
    }

} // namespace Updater2::IO