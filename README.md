# 🗃️ KacperSQL

KacperSQL is a simple SQL database engine written in C. It allows creating, modifying, and querying a database. 🖥️💾

## 📚 Table of Contents

- [Project Description](#project-description)
- [Installation](#installation)
- [Usage](#usage)
- [Available Commands](#available-commands)
- [Examples](#examples)

## 📜 Project Description

KacperSQL is a database project written in C that implements basic SQL operations such as inserting, updating, deleting, and selecting data. It also supports table management and helper commands. 🛠️🔍

## 🔧 Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/Kerciu/sqlite-clone.git
   ```

2. Navigate to the project directory:

   ```bash
   cd kacpersql
   ```

3. Build the project using Makefile:

   ```bash
   make
   ```

## 🏃 Usage

1. Run the program as it is or choose a working file:

   ```bash
   ./KacperSql
   ```
   
   ```bash
   ./KacperSql <file-name>
   ```

2. Enter a command to open or create a table:

   ```sql
   OPEN TABLE <file-name>
   ```

3. Use the available SQL commands to interact with the database. 📋

## 🛠️ Available Commands

### `HELP`

Displays available commands and details for each command:

- `INSERT <id> <username> <email>`: Inserts a new row into the table. 🆕
- `SELECT *`: Selects all rows from the table. 🔍
- `SELECT LIMIT <id>`: Selects rows up to the specified id. 📊
- `SELECT BETWEEN <id> AND <id>`: Selects rows between two ids. 📈
- `DELETE <id>`: Deletes the row with the specified id. ❌
- `UPDATE <id> <username> <email>`: Updates the row with the specified id. ✏️
- `OPEN TABLE <file-name>`: Opens an existing table or creates a new one with the specified file name. 📂
- `DROP`: Clears the currently working table. 🗑️

### `HELP <command>`

Displays details for a specific command. ❓

## 📑 Examples

1. **Creating a Table:**

   ```sql
   OPEN TABLE <file-name>
   ```

2. **Inserting a Record:**

   ```sql
   INSERT 1 Kacper kerciuuu@gmail.com
   ```

3. **Selecting Records:**

   ```sql
   SELECT *
   ```

4. **Updating a Record:**

   ```sql
   UPDATE 1 Kacper kerciuuu@gmail.com
   ```

5. **Deleting a Record:**

   ```sql
   DELETE 1
   ```

## 📝 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details. 📜

## 📬 Contact

If you have any questions, feedback, or suggestions, feel free to reach out through the following channels:

- **Author**: [Kacper Górski](mailto:kacper.gorski.contact@gmail.com) 🧑‍💻
- **LinkedIn**: [Kacper Górski](https://www.linkedin.com/in/kacper-gorski-se/) 🌐
- **Instagram**: [@kxcper.gorski](https://www.instagram.com/kxcper.gorski/) 📸
- **X**: [@Kerciuu](https://x.com/Kerciuu) 🐦
- **GitHub**: [Kerciu](https://github.com/Kerciu) 🧑‍💻
- **Support**: For technical support, please email [kerciuuu@gmail.com](mailto:kerciuuu@gmail.com) 📧

For issues, please open a [GitHub Issue](https://github.com/Kerciu/sqlite-clone/issues). 🚀

---
