/*  The MIT License (MIT)
    ============================================================================

    ██████╗ ███████╗███████╗██████╗ ███████╗ ██████╗ ██████╗  ██████╗ ███████╗
    ██╔══██╗██╔════╝██╔════╝██╔══██╗██╔════╝██╔═══██╗██╔══██╗██╔════╝ ██╔════╝
    ██║  ██║█████╗  █████╗  ██████╔╝█████╗  ██║   ██║██████╔╝██║  ███╗█████╗
    ██║  ██║██╔══╝  ██╔══╝  ██╔═══╝ ██╔══╝  ██║   ██║██╔══██╗██║   ██║██╔══╝
    ██████╔╝███████╗███████╗██║     ██║     ╚██████╔╝██║  ██║╚██████╔╝███████╗
    ╚═════╝ ╚══════╝╚══════╝╚═╝     ╚═╝      ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝

    ████████╗ ██████╗  ██████╗ ██╗     ███████╗███████╗████████╗
    ╚══██╔══╝██╔═══██╗██╔═══██╗██║     ██╔════╝██╔════╝╚══██╔══╝
       ██║   ██║   ██║██║   ██║██║     ███████╗█████╗     ██║
       ██║   ██║   ██║██║   ██║██║     ╚════██║██╔══╝     ██║
       ██║   ╚██████╔╝╚██████╔╝███████╗███████║███████╗   ██║
       ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝   ╚═╝

    ============================================================================
    Copyright (c) 2023 DeepForge Technology
    ============================================================================
    Organization: DeepForge Technology
    ============================================================================
    Author: Blackflame576
    ============================================================================
    Created: 4 Juny 2023
    ============================================================================
*/
#include <filesystem>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "Progressbar.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../DatabaseConnect.cpp"
#include <map>
#include <zipper/unzipper.h>
#include <fstream>
#include <cctype>
#include "Logger.cpp"
#include <fstream>
#include <AclAPI.h>
#include <Lmcons.h>
#include <urlmon.h>
#include "json/json.h"
#pragma comment (lib, "urlmon.lib")

#define DEEPFORGE_TOOLSET_VERSION "0.1"
#define DB_URL "https://github.com/DeepForge-Technology/DeepForge-Toolset/releases/download/InstallerUtils/Versions.db"
#define OS_NAME "Windows"
#define UpdateManagerTable "UpdateManager_Windows"
#define NameVersionTable "WindowsVersions"
#define Locale_RU_URL "https://github.com/DeepForge-Technology/DeepForge-Toolset/releases/download/InstallerUtils/locale_ru.json"
#define Locale_EN_URL "https://github.com/DeepForge-Technology/DeepForge-Toolset/releases/download/InstallerUtils/locale_en.json"
#define PathmanURL_AMD64 "https://github.com/DeepForge-Technology/DeepForge-Toolset/releases/download/InstallerUtils/pathman-v0.5.2-windows-amd64.exe"
#define PathmanURL_ARM64 "https://github.com/DeepForge-Technology/DeepForge-Toolset/releases/download/InstallerUtils/pathman-v0.5.2-windows-amd64.exe"

using namespace std;
using namespace DB;
using namespace Bar;
using namespace zipper;

namespace Windows
{
    // int type
    int result;
    int Percentage;
    int TempPercentage = 0;
    // bool type
    bool Updating = true;
    bool withProgress = true;
    // float type
    double LastSize;
    double LastTotalSize;
    double DownloadSpeed;
    // string type
    string Answer;
    const string NewOrganizationFolder = "C:\\ProgramData\\DeepForge";
    const string NewApplicationFolder = NewOrganizationFolder + "\\DeepForge-Toolset";
    const string NewUpdateManagerFolder = NewOrganizationFolder + "\\UpdateManager";
    const string NewTempFolder = NewApplicationFolder + "\\Temp";
    std::filesystem::path ProjectDir = std::filesystem::current_path().generic_string();
    string DB_PATH = NewTempFolder + "\\Versions.db";
    const string TrueVarious[3] = {"yes", "y", "1"};
    string InstallDelimiter = "========================================================";
#if defined(__x86_64__)
    string Architecture = "amd64";
#elif __arm__ || __aarch64__ || _M_ARM64
    string Architecture = "arm64";
#endif
    Json::Value translate;
    // init classes
    Logger logger;
    ProgressBar_v1 progressbar;
    Database database;

