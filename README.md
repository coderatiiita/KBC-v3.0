# KBC-v3.0

"Kaun Banega Crorepati" (KBC) is a popular Indian television game show based on the British program "Who Wants to Be a Millionaire?". This project is a command-line C++ implementation of the KBC game.

## Features

*   **Engaging Gameplay:** Experience the thrill of KBC with a series of multiple-choice questions leading to a grand prize.
*   **Question Bank:** Questions are stored in a binary file (`q.bin`), ensuring they are not easily viewable outside the game.
*   **Randomized Questions:** The order of questions is shuffled in each game instance, providing a unique experience every time.
*   **Lifelines:** Utilize lifelines to help you answer difficult questions.
*   **Admin Console:** A password-protected admin console allows for adding new questions to the question bank.
*   **Command-Line Interface:** The game is played entirely within the command-line interface (CLI), without requiring a graphical user interface (GUI).

## Technologies Used

*   C++
*   C++ File I/O libraries for question bank management.

## How to Compile and Run

1.  **Prerequisites:**
    *   A C++ compiler (like g++)
    *   Make (optional, but recommended for easier compilation)

2.  **Compilation:**
    *   **Using Make (Recommended):**
        If a `Makefile` is provided (or you create one), navigate to the project directory in your terminal and run:
        ```bash
        make
        ```
    *   **Using g++ (Manual Compilation):**
        Navigate to the project directory in your terminal and compile the main source file. For example, if your main source file is `kbcv3.cpp`, you would use:
        ```bash
        g++ kbcv3.cpp -o kbcv3_game
        ```
        (Adjust the command based on your actual source file name. On Windows, the output file might be named `kbcv3_game.exe`.)

3.  **Running the Game:**
    After successful compilation, an executable file will be created (e.g., `kbcv3_game` on Linux/macOS or `kbcv3_game.exe` on Windows). Run it from your terminal:
    ```bash
    ./kbcv3_game       # On Linux/macOS
    .\kbcv3_game.exe # On Windows
    ```

## Admin Console
The admin console is used to add new questions to the game's question bank.

*   **Accessing the Console:** To access the admin console, select the 'Administrator menu' option from the main menu. You will be prompted for a password. The default password is `kbc?`.
*   **Admin Features:** Once in the console, you can perform the following actions:
    *   Add new questions to the question bank (you will be guided to input the question, four multiple-choice options, and the correct answer).
    *   Display all questions currently in the bank.
    *   View the total number of questions in the bank.

## Lifelines

The game includes the following three lifelines to assist players. Each lifeline can be used once per game:

*   **50-50:** Eliminates two incorrect options, leaving the player with one correct and one incorrect option.
*   **Drop the Question:** Allows the player to skip the current question and move to the next one without any penalty. The skipped question is not answered and does not count towards winnings.
*   **Double Dip:** Allows the player to make two guesses for a single question. If the first guess is incorrect, they get an immediate second attempt at the same question with the remaining options.

## Question Bank (`q.bin`)

*   The questions for the game are stored in a binary file named `q.bin`.
*   This format helps to prevent easy viewing or modification of the questions outside of the game's admin console.
*   The admin console is the proper way to add new questions to this file.
*   The order of questions presented to the player is randomized in each game session.

## How to Contribute

Contributions are welcome! If you'd like to contribute to this project, please follow these steps:

1.  **Fork the repository.**
2.  **Create a new branch for your feature or bug fix:**
    ```bash
    git checkout -b feature/your-feature-name
    ```
3.  **Make your changes.**
4.  **Test your changes thoroughly.**
5.  **Commit your changes:**
    ```bash
    git commit -m "Add your descriptive commit message here"
    ```
6.  **Push to your branch:**
    ```bash
    git push origin feature/your-feature-name
    ```
7.  **Create a Pull Request** against the `main` (or `master`) branch of this repository.

Please ensure your code follows the existing style and includes comments where necessary.

## License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2024 Shubham INC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
