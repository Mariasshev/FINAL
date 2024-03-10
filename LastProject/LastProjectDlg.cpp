#include "LastProject.h"

HWND hChooseFileBut, hStartBut, hStopBut, hEndBut,hResumeBut, hWords, hResult, hProgress;
FindWords* FindWords::ptr = NULL;
HANDLE hMutex;
HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
vector<string> words;
HWND hDialog;
HANDLE hThreads[5];
bool stopProgress = false;


vector<string> filesWithWords;
vector<string> filePaths;
vector<int> fileSizes;
vector<int> wordsAmount;
map<string, int> wordCountMap;

int totalWordCount = 0;  // words counter

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

BOOL FindWords::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    hDialog = hwnd;
    //buttons
    hChooseFileBut = GetDlgItem(hwnd, IDC_BUTTON3);
    hStartBut = GetDlgItem(hwnd, IDC_BUTTON1);
    hStopBut = GetDlgItem(hwnd, IDC_BUTTON2);
    hEndBut = GetDlgItem(hwnd, IDC_BUTTON4);
    hResumeBut = GetDlgItem(hwnd, IDC_BUTTON5);

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

DWORD WINAPI ProgressThread(LPVOID lpParam)
{
    for (int i = 0; i <= 100; ++i) {
        SendMessage(hProgress, PBM_SETPOS, i, 0);
        Sleep(100);

        // Перевірка, чи треба зупинити потік
        if (stopProgress) {
            // Призупинення потоку
            while (stopProgress) {
                Sleep(100);
            }
            // Повторне встановлення позиції прогресу, якщо потік відновлено
            SendMessage(hProgress, PBM_SETPOS, i, 0);
        }
    }
    return 0;
}

//---------------------------

//Функция для записи статистики в файл
void WriteStatisticsToFile() {
    SetEvent(hEvent);
    ofstream outputFile("statistic.txt");

    if (outputFile.is_open()) {

        for (size_t i = 0; i < filesWithWords.size(); ++i) {
            Sleep(100);
            outputFile << "File Name: " << filesWithWords[i] << endl;
            outputFile << "File Path: " << filePaths[i] << endl;
            outputFile << "File Size: " << fileSizes[i] << " bytes" << endl;
            outputFile << "Amount of words: " << wordsAmount[i] << " words" << endl;
            outputFile << endl;
        }

        // Записываем информацию о самых популярных словах
        outputFile << "Most Popular Words:" << endl;
        for (const auto& entry : wordCountMap) {
            outputFile << entry.first << ": " << entry.second << " occurrences" << endl;
        }

        outputFile.close();


    }
}

// Функция для получения информации о файле
void GetFileInfo(const string& filePath, string& fileName, int& fileSize) {
    fileName = filePath.substr(filePath.find_last_of("\\") + 1);

    ifstream file(filePath, ios::binary | ios::ate);
    fileSize = static_cast<int>(file.tellg());
}

// Функция для получения статистики
void GetStatistics(const string& filePath) {
    string fileName;
    int fileSize;


    GetFileInfo(filePath, fileName, fileSize);

    // Добавляем информацию о файле
    filesWithWords.push_back(fileName);
    filePaths.push_back(filePath);
    fileSizes.push_back(fileSize);

}

// Функция для записи слов в файл
void WriteWordsToFile() {
    ofstream outputFile("statistic.txt", ios::app);

    if (outputFile.is_open()) {
        outputFile << "Words in array: ";
        for (const auto& word : words) {
            outputFile << word << " ";
        }
        outputFile << endl;
        outputFile.close();
    }
}

// Функция для подсчета слов
DWORD WINAPI CountWordsInFile(LPVOID lpParam) {
    string filePath = static_cast<const char*>(lpParam);
    FindWords* findWords = (FindWords*)lpParam;
    ifstream file(filePath);
    int counter = 0;
    if (file.is_open()) {
        string word;
        counter = 0;
        while (file >> word) {
            if (find(words.begin(), words.end(), word) != words.end()) {
                ++totalWordCount;
                ++counter;
            }
        }
        wordsAmount.push_back(counter);
        file.close();
        

        // Получаем статистику о файле
        GetStatistics(filePath);
    }

    return 0;
}
DWORD WINAPI FileProcessingThread(LPVOID lpParam);

// Функция для записи статистики в файл
DWORD WINAPI StatisticsThread(LPVOID lpParam) {
    WriteStatisticsToFile();
    return 0;
}


//---------------------------

