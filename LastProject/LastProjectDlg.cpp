#include "LastProject.h"

HWND hChooseFileBut, hStartBut, hStopBut, hEndBut, hWords, hResult, hProgress;
FindWords* FindWords::ptr = NULL;
HANDLE hMutex;
BOOL timerStarted = FALSE;
vector<string> words;
HWND hDialog;
HANDLE hThreads[5];

FindWords::FindWords(void)
{
    ptr = this;
    hDialog = NULL;
}

FindWords::~FindWords(void)
{

}

void FindWords::Cls_OnClose(HWND hwnd)
{
    ReleaseMutex(hMutex); // освобождение мьютекса
    EndDialog(hwnd, 0);
}


/*DWORD WINAPI WriteResult(LPVOID lp)
{
    FindWords* ptr = (FindWords*)lp;
    HWND hEdit = GetDlgItem(ptr->hDialog, IDC_EDIT1);
    HWND hButton = GetDlgItem(ptr->hDialog, IDC_BUTTON1);
    char buf[100];
    ifstream in("music.txt", ios::in | ios::binary);

    if (!in)
    {
        MessageBoxA(ptr->hDialog, "Ошибка открытия файла!", "Мьютекс", MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, L"{0D68E69C-F6D7-456B-ADE5-8277ED004AE3}");

    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    ofstream out("coding.txt", ios::out | ios::trunc | ios::binary);
    const int KEY = 100;
    while (!in.eof())
    {
        in.getline(buf, 100);
        out << buf << endl;

        SendMessageA(hEdit, EM_REPLACESEL, 0, (LPARAM)buf);
        SendMessageA(hEdit, EM_REPLACESEL, 0, (LPARAM)"\r\n");
    }

    out.close();
    in.close();

    return 0;
}
*/
 
BOOL FindWords::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    hDialog = hwnd;
    //buttons
    hChooseFileBut = GetDlgItem(hwnd, IDC_BUTTON3);
    hStartBut = GetDlgItem(hwnd, IDC_BUTTON1);
    hStopBut = GetDlgItem(hwnd, IDC_BUTTON2);
    hEndBut = GetDlgItem(hwnd, IDC_BUTTON4);

    //edits
    hWords = GetDlgItem(hwnd, IDC_EDIT3);
    hResult = GetDlgItem(hwnd, IDC_EDIT2);

    //progress
    hProgress = GetDlgItem(hDialog, IDC_PROGRESS1);
    SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // установка интервала для индикатора 
    SendMessage(hProgress, PBM_SETSTEP, 20, 0); // установка шага приращения  индикатора 
    SendMessage(hProgress, PBM_SETPOS, 0, 0); // установка текущей позиции индикатора
    SendMessage(hProgress, PBM_SETBKCOLOR, 0, LPARAM(RGB(0, 0, 255))); // установка цвета фона индикатора
    SendMessage(hProgress, PBM_SETBARCOLOR, 0, LPARAM(RGB(255, 255, 0))); // установка цвета заполн¤емых пр¤моугольников

    EnableWindow(hStartBut, FALSE);

    return TRUE;
}

int totalWordCount = 0;  // Общий счетчик слов
int countMutex;  // Мьютекс для безопасного обновления счетчика

DWORD WINAPI CountWordsInFile(LPVOID lpParam) 
{
    string filePath = static_cast<const char*>(lpParam);

    ifstream file(filePath);
    if (file.is_open()) {
        string word;
        while (file >> word) 
        {
            if (find(words.begin(), words.end(), word) != words.end()) 
            {
                countMutex++;
                ++totalWordCount;
            }
        }
        file.close();
    }

    return 0;
}
void WriteWordsToFile() {
    
    size_t currentIndex = 0;
    wstring filePath = L"C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\res.txt";

    // Создаем или открываем файл для записи
    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        // Записываем текст в файл
        //int check = 0;
        DWORD bytesWritten;
        for (const auto& word : words) {
            string replacement = "******* ";

            if (!WriteFile(hFile, replacement.c_str(), static_cast<DWORD>(replacement.size()), &bytesWritten, NULL)) {
                MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
                CloseHandle(hFile);
                //check = true;
                return;
            }
            if(currentIndex == words.size())
            {
                MessageBoxA(NULL, "File created!", "Result", MB_OK | MB_ICONINFORMATION);

            }
        }
        
        /*if (WriteFile(hFile, textToWrite, static_cast<DWORD>(strlen(textToWrite)), &bytesWritten, NULL)) {
            MessageBoxA(NULL, "Sucsess", "Result", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
        }*/

        // Закрываем файл
        CloseHandle(hFile);
    }
    else {
        MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
    }


    //wstring filePath = L"C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\";
    //HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    //if (hFile == INVALID_HANDLE_VALUE) {
    //    MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);

    //    return;
    //}

    //for (const auto& word : words) {
    //    string replacement = "******* ";

    //    DWORD bytesWritten;
    //    if (!WriteFile(hFile, replacement.c_str(), static_cast<DWORD>(replacement.size()), &bytesWritten, NULL)) {
    //        MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
    //        CloseHandle(hFile);
    //        return;
    //    }

    //    // Добавляем разделитель между словами (просто для примера)
    //    const char separator = ' ';
    //    if (!WriteFile(hFile, &separator, sizeof(separator), &bytesWritten, NULL)) {
    //        MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
    //        CloseHandle(hFile);
    //        return;
    //    }
    //}

    //CloseHandle(hFile);
    //MessageBoxA(NULL, "File created and words replaced successfully", "Result", MB_OK | MB_ICONINFORMATION);
}
//DWORD WINAPI FileProcessingThread(LPVOID lpParam) 
//{
//    // directory with files
//    string directoryPath = "C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\";
//
//    for (int i = 1; i <= 5; ++i) 
//    {  //looking for files text1.txt, text2.txt...
//        string filePath = directoryPath + "text" + to_string(i) + ".txt";
//
//        // Создание потока для каждого файла
//        HANDLE hThread = CreateThread(NULL, 0, CountWordsInFile, (LPVOID)filePath.c_str(), 0, NULL);
//
//        // Ожидание завершения потока
//        WaitForSingleObject(hThread, INFINITE);
//
//        // Закрытие дескриптора потока
//        CloseHandle(hThread);
//    }
//
//    // Вывод результата в MessageBox
//    string resultMessage = "Total word count in files: " + to_string(totalWordCount);
//    MessageBoxA(NULL, resultMessage.c_str(), "Result", MB_OK | MB_ICONINFORMATION);
//
//    return 0;
//}

DWORD WINAPI ProgressThread(LPVOID lpParam) {
    HWND hProgress = reinterpret_cast<HWND>(lpParam);

    for (int i = 0; i <= 100; ++i) {
        // Устанавливаем текущую позицию
        SendMessage(hProgress, PBM_SETPOS, i, 0);
        Sleep(100); 
    }
    return 0;
}

DWORD WINAPI FileProcessingThread(LPVOID lpParam) {
    FindWords* ptr = (FindWords*)lpParam;

    // directory with files
    string directoryPath = "C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\";
    for (int i = 1; i <= 5; ++i) {  //looking for files text1.txt, text2.txt...
        string filePath = directoryPath + "text" + to_string(i) + ".txt";

        // Создание потока для каждого файла
        HANDLE hThread = CreateThread(NULL, 0, CountWordsInFile, (LPVOID)filePath.c_str(), 0, NULL);

        // Сохраняем дескриптор потока в массив
        hThreads[i - 1] = hThread;

        // Ожидание завершения потока
        WaitForSingleObject(hThread, INFINITE);
    }

    // Устанавливаем таймер для отслеживания завершения операций
    timerStarted = TRUE;
    HANDLE hProgressThread = CreateThread(NULL, 0, ProgressThread, hProgress, 0, NULL);

    WaitForSingleObject(hProgressThread, INFINITE);
    Sleep(500);
    WaitForMultipleObjects(5, hThreads, TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < 5; ++i) {
        CloseHandle(hThreads[i]);
    }

    // Вывод результата в MessageBox
    string resultMessage = "Total word count in files: " + to_string(totalWordCount);
    MessageBoxA(NULL, resultMessage.c_str(), "Result", MB_OK | MB_ICONINFORMATION);

    WriteWordsToFile();

    return 0;
}

void FindWords::Cls_OnTimer(HWND hwnd, UINT id)
{
    SendMessage(hProgress, PBM_STEPIT, 0, 0); //изменение позиции
}

BOOL CALLBACK FindWords::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
        HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON4) 
        {
            ReleaseMutex(hMutex);
            EndDialog(hwnd, 0);
        }
        if (LOWORD(wParam) == IDC_BUTTON3) 
        {
            TCHAR FullPath[MAX_PATH] = { 0 };
            OPENFILENAME open = { sizeof(OPENFILENAME) };
            open.hwndOwner = hwnd;
            open.lpstrFilter = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0");
            open.lpstrFile = FullPath;
            open.nMaxFile = MAX_PATH;
            open.lpstrInitialDir = TEXT("C:\\");
            open.Flags = OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&open))
            {
                ifstream file(open.lpstrFile);
                if (file.is_open())
                {
                    string word;
                    while (file >> word)
                    {
                        //add words in array
                        words.push_back(word);
                    }
                    file.close();

                    string messageText= "Words in the file:\n";
                    for (const auto& word : words) 
                    {
                        messageText += word + "\r\n";
                    }

                    EnableWindow(hWords, FALSE);
                    SetWindowText(hChooseFileBut, TEXT("File chosen!"));
                    EnableWindow(hStartBut, TRUE);
                    MessageBoxA(hwnd, messageText.c_str(), "File Content", MB_OK | MB_ICONINFORMATION);
                    
                }
                else
                {
                    MessageBox(hwnd, TEXT("Error opening the file!"), TEXT("Error"), MB_OK | MB_ICONERROR);
                }
            }
        }
        if (LOWORD(wParam) == IDC_BUTTON1)
        {
            
            CreateThread(NULL, 0, FileProcessingThread, NULL, 0, NULL);
        }
    
        break;


    case WM_TIMER:
        
        break;


    }
    return FALSE;
}