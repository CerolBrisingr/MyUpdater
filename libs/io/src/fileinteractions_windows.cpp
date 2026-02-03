#include <io/fileinteractions.h>
#include <windows.h>

namespace Updater2::IO {

    bool createProcess(const std::filesystem::path& processPath, const std::wstring& commandLineArgs) {

        STARTUPINFOW si{ sizeof(si) };
        PROCESS_INFORMATION pi{ 0 };

        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWDEFAULT;

        // To avoid errors: set binary path in quotes
        std::wstring fullCmd = L"\"" + processPath.wstring() + L"\"";
        // Add command line arguments
        if (!commandLineArgs.empty()) {
            fullCmd += L" " + commandLineArgs;
        }

        std::wstring workingDir = processPath.parent_path().wstring();

        // Start the child process. 
        if (!CreateProcessW(NULL,   // No module name (use command line)
            fullCmd.data(),         // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            DETACHED_PROCESS,       // Creation flags
            NULL,           // Use parent's environment block
            workingDir.c_str(),     // Use target's starting directory 
            &si,     // Pointer to STARTUPINFO structure
            &pi)     // Pointer to PROCESS_INFORMATION structure
            )
        {
            return false;
        }

        // Close process and thread handles. Don't leak process control.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return true;
    }

} // namespace Updater2::IO