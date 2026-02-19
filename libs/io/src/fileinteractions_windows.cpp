#include <io/fileinteractions.h>
#include <windows.h>

namespace Updater2::IO {

    namespace {
        std::wstring getQuotedArgument(const std::string& arg) {
            // We fully expect the string to be encoded with utf-8
            std::u8string u8Arg(arg.begin(), arg.end());
            // use string translation in std::filesystem::path
            std::wstring wArg = std::filesystem::path(u8Arg).wstring();
            if (wArg.find(L'"') == std::wstring::npos) {
                // Argument contains no quotes, add some
                wArg = L"\"" + wArg + L"\"";
            }
            return wArg;
        }
    } // namespace


    bool createProcess(const std::filesystem::path& executable, const stringList& args) {
        // Build command line ("executable" "arg1" "arg2" ...)
        std::wstring cmd = L"\"" + executable.wstring() + L"\"";
        for (const auto& arg : args) {
            cmd += L" " + getQuotedArgument(arg);
        }

        STARTUPINFOW si = { 
            .cb = sizeof(si),
            .dwFlags = STARTF_USESHOWWINDOW,
            .wShowWindow = SW_SHOWDEFAULT
        };
        PROCESS_INFORMATION pi = { 0 };

        // Adjust format for CreateProcessW (needs to be able to modify content)
        std::vector<wchar_t> buffer(cmd.begin(), cmd.end());
        buffer.push_back(0);

        std::wstring workingDir = executable.parent_path().wstring();

        if (CreateProcessW(
            NULL,                   // No module name (use command line)
            buffer.data(),          // Command line
            NULL,                   // Process handle not inheritable
            NULL,                   // Thread handle not inheritable
            FALSE,                  // Set handle inheritance to FALSE
            DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP, // Creation flags
            NULL,                   // Use parent's environment block
            workingDir.c_str(),     // Use target's starting directory 
            &si,                    // Pointer to STARTUPINFO structure
            &pi                     // Pointer to PROCESS_INFORMATION structure
        )) {
            // Close process and thread handles. Don't leak process control.
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return true;
                           }
        else {
            // GetLastError() ...
            return false;
        }
    }

} // namespace Updater2::IO