#include <iostream>
#include <fstream>
#include <filesystem>


namespace fs = std::filesystem;

#define FILE_BLOCK_SIZE 1024
#define TEST_TEXT_SIZE 1024

inline bool is_text_file(const fs::path& file_path) {
    std::ifstream f(file_path, std::ios::binary);
    if (!f) return false;

    char buf[TEST_TEXT_SIZE];
    f.read(buf, sizeof(buf));
    std::streamsize n = f.gcount();

    for (std::streamsize i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 0 || c < 0x09 || c > 0x0D && c < 0x20) 
            return false;
    }
    return true;
}

size_t fix_buffer(char* buffer, size_t length) {
    size_t j = 0;
    for (size_t i = 0; i < length; i++) {
        if (buffer[i] != '\r') {
            buffer[j++] = buffer[i];
        }
    }
    return j;
}

void fix_file(const fs::path& file_path) {
    if (is_text_file(file_path)) {
        std::fstream file(file_path, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return;
        }
        char buffer[FILE_BLOCK_SIZE];
        size_t write_pos = 0;
        size_t read_pos = 0;
        bool end_read = false;
        while (!end_read) {
            file.seekg(read_pos, std::ios::beg);
            file.read(buffer, sizeof(buffer));
            read_pos += file.gcount();
            size_t fixed_len = fix_buffer(buffer, file.gcount());
            if (!file) {
                end_read = true;
            }
            file.clear();
            file.seekp(write_pos, std::ios::beg);
            file.write(buffer, fixed_len);
            write_pos += fixed_len;
        }
        file.close();
        fs::resize_file(file_path, write_pos);
    }

    std::cout << "fixed file: " << file_path << std::endl;
}

void fix_directory(const fs::path& dir_path) {
    for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
        if (fs::is_regular_file(entry.path())) {
            fix_file(entry.path());
        } else {
            std::cout << "skipped: " << entry.path() << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Missing require arguments, dir or file path" << std::endl;
        return 1;
    }

    fs::path path(argv[1]);

    if (!fs::exists(path)) {
        std::cerr << "The specified path does not exist: " << path << std::endl;
        return 1;
    }
    if (fs::is_regular_file(path)) {
        fix_file(path);
    }
    else if (fs::is_directory(path)) {
        fix_directory(path);
    } else {
        std::cout << "The path is neither a regular file nor a directory: " << path << std::endl;
    }
    
    return 0;
}
