
# 🚀 Automated C++ Test Runner

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)


A lightweight, high-performance C++ utility to automate the compilation, execution, and output validation of algorithmic solutions. Perfect for competitive programming and checking edge cases before technical interviews.

## 📂 1. Set Up Your Folders

Your workspace needs a specific structure so the runner knows where to look for your inputs and where to save the outputs. Create a directory that looks exactly like this:

```
Test_Runner/
├── tester.cpp       # The tool from this repository
├── algorithm.cpp          # The C++ code you want to test
└── test_cases/        # Your test folder (can be named anything)
    ├── inputs/           # Put your .txt or .in files here
    └── expected/         # (Optional) Put the correct .txt or .out files here
```


*(Note: The runner will automatically create an `outputs/` folder inside your test directory when you run it, and it will clean out old files for you).*

## 💻 2. Write Your Code

Your `algorithm.cpp` doesn't need any special file I/O code. Just read from `cin` and print to `cout` like you normally would on LeetCode or Codeforces. The runner handles the file piping automatically.

## 🛠️ 3. Compile the Test Runner

Before you can test your code, you need to compile the test runner utility itself. Open your terminal in the `Test_Runner` and run:

**Linux / macOS:**

```bash
g++ -O3 -std=c++17 tester.cpp -o runner

```

**Windows:**

```cmd
g++ -O3 -std=c++17 tester.cpp -o runner.exe

```

## 🚀 4. Run the Tests

Execute the compiled runner by passing two arguments:

1. The path to the C++ file you want to test.
2. The path to the folder containing your test cases.

```bash
./runner algorithm.cpp ./test_cases

```

## 📊 5. Read the Output

The runner will compile your solution and instantly test it against every file in the `inputs/` folder. It enforces a strict **2.0-second time limit** to catch infinite loops.

If you included an `expected/` folder, it will compare your output line-by-line:

```text
[INFO] Compiling solution.cpp...

[+] test_01.txt: PASSED (12.4 ms)
[+] test_02.txt: PASSED (15.1 ms)
[-] test_03.txt: FAILED (Mismatch)
[-] test_04.txt: RUNTIME ERROR (Segfault/Abort)
[-] test_05.txt: TLE (Exceeded 2s limit)

Results: 2/5 tests matched.

```

If you **did not** include an `expected/` folder, it simply generates the outputs for you to review manually:

```text
[INFO] Compiling solution.cpp...

[?] test_01.txt: GENERATED (12.4 ms) - No expected file.
[?] test_02.txt: GENERATED (15.1 ms) - No expected file.

```