void WriteStars() {

    size_t currentIndex = 0;
    wstring filePath = L"C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\res.txt";

    // create a file
    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        // write
        DWORD bytesWritten;
        for (const auto& word : words) {
            string replacement = "******* ";
            currentIndex++;
            if (!WriteFile(hFile, replacement.c_str(), static_cast<DWORD>(replacement.size()), &bytesWritten, NULL)) {
                MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
                CloseHandle(hFile);
                return;
            }
            if (currentIndex == words.size())
            {
                MessageBoxA(NULL, "File created!", "Result", MB_OK | MB_ICONINFORMATION);

            }
        }

        // close file
        CloseHandle(hFile);
    }
    else
    {
        MessageBoxA(NULL, "Failed to create file", "Result", MB_OK | MB_ICONINFORMATION);
    }

}


// Основная функция обработки файлов
DWORD WINAPI FileProcessingThread(LPVOID lpParam) {
    FindWords* ptr = (FindWords*)lpParam;

    // Создаем поток для записи статистики
    HANDLE hStatisticsThread = CreateThread(NULL, 0, StatisticsThread, NULL, 0, NULL);
    // directory with files
    string directoryPath = "C:\\Users\\Admin\\source\\repos\\LastProject\\LastProject\\texts\\";
    for (int i = 1; i <= 5; ++i) {
        //searching for files text1.txt, text2.txt...
        string filePath = directoryPath + "text" + to_string(i) + ".txt";

        // create threads for files
        HANDLE hThread = CreateThread(NULL, 0, CountWordsInFile, (LPVOID)filePath.c_str(), 0, NULL);

        // save thread`s handle in array
        hThreads[i - 1] = hThread;
        // waiting for thread to complete
        WaitForSingleObject(hThread, INFINITE);
    }
    
    WaitForMultipleObjects(5, hThreads, TRUE, INFINITE);
    //WaitForSingleObject(StatisticsThread, INFINITE);
    // create a thread for the progress bar
    
    HANDLE hProgressThread = CreateThread(NULL, 0, ProgressThread, hProgress, 0, NULL);
    // waiting end of progress bar
    WaitForSingleObject(hProgressThread, INFINITE);

    WaitForSingleObject(hEvent, INFINITE);
    Sleep(500);
       

    // closing thread handles
    for (int i = 0; i < 5; ++i) {
        CloseHandle(hThreads[i]);
    }
    // waiting for statistics thread to complete


    // show result in MessageBox
    string resultMessage = "Total word count in files: " + to_string(totalWordCount);
    MessageBoxA(NULL, resultMessage.c_str(), "Result", MB_OK | MB_ICONINFORMATION);
    WriteStars();
    WriteWordsToFile();

    char buf[100];
    ifstream in("statistic.txt", ios::in | ios::binary);

    if (!in)
    {
        MessageBoxA(hDialog, "Ошибка открытия файла!", "Ошибка", MB_OK | MB_ICONINFORMATION);
        return 1;
    }
    else
    {
        while (!in.eof())
        {
            in.getline(buf, 100);

            SendMessageA(hResult, EM_REPLACESEL, 0, (LPARAM)buf);
            SendMessageA(hResult, EM_REPLACESEL, 0, (LPARAM)"\r\n");
        }
    }

    in.close();

    return 0;
}


void FindWords::Cls_OnTimer(HWND hwnd, UINT id)
{
    SendMessage(hProgress, PBM_STEPIT, 0, 0); 
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
            EnableWindow(hStartBut, TRUE);
            CreateThread(NULL, 0, FileProcessingThread, NULL, 0, NULL);
        }
        if (LOWORD(wParam) == IDC_BUTTON2)
        {
            stopProgress = true;
            
        }
        if (LOWORD(wParam) == IDC_BUTTON5)
        {
            stopProgress = false;

        }
    
        break;


    }
    // get text length
    int textLength = GetWindowTextLength(hWords);

    if (textLength == 0)
    {

    }
    else
    {
        vector<TCHAR> buffer(textLength + 1);
        GetWindowText(hWords, buffer.data(), textLength + 1);

        // Преобразуем TCHAR в string
        string text(buffer.begin(), buffer.end());

        words.clear();

        // Используем потоковый ввод для разбивки строки на слова
        istringstream stream(text);
        string word;

        // Помещаем слова в векторный массив
        while (stream >> word)
        {
            words.push_back(word);
        }

        EnableWindow(hStartBut, TRUE);
    }

    return FALSE;
}