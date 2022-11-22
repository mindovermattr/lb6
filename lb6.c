#include <iostream>
#include <string>
#include <time.h>

// Types of input data.
enum string_type {
    string_unknown,
    string_hex,
    string_oct,
    string_bin
};

// Define the type of input data.
string_type Define_string_type(std::string input_string) {
    const char* string = input_string.c_str();

    string_type result = string_unknown;

    __asm {

        push esi

        mov esi, string

        lodsb

        cmp al, '0'
        jne finish

        lodsb

        or al, 0x20

        mov result, string_hex

        cmp al, 'x'
        je finish

        mov result, string_bin

        cmp al, 'b'
        je finish

        mov result, string_oct

        finish :
        pop esi
    }
    return result;
}

// Convert hex to int.
bool Hex_to_int(std::string input_string, int& number) {
    const char* string = input_string.c_str();

    bool result = false;

    __asm {
        cld

        push esi

        mov esi, string

        lodsw

        xor eax, eax
        xor edx, edx

        repeat :
        lodsb

            test al, al
            jz ok

            cmp al, '0'
            jb error

            cmp al, '9'
            jbe good

            or al, 0x20
            cmp al, 'a'

            jb error
            cmp al, 'f'

            ja error

            good :
        sub al, '0'

            cmp al, 9
            jbe norm

            sub al, 'a' - ('9' + 1)

            norm:
        shl edx, 4
            add edx, eax

            mov result, 1

            jmp repeat

            error :
        mov result, 0

            ok :
            mov eax, number
            mov[eax], edx

            pop esi
    }

    return result;
}

// Convert oct to int.
bool Oct_to_int(std::string input_string, int& number) {
    const char* string = input_string.c_str();

    bool result = false;

    __asm {
        cld

        push esi

        mov esi, string

        lodsb

        xor eax, eax
        xor edx, edx

        repeat :
        lodsb

            test al, al
            jz ok

            sub al, '0'

            cmp al, 7
            ja error

            shl edx, 3
            add edx, eax

            mov result, 1

            jmp repeat

            error :
        mov result, 0

            ok :
            mov eax, number
            mov[eax], edx

            pop esi
    }

    return result;
}

// Convert bin to int.
bool Bin_to_int(std::string input_string, int& number) {
    const char* string = input_string.c_str();

    bool result = false;

    __asm {
        cld

        push esi

        mov esi, string

        lodsw

        xor eax, eax
        xor edx, edx

        repeat :
        lodsb

            test al, al
            jz ok

            sub al, '0'

            cmp al, 1
            ja error

            shl edx, 1
            add edx, eax

            mov result, 1
            jmp repeat

            error :
        mov result, 0

            ok :
            mov eax, number
            mov[eax], edx

            pop esi
    }

    return result;
}

// Convert string to int.
bool Str_to_int(std::string input_string, int& number) {
    if (Define_string_type(input_string) == string_hex)
        return Hex_to_int(input_string, number);

    else if (Define_string_type(input_string) == string_oct)
        return Oct_to_int(input_string, number);

    else if (Define_string_type(input_string) == string_bin)
        return Bin_to_int(input_string, number);

    else {
        number = atoi(input_string.c_str());
        return true;
    }
}

// Convert int to hex.
void Int_to_hex(int number, std::string& result) {
    __asm {
        mov ecx, [result]

        call std::string::clear

        push ebx
        push esi

        mov ebx, [number]

        mov esi, 8

        repeat:
        rol ebx, 4

            mov al, bl

            and al, 15
            add al, '0'

            cmp al, '9'
            jbe norm

            add al, 'A' - ('9' + 1)

            norm:
        push eax

            mov ecx, [result]

            call std::string::push_back

            dec esi

            jnz repeat

            pop esi
            pop ebx
    }
}

// Convert int to oct.
void Int_to_oct(int number, std::string& result) {
    __asm {
        mov ecx, [result]

        call std::string::clear

        push ebx
        push esi

        mov ebx, [number]

        mov esi, 11

        rol ebx, 2

        mov al, bl
        and al, 3

        repeat:
        add al, '0'

            push eax

            mov ecx, [result]

            call std::string::push_back

            rol ebx, 3

            mov al, bl
            and al, 7

            dec esi

            jnz repeat

            pop esi
            pop ebx
    }
}

// Convert int to bin.
void Int_to_bin(int number, std::string& result) {
    __asm {
        mov ecx, [result]

        call std::string::clear

        push ebx
        push esi

        mov ebx, [number]

        mov esi, 32

        repeat:
        shl ebx, 1

            setc al

            add al, '0'

            push eax

            mov ecx, [result]

            call std::string::push_back

            dec esi

            jnz repeat

            pop esi
            pop ebx
    }
}

// Print information about line.
void Show_line_info(char* ptr, int bytes) {
    std::string string;

    Int_to_hex((int)ptr, string);
    std::cout << string << '\t';
    for (int i = 0; i < bytes; ++i) {
        Int_to_hex(*(ptr + i), string);

        string.erase(0, 6);

        std::cout << string << ' ';
    }

    std::cout << "\t";

    unsigned char ch;
   
    for (int i = 0; i < bytes; ++i) {
        ch = *(ptr + i);

        if (ch < 32) ch = '.';

        std::cout << ch << ' ';
    }

    std::cout << "\n";
}

//Print error message.
void Error_msg() {
    std::cout << "Ошибка в выводе!\n";
}

int main() {
    setlocale(LC_ALL, "ru");
    srand(time(NULL));
    std::string
        temp_string,
        string;

    int count = 0,
        option = 0,
        bytes;

    char* _ptr;

    for (int i = 0; i < 250; ++i) {
        temp_string.push_back(rand() % 95 + 33);
    }
    std::cout <<"\t\tИзначальная строка (задана случайно):\n" << temp_string << std::endl;
    const char* data = temp_string.c_str();
    std::cout << "\nВыберите опцию" << std::endl;
    while (true) {

    start:

        std::cout << "1) Вывести адрес, hex, ascii\n" <<"2) Выход"<< std::endl;
            

    cycle:

        std::cin >> option;

        if (option == 2) break;

        __asm {
            mov edx, data
            mov _ptr, edx  
        }
        std::cout << "Введите количество строк: " << std::endl;

        std::cin.ignore();

        getline(std::cin, string);

      if (!Str_to_int(string, count) || count < 1) count = 1;

        std::cout << "Введите количество байт: ";
        std::cin >> bytes;
        std::cin.ignore();

        do {
            std::cout << "\nАдрес\t\tHEX\t\t\t\tASCII\n";

            for (int i = 0; i < count; ++i) {
                Show_line_info(_ptr,bytes);

               _ptr += bytes;
            }

            std::cout <<"\t\"Enter\" для отображения следующих " << count << " строк\n" <<"\tВведите любую букву для выхода: ";

            getline(std::cin, string);
        } while (string.empty());

        std::cout << "\n";
    }

    return 0;
}
