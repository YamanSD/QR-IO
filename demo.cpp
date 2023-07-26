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

#include <stdexcept>
#include <string>

#include "Qrio/QrCode.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace Qrio;


int main() {
    wstring wstr_0{L"This is a test"};
    wstring wstr_1{L"This is a very long test:"
                    "'sXRk+=M2=XfhW!3kwYgN+rEcEHcy2jdmEA96bVahdk@UGmMpmz$VWx%xCY5V9td+Dxxq@Z6D%dj@Xts5N"
                    "eg73ZHEfPNJuZvv*MUq3d!E5aHbmjJwgM7SMcnW4NrfbEzM+YyKQWEB3em9JpaA5nvrNv8!HE8*vWtzp5%"
                    "czKWPYgvWvBT5mCXHaUQEE9Ymm4GV4@b$q@Tu=7R4Q7DNCKNWFT5k5TyZWEsArzCUP=C$qDCX&vPw9SbjD"
                    "F#qKQga7j6T2vReMaFN#eYCq#autrBsGJ&Qt3ut4W!gX4cS@SNgUvHD=atbNyWTZSv*vWpPCsgJG3CdMG9"
                    "FB74PGyY#X2wwHnjCa7vkcmygHE#6qXR$f4Q2Ee9q2P*fTNRK9JKwSB8p34vaS6tRXbfcA*NY#=%5Xzw@J"
                    "aRF8$xY&H9msbvXD5#2R65e&g3DD@JjGZFT3$!usq45p=5cES7ZnXXM@@7nQ@a*WP$jVN#Eqkq8FwJr=4E"
                    "++n@JUcbufsaaXyqejjhQvgh8BShbePNk@rjGU#WMbxep*e&F%q*c9HHkXMPr55TB%b$58HG*unDfJm8!d"
                    "!c+UkpCCM8Aw@vxg56h$4fDp6QWQy4wTJ%h5U&%nftpEQHC86WEGvMhTZTg&pG4wp=un*XMBxjx2U6!4dG"
                    "6P=6rvT9Qj94AEFaXzGkjckd5+hv#&wTHGABZVYBNRdXAZp587HdFF2ydG=JDKesHc*=E!#DBQa&N@C9h@"
                    "ss3B9jDxUzr5=+rh8Ah$QuAqN=zW2F@czQ3C7khhG&TC&P+d6sVbjX@VUYBJ%uJsXKjWQbs*$8azMBd8gm"
                    "uwWjaB=UzVPvG+Q5#m689GUAJH7X!e5usbZx#$NV$*raXqXq=4!SC@DmtV74mjQ!9$%A7s%3tk@&@5+D4S"
                    "@8*4s#BmzUuZ#6$WD!w2s2FwK+QpC@D3J=W%cw&&z7FmVJ7XkK4T5ab%k4dAcBfQ8eAc#g5G2Y3bzr7&P%"
                    "CSFTvGj!6QBe$YhuuGPZbmew67+gP@cJZAXqfRSRqHFjnGUX49Re4bBpSxqCHKWHKfgvuKnZSe*Np5BnUZ"
                    "mjmDTtDNM3*r89YY$2uPxf3nQZnu@M+DeUCNX6QGpFM+JxWGRU$*D*2xgcYRZRDfsgtGzu5+TxmUYrj=35"
                    "rGa@8FBunPbJuc#v5pD888jU2aKh33hnh7FANgcct3kS4yATjSDWvK36rhUnfR7KMCwwbM+5HY!7dswqyg"
                    "eT*eA#BM&MKWxhR$NFtDkXvYQ&5ykcG#FphkNYZXkFTRUhENw*7@Ez#8EQeYAHTjZRX4mSn%tkX*$sfAbS"
                    "3NFp8ec@s$6$W*gBwXKCFcHYj$@++sa!wQRtnhFgph&47n&jTGbgvZT$y=5NrPJZCHgdbK9kYxRtmA72cj"
                    "2s!MC7fZgqCC#SMxU%CG@Cv9jGscf6*UgJ@ajHHr6KcwAD87c7+4M2h=2Eya+Qfe6#JdZz6VrZmHTKf!%W"
                    "ZY9A8%9Y5&XtJZY7mSB6xP6wExQ+EmYzrmsuB=%A9rZg#Fuy2&#%pb5m7eZgtvzh@&ZDbN8=NT&*YYuR=%"
                    "@Muhz#$fXapCXpJmVe6M4Gkhyf@6GjMvT3*w!jxbe*MXm+PJ#&=NN7C!mfcv2EjcSx8A&F8v9d6*Ey9v!H"
                    "q95RP=+gq#cGtVW&Umu74*$CapCMB+P%@hE4h8KGS!e8!S*EjBU7hc%f&DJk=3GVTt%2AzvqW&M6H4nm3!"
                    "Hh@y5KD@U6!EcaMaRpjQcDgFEjxfWeQWxP9wWEv&W!dM7qfcKDhE7CgpCzgwGp@BSV!YuGUs69=WzAr7vf"
                    "m@Vrpy+!kaGb!kcn4FnEDCQVhkxg5kyb6ajK7CBc*6!ummwP+XSs2Bf8&Rm+Sq+EzRytC+@2ZgaXcTe@Rt"
                    "rZkY!2s2R6dTCSAuP!b=Q4xpWVRrg=+aJ#jku%@UwQ4QE7pjT$#CByauy%9P!NQmk#!PqaD&HqWbNGs89$"
                    "=KpeW$XJdJK24c8zx!@8YJDSRF@GpKqsPWb4rPZZ@DqJ$RNcYA4ex!KM*YFUKtbdM9Ax#@e7sXQXC@GvVC"
                    "J2t%A+eG2wvTVdxWv!5AY7jW8*r*xyMr#zDScmn+syggNWkGfP=tMVn+VHgevG9pfrB44xUVjx#d!XgWgJ"
                    "mUyf!WV3H=uuT%dkQThf7U*34P&RA2Prea%Z3YC+rm6DX4T@V%*7tAG*w6kYGEz3vyC2yY&TDMhwQyvNzN"
                    "AH=k$QY*CPqsbg7fhWuJ7*Nt6@xPb7wE7H%=qv75264Cnr+5D*evuy+s4GyP=UNr6!tspXrR5Fk#@DBuED"
                    "XFhXKHK%*Kur2%a7E7UMEQXTC&7CYEGv9d**Y2@8n@3tcsvtPfsKUK$pc$RvHCu#rVwB*pTaxNr2wSJPEm"
                    "7qfWtkgkqFqv6A4yVGQ8JEnJP&M2UgC+B@*RhSfP%h62%tCpjWVvB=fEMMT75%J$qk3zrG7ynt9nj4SeuH"
                    "Y=s9pmXSNfk8j5QYVNHQFNga$YwMezjxG*$ffTY5tShnJSVy62uJmBAzrv%f4ybACR4&wTwRxTH@Avcs2="
                    "&98tPbsWvJGDGFVcZ3Y%me9=gCU84Br8fD@zx*5WJxAYQ##p7!SCg$7#U+=$UHxUY5t3HS%4HCx4eY$BcV"
                    "4hfGR$su%$yYvgtR9!ZAP4f9q%Y3AN&Cwge+xQdVw7s$YPsXb=RGTxH#@uHcGk$8rCyY5W2S44&VtbCH*!"
                    "QkQQ%4#&Vqq8C@*vgPBPR66&g49PU%*$kGzbfC$%!ramQ=YcubeNytzX@WxcR69YQ6EAKnr%b&aHvG=%A2"
                    "gPRmhhsw%@2p4ynXT&*K%Kx=qNZbk6X+Y!aNJn33d@&8@ePQ'"
    };
    wstring wstr_2{L"This is a Kanji test: \u935F\uE4AA\u89D7\uE4AA!"};
    string str_0{"You can also use regular strings!"};
    string str_1{"https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved="
                 "2ahUKEwip69-Ar6yAAxVjVqQEHYtrDUAQyCl6BAgdEAM&url=https%3A%2F%2Fwww.youtube.com%2Fwatc"
                 "h%3Fv%3DdQw4w9WgXcQ&usg=AOvVaw0aHtehaphMhOCAkCydRLZU&opi=89978449"};
    
    string str_2{"0110100001100101011011000110110001101111001000000111011101101111011100100110110001100100"};
    
    /*
     * First five greek letters under the greek ECI (based on example in ISO standard)
     * ECI designator manually placed.
     * */
    wstring wstr_3{L"\\000006\xA1\xA2\xA3\xA4\xA5"};

    /* Tests on wstr_0 */
    QrCode
    qrw_0L{wstr_0}, // By default Ecl::L
    qrw_0M{wstr_0, Ecl::M},
    qrw_0Q{wstr_0, Ecl::Q},
    qrw_0H{wstr_0, Ecl::H},
    qrw_0A{wstr_0, Ecl::H, Designator::BYTE}, // Specify encoding
    qrw_0B{wstr_0, Ecl::H, Designator::TERMINATOR, 25}, // Specify version
    qrw_0C{wstr_0, Ecl::H, Designator::TERMINATOR, -1, 5}, // Specify mask
    qrw_0D{wstr_0, Ecl::H, Designator::TERMINATOR, -1, -1, 1}, // Add fnc1-1st
    qrw_0E{wstr_0, Ecl::H, Designator::TERMINATOR, -1, -1, 2}; // Add fnc1-2nd

    cout << "qrw_0L: " << qrw_0L.getEcl() << endl;
    cout << "qrw_0M: " << qrw_0M.getEcl() << endl;
    cout << "qrw_0Q: " << qrw_0Q.getEcl() << endl;
    cout << "qrw_0H: " << qrw_0H.getEcl() << endl;
    cout << "qrw_0A: Check below" << endl;
    cout << "qrw_0B: " << qrw_0B.getVersion() << endl;
    cout << "qrw_0C: " << qrw_0C.getMask() << endl;
    cout << "qrw_0D: Check below" << endl;
    cout << "qrw_0E: Check below" << endl;

    /* Tests on wstr_1 */
    QrCode qrw_1{wstr_1};

    try {
        QrCode qrw_1FAIL{wstr_1, Ecl::H};
    } catch (length_error& e) {
        cout << "Data too large for Ecl::H" << endl;
    }

    /* Tests on wstr_2 */

    // Pure Kanji encoding.
    QrCode qrw_2{wstr_2, Ecl::H, Designator::KANJI};

    // Automatic detection & interchanging with byte.
    QrCode qrw_2A{wstr_2, Ecl::H};

    /* ECI */
    QrCode qrw_3{wstr_3};

    /* Others */
    QrCode qr_0{str_0};
    QrCode qr_1{str_1};
    QrCode qr_2{str_2};

    /* Testing structured append */
    vector<wstring> sap{L"ABCDEFGHIJKLMN", L"OPQRSTUVWXYZ",
                        L"1233333333", L"alndaskjndasjndakjndaADASDKHJASDKHBASD"};

    /* Generating the images */
    qrw_0L.save("qrw_0L.png"); // Standard

    qrw_0M.save("qrw_0M.png", 15); // 15 pixels per module

    qrw_0Q.save("qrw_0Q.png", 10, 2); // Border width of 2 modules.

    // Replace light color by blue.
    qrw_0H.save("qrw_0H.png", 10, 4, {255, 0, 0});

    // Swap light and dark module colors.
    qrw_0H.save("qrw_0A.png", 10, 4,
                {0, 0, 0}, {255, 255, 255});

    qrw_0B.save("qrw_0B.png");
    qrw_0C.save("qrw_0C.png");
    qrw_0D.save("qrw_0D.png");
    qrw_0E.save("qrw_0E.png");

    qrw_1.save("qrw_1.png");
    qrw_2.save("qrw_2.png");
    qrw_2A.save("qrw_2A.png");
    qrw_3.save("qrw_3.png");
    qr_0.save("qr_0.png");
    qr_1.save("qr_0.png");
    qr_2.save("qr_0.png");

    const auto& qrs{QrCode::makeStructured(sap)};

    for (size_t i{0}; i < qrs.size(); i++) {
        qrs[i].save("qrsap_" + to_string(i) + ".png");
    }

    return 0;
}
