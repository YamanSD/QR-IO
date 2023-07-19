/*
 * MIT License
 * Copyright (c) 2023 Yolo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Qrio/DataAnalyzer.h>
#include <iostream>
#include <string>

using namespace std;
using namespace Qrio;

int main() {
    const string t{"012345AVC1234567890123ABab"};
//    cout << j.size() << endl;
//
//    for (auto c: j) {
//        cout << hex << c << endl;
//    }
//    cout << hex << j[0] << endl;
//    cout << hex << j[1] << endl;
//    cout << j.data() << endl;
//    cout << hex << j << endl;

//    for (int i{0}; i < j.size(); i += 3) {
//        cout << (j.substr(0, 3)) << ' ';
//    }
//
//    cout << endl;
////
    for (auto c: DataAnalyzer(t, 20)) {
        cout << c.getDataSegment().size() << " & " << ((int)c.getType()) << endl;
    }
//    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//
//    // Read the C++ string containing Japanese characters
//    std::string inputString;
//    std::cout << "Enter the C++ string containing Japanese characters: ";
//    std::getline(std::cin, inputString);
//
//    // Convert the string to a UTF-16 encoded wide string
//    std::wstring utf16String = converter.from_bytes(inputString);
//
//    // Print the UTF-16 encoded wide string (2 bytes per character)
//    for (wchar_t c : utf16String) {
//        std::wcout << std::hex << static_cast<unsigned>(c) << " ";
//    }
//    std::wcout << std::endl;
//
//    return 0;
}
