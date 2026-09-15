#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <future>
#include <thread>

namespace fs = std::filesystem;

std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool compareFiles(const std::string &file1, const std::string &file2)
{
    std::ifstream f1(file1), f2(file2);
    if (!f1.is_open() || !f2.is_open())
        return false;

    std::string line1, line2;
    while (true)
    {
        bool has1 = static_cast<bool>(std::getline(f1, line1));
        bool has2 = static_cast<bool>(std::getline(f2, line2));
        if (!has1 && !has2)
            break;
        if (trim(line1) != trim(line2))
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: ./runner <source.cpp> <test_directory>\n";
        return 1;
    }

    std::string sourceFile = argv[1];
    std::string testDir = argv[2];
    int timeLimitSeconds = 2;

   
#ifdef _WIN32
    std::string binName = "temp_bin.exe";
#else
    std::string binName = "./temp_bin";
#endif

    std::cout << "[INFO] Compiling " << sourceFile << "...\n";
    std::string compileCmd = "g++ -O3 -std=c++17 " + sourceFile + " -o " + binName;
    if (std::system(compileCmd.c_str()) != 0)
    {
        std::cerr << "[ERROR] Compilation failed.\n";
        return 1;
    }

    fs::path inputDir = fs::path(testDir) / "inputs";
    fs::path expectedDir = fs::path(testDir) / "expected";
    fs::path outputDir = fs::path(testDir) / "outputs";

    if (fs::exists(outputDir))
    {
        fs::remove_all(outputDir);
    }
    fs::create_directories(outputDir);

    int passed = 0, total = 0;

    for (const auto &entry : fs::directory_iterator(inputDir))
    {
        if (!entry.is_regular_file())
            continue;

        total++;
        std::string testName = entry.path().filename().string();
        fs::path expectedFile = expectedDir / testName;
        fs::path generatedFile = outputDir / testName;

     
#ifdef _WIN32
        std::string runCmd = binName + " < \"" + entry.path().string() + "\" > \"" + generatedFile.string() + "\"";
#else
        std::string runCmd = "./" + binName + " < \"" + entry.path().string() + "\" > \"" + generatedFile.string() + "\"";
#endif

        auto start = std::chrono::high_resolution_clock::now();

        std::future<int> result = std::async(std::launch::async, [&]()
                                             { return std::system(runCmd.c_str()); });

        std::future_status status = result.wait_for(std::chrono::seconds(timeLimitSeconds));
        int exitCode = 0;

        if (status == std::future_status::timeout)
        {
#ifdef _WIN32
            std::system("taskkill /F /T /IM temp_bin.exe >nul 2>&1");
#else
            std::system("pkill -9 -f temp_bin >/dev/null 2>&1");
#endif
            exitCode = -1; 
            result.wait(); 
        }
        else
        {
            exitCode = result.get(); 
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = end - start;

        if (exitCode != 0)
        {
            if (status == std::future_status::timeout)
            {
                std::cout << "[-] " << testName << ": TLE (Exceeded " << timeLimitSeconds << "s limit)\n";
            }
            else
            {
                std::cout << "[-] " << testName << ": RUNTIME ERROR (Segfault/Abort)\n";
            }
            continue;
        }

        if (fs::exists(expectedFile))
        {
            if (compareFiles(generatedFile.string(), expectedFile.string()))
            {
                std::cout << "[+] " << testName << ": PASSED (" << ms.count() << " ms)\n";
                passed++;
            }
            else
            {
                std::cout << "[-] " << testName << ": FAILED (Mismatch)\n";
            }
        }
        else
        {
            std::cout << "[?] " << testName << ": GENERATED (" << ms.count() << " ms) - No expected file.\n";
        }
    }

    std::cout << "\nResults: " << passed << "/" << total << " tests matched.\n";
    
    if (fs::exists(binName)) {
        fs::remove(binName);
    }
    
    return (passed == total ? 0 : 1);
}