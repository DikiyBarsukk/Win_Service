#include <iostream>
#include <Windows.h>
#include <tchar.h>

#define SERVICE_NAME TEXT("MyWindowsService")

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;


void ServiceMain(int argc, LPTSTR* argv);
void ControlHandler(DWORD request);
void InitService();

// Service installation function
void InstallService() {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (schSCManager) {
        TCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);

        SC_HANDLE schService = CreateService(
            schSCManager,
            SERVICE_NAME,
            SERVICE_NAME,
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,
            SERVICE_ERROR_NORMAL,
            szPath,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

        if (schService) {
            CloseServiceHandle(schService);
        }
        CloseServiceHandle(schSCManager);
    }
}

// Service initialization
void InitService() {
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)ControlHandler);
}

// Main service function
void ServiceMain(int argc, LPTSTR* argv) {
    InitService();

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        // Main service logic goes here
        Sleep(1000);
    }
}

// Service control handler
void ControlHandler(DWORD request) {
    switch (request) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    default:
        break;
    }
    SetServiceStatus(hStatus, &ServiceStatus);
}

int _tmain(int argc, TCHAR* argv[]) {
    if (argc > 1 && _tcscmp(argv[1], TEXT("install")) == 0) {
        InstallService();
        return 0;
    }

    SERVICE_TABLE_ENTRY ServiceTable[] = {
    { (LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
    { NULL, NULL }
    };

    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}
