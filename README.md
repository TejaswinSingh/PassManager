# PassManager
Securely manage passwords with this efficient C-based password manager utilizing hashtable data structure for optimal performance and reliable storage.

## Getting Started

1. Compile the `control.c` file.
2. Run the program for the first time. It will create a folder called `PassManager` in the C drive.
3. Copy the `testdata.txt` and `testdata2.txt` files to the `PassManager` folder.

## Features

- Password masking: Passwords are masked when entered for enhanced security.
- Login password: The app requires a login password to open. If you forget the password, it can be reset to the default - `0000`, but all records will be deleted.
- Manage Records: The password manager allows you to search, view, edit, add new records, and remove records from the database.
- Database Operations: You can view the entire database, see the hashtable representation of the data structure used for storage, make backups of your passwords, and dump the database with test data for testing purposes.
- Random Password Generator: Generate strong and secure passwords using the built-in random password generator.
- Reset and Delete: Reset the login password or delete the entire record database.
- Data Encryption: Passwords are stored in an encrypted file for added security.

## Dependencies

This project does not have any external dependencies.

## License

This project is licensed under the [MIT License](LICENSE).

