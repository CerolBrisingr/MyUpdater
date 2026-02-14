#include <io/fileinteractions.h>
#include <windows.h>

namespace Updater2::IO {


    bool createProcess(const std::filesystem::path& executable, const stringList& args) {
        // 1. Kommandozeile für Windows bauen
        std::wstring cmd = L"\"" + executable.wstring() + L"\"";
        for (const auto& arg : args) {
            std::wstring wArg = std::filesystem::path(arg).wstring();
            if (wArg.find(L' ') != std::wstring::npos) wArg = L"\"" + wArg + L"\"";
            cmd += L" " + wArg;
        }

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };
        std::vector<wchar_t> buffer(cmd.begin(), cmd.end());
        buffer.push_back(0);

        if (CreateProcessW(NULL, buffer.data(), NULL, NULL, FALSE,
                           DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP,
                           NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return true; // Hauptprogramm läuft hier einfach weiter
                           }
        return false;
    }

    // TODO: remove
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