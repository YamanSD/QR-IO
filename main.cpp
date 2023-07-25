/*
 * MIT License
 * Copyright (c) 2023 Yolo
 *
 * Permission is hereby granted free of charge to any person obtaining a copy
 * of this software and associated documentation files (the "Software") to deal
 * in the Software without restriction including without limitation the rights
 * to use copy modify merge publish distribute sublicense and/or sell
 * copies of the Software and to permit persons to whom the Software is
 * furnished to do so subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND EXPRESS OR
 * IMPLIED INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT TORT OR OTHERWISE ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Qrio/QrCode.h"
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace Qrio;


int main() {
    const wstring t{L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWX"};
    const string ts{"Hello, world!"};
    wstring j{L"\ue4aa\u935fA"};

    QrCode qr(t, Ecl::H);
//

    std::string filename = "./output_image.png";

    vector<wstring> test{t.substr(0, 15),
                        t.substr(15, 15),
                        t.substr(30, 15),
                        t.substr(45, 15)};

    cout << test.size() << endl;
    const auto& qrs{QrCode::makeStructured(test)};
//
//    DataAnalyzer analyzer{t, 1, Ecl::Q};
//    Encoder encoder{analyzer};
//    ErrorCorrectionEncoder ec_encoder{encoder};

//    cout << qr.getVersion() << ' ' << qr.getMask() << endl;
//    qr.save(filename);

    for (auto i{0}; i < qrs.size(); i++) {
        qrs[i].save("Output" + to_string(i) + ".png");
    }

//saveMatrixAsImageWithBorder(f, filename, 4, {255, 255, 255});

return 0;
}