    class WriteData : public IBindStatusCallback
    {
    public:
        HRESULT __stdcall QueryInterface(const IID &, void **)
        {
            return E_NOINTERFACE;
        }
        ULONG STDMETHODCALLTYPE AddRef(void)
        {
            return 1;
        }
        ULONG STDMETHODCALLTYPE Release(void)
        {
            return 1;
        }
        HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding *pib)
        {
            return E_NOTIMPL;
        }
        virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG *pnPriority)
        {
            return E_NOTIMPL;
        }
        virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved)
        {
            return S_OK;
        }
        virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError)
        {
            return E_NOTIMPL;
        }
        virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo)
        {
            return E_NOTIMPL;
        }
        virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed)
        {
            return E_NOTIMPL;
        }
        virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown *punk)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
        {
            if (withProgress == true)
            {
                Percentage = static_cast<float>(ulProgress) / static_cast<float>(ulProgressMax) * 100;
                if (TempPercentage != Percentage && TempPercentage <= 97)
                {
                    progressbar.Update((double)(ulProgress), (double)(ulProgressMax));
                    LastSize = (double)(ulProgress);
                    LastTotalSize = (double)(ulProgressMax);
                    TempPercentage = Percentage;
                }
            }
            return S_OK;
        }
    };
    // Main class
    class Installer
    {
    public:
        Installer()
        {
            // Set console code page to UTF-8 so console known how to interpret string data
            SetConsoleOutputCP(CP_UTF8);
            // Create temp folder
            MakeDirectory(NewTempFolder);
            cout << "Downloading database..." << endl;
            // Download database Versions.db
            Download(DB_URL, NewTempFolder,true);
            cout << "Database successfully downloaded." << endl;
            cout << InstallDelimiter << endl;
            database.open(&DB_PATH);
        }
        void CommandManager();
        void InstallUpdateManager();
        void InstallDeepForgeToolset(string channel);
        void ChangeUpdating();
        void ChangeLanguage()
        {
            string NumLang;
            cout << "1. Russian" << endl;
            cout << "2. English" << endl;
            cout << "Choose language (default - 1):";
            getline(cin, NumLang);
            if (NumLang == "1" || NumLang.empty())
            {
                ReadJSON("Russian");
            }
            else if (NumLang == "2")
            {
                ReadJSON("English");
            }
            // If the user enters a non-digit, then the method is called again
            else
            {
                ChangeLanguage();
            }
            cout << InstallDelimiter << endl;
        }
        // JSON file reading function with interface localization
    void ReadJSON(string language)
    {
        try
        {
            if (language == "Russian")
            {
                string LocaleDir = NewApplicationFolder + "\\locale";
                string LocalePath = LocaleDir + "\\locale_ru.json";
                MakeDirectory(LocaleDir);
                Download(Locale_RU_URL,LocaleDir,false);
                ifstream file(LocalePath);
                // File open check
                if (file.is_open())
                {
                    // Dictionary entry with translation
                    file >> translate;
                    file.close();
                }
            }
            else if (language == "English")
            {
                string LocaleDir = NewApplicationFolder + "\\locale";
                string LocalePath = LocaleDir + "\\locale_en.json";
                MakeDirectory(LocaleDir);
                Download(Locale_EN_URL,LocaleDir,false);
                ifstream file(LocalePath);
                // File open check
                if (file.is_open())
                {
                    // Dictionary entry with translation
                    file >> translate;
                    file.close();
                }
            }
        }
        catch (exception &error)
        {
            // Error output
            logger.WriteError("Function: ReadJSON");
            logger.WriteError(error.what());
            logger.SendError(Architecture,"Empty",OS_NAME,"ReadJSON",error.what());
        }
    }

    private:
        void Download(string url, string dir,bool Progress)
        {
            try
            {
                // Class for write data on windows
                WriteData writer;
                withProgress = Progress;
                // Get name of file from url
                string name = (url.substr(url.find_last_of("/")));
                string filename = dir + "/" + name.replace(name.find("/"), 1, "");
                // Call method for download file
                HRESULT Download = URLDownloadToFile(NULL, url.c_str(), filename.c_str(), 0, static_cast<IBindStatusCallback *>(&writer));
                switch (Download)
                {
                case -2146697211:
                    throw domain_error("No internet connection");
                case -2147467260:
                    throw domain_error("Connection reset");
                }
                if (Progress == true)
                {
                    // If the progress bar is not completely filled in, then paint over manually
                    if (Process < 100 && Process != Percentage)
                    {
                        for (int i = (Process - 1); i < 98; i++)
                        {
                            progressbar.Update(LastSize, LastTotalSize);
                        }
                        progressbar.Update(LastTotalSize, LastTotalSize);
                    }
                    // Reset all variables and preferences
                    progressbar.ResetAll();
                    Percentage = 0;
                    TempPercentage = 0;
                }
            }
            catch (exception &error)
            {
                string ErrorText = "==> ❌ " + string(error.what());
                logger.SendError(Architecture, "Empty", OS_NAME, "Download()", error.what());
                cerr << ErrorText << endl;
            }
        }
        void AddToPATH()
        {
            string Command = "C:\\ProgramData\\DeepForge\\DeepForge-Toolset\\Temp\\pathman-v0.5.2-windows-amd64.exe add " + NewApplicationFolder + " && del C:\\ProgramData\\DeepForge\\DeepForge-Toolset\\Temp\\pathman-v0.5.2-windows-amd64.exe";
            #if defined(__x86_64__)
                Download(PathmanURL_AMD64,NewTempFolder,false);
            #elif __arm__ || __aarch64__ || _M_ARM64
                Download(PathmanURL_ARM64,NewTempFolder,false);
            #endif
            system(Command.c_str());
        }
        // Method for create symlink on desktop
        void CreateSymlink(string nameSymlink, string filePath)
        {
            nameSymlink = nameSymlink + ".exe";
            filePath = filePath + ".exe";
            char *UserFolder = getenv("USERPROFILE");
            string symlinkPath = string(UserFolder) + "\\Desktop\\" + nameSymlink;
            CreateHardLinkA(symlinkPath.c_str(), filePath.c_str(), NULL);
        }
        void WriteInformation(string version)
        {
            try
            {
                map<string,string> ApplicationColumns = {
                    {"Name","TEXT"},
                    {"Version","TEXT"},
                };
                map<string,string> ApplicationFields = {
                    {"Name","DeepForge-Toolset"},
                    {"Version",version},
                };
                string pathFile = NewUpdateManagerFolder + "\\AppInformation.db";
                Database AppInformationDB;
                /* The bellow code is checking if a file exists at the specified path. If the file does not exist, it creates a new file and writes an empty string to it. Then, it opens a database connection using the file as the database path. It checks if a table named "Applications" exists in the database. If the table does not exist, it creates the table with the specified columns. Finally, it inserts values into the "Applications" table. */
                if (filesystem::exists(pathFile) == false)
                {
                    ofstream file(pathFile);
                    file << "";
                    file.close();
                    
                }
                AppInformationDB.open(&pathFile);
                /* The bellow code is creating a table called "Applications" in the AppInformationDB database using the CreateTable method. It then inserts values into the "Applications" table using the InsertValuesToTable method. The boolean variable "exists" is used to store the result of the CreateTable method, indicating whether the table creation was successful or not. The integer variable "result" is used to store the number of rows affected by the InsertValuesToTable method. */
                bool existsTable = AppInformationDB.CreateTable("Applications",ApplicationColumns);
                int existsValue = AppInformationDB.ExistNameAppInTable("Applications","DeepForge-Toolset");
                /* The code is checking if a table called "Applications" exists in the database. If the table does not exist (existsTable == -1), it inserts values into the table using the AppInformationDB.InsertValuesToTable() method. If the table does exist, it removes an application called "DeepForge-Toolset" from the table using the AppInformationDB.RemoveApplicationFromTable() method, and then inserts values into the table using the AppInformationDB.InsertValuesToTable() method. */
                if (existsTable == -1)
                {
                    int result = AppInformationDB.InsertValuesToTable("Applications",ApplicationFields);
                }
                else
                {
                    AppInformationDB.RemoveApplicationFromTable("Applications","DeepForge-Toolset");
                    int result = AppInformationDB.InsertValuesToTable("Applications",ApplicationFields);
                }
            }
            catch (exception &error)
            {
                // Error output
                logger.SendError(Architecture, "Empty", OS_NAME, "WriteInformation()", error.what());
                cerr << error.what() << endl;
            }
        }
        // Method of make string to lower
        string to_lower(string sentence)
        {
            string new_sentence = "";
            for (int i = 0; i < sentence.length(); i++)
            {
                char ch = sentence[i];
                // cout << ch << endl;
                ch = tolower(ch);
                new_sentence += ch;
            }
            return new_sentence;
        }
        /* The 'MakeDirectory' function is used to create a directory (folder) in the file system.*/
        void MakeDirectory(string dir)
        {
            string currentDir;
            string fullPath = "";
            string delimiter = "\\";
            size_t pos = 0;
            while ((pos = dir.find(delimiter)) != string::npos)
            {
                currentDir = dir.substr(0, pos);
                if (fullPath != "")
                {
                    fullPath = fullPath + "\\" + currentDir;
                    if (filesystem::exists(fullPath) == false)
                    {
                        CreateDirectoryA(fullPath.c_str(),NULL);
                    }
                }
                else
                {
                    fullPath = currentDir + "\\";
                }
                dir.erase(0, pos + delimiter.length());
            }
            if (fullPath != "")
            {
                fullPath = fullPath + "\\" + dir;
            }
            else
            {
                fullPath = dir + "\\";
            }
            if (filesystem::exists(fullPath) == false)
            {
                CreateDirectoryA(fullPath.c_str(),NULL);
            }
        }
        /*  The `UnpackArchive` function takes two parameters: `path_from` and `path_to`.
            It uses the `Unzipper` class to extract the contents of an archive file located at `path_from` and saves them to the directory specified by `path_to`.
            After extracting the contents, the function closes the `Unzipper` object.
        */
        void UnpackArchive(string path_from, string path_to)
        {
            Unzipper unzipper(path_from);
            unzipper.extract(path_to);
            unzipper.close();
        }
        void AddToStartupSystem(string filePath)
        {
            filePath = filePath + ".exe";
            HKEY hKey;
            const char *czStartName = "DeepForge-UpdateManager";
            // const char* czExePath   = "C:\\Users\\user\\AppData\\Roaming\\Microsoft\\Windows\\DeepForgeToolset.exe";

            LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER,
                                      "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                                      0, KEY_WRITE,
                                      &hKey);
            if (ERROR_SUCCESS == lnRes)
            {
                lnRes = RegSetValueEx(hKey,
                                      czStartName,
                                      0,
                                      REG_SZ,
                                      (unsigned char *)filePath.c_str(),
                                      strlen(filePath.c_str()));
            }

            RegCloseKey(hKey);
        }
        void RebootSystem()
        {
            system("shutdown -r -t 0");
        }
        // Function for check of answer
        bool CheckAnswer(string answer)
        {
            bool status;
            string Answer = to_lower(answer);
            // string Answer = answer;
            for (int i = 0; i < TrueVarious->size(); i++)
            {
                if (Answer == TrueVarious[i] || Answer.empty() || Answer == "\n" || Answer == "да" || Answer == "ДА" || Answer == "Да")
                {
                    status = true;
                    break;
                }
            }
            return status;
        }
    };
}