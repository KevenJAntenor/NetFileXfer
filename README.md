# NetFileXfer

## Project Objective
The goal of this assignment is to transfer files over a network. The client component reads the specified file and sends it to the server. The server receives the file and saves it in a destination file.

Connection management is already implemented for you. You have direct access to a file descriptor for writing data to send and reading data to receive.

## Key Requirements
To transfer files of arbitrary size, two elements are necessary. The first is to use `mmap()` for reading the file on the client side and writing the file on the server side. If the file does not fit in memory, the operating system will automatically free pages. The second element involves sending and receiving data in blocks. The block size is a parameter of the function you need to complete.

## Code Completion
The code to be completed is in the `src/xfer.c` file. It contains two functions: `xfer_send_file()` and `xfer_recv_file()`. Comments indicate the general approach to realize these two functions. You must use `mmap()` for reading and writing the file, and the `sockfd` descriptor for sending and receiving the file over the socket.

## Testing
You have access to the `test_xfer` program. The main test transfers a file and verifies that the received file is identical for different combinations of block size and file sizes. It also checks for any file descriptor leaks.

## Benchmarks
A benchmark suite is provided. It measures transfer speed as a function of block size. Run it before making your archive for submission with the command `make runbench`.

## Compilation and Execution Instructions

1. **Compile the Code:** Use the command `make` in the project directory to compile the code.

2. **Run Benchmarks:** Execute `make runbench` to run the benchmark tests and evaluate block size impacts.

3. **Test the Implementation:** Use `./test_xfer` to run the main file transfer tests.

4. **Create Submission Archive:** Once all tests are passed, create a submission archive using `make dist`.
