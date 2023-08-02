// このソースコードは UTF-8 で保存する。BOM は不要。
#include <locale>
#include <string>
#include <fstream>
#include <iostream>


int main(int argc, char** argv)
{
    #ifdef _WIN32
    // UTF-8のコンソール出力のために必要
    std::locale::global(std::locale(""));
    #endif

    // u8プレフィクスは不要
    std::cout << "おはよう世界！" << std::endl;

    if (argc == 2)
    {
        // コマンドラインで渡された日本語もUTF-8で受け取れる
        std::cout << "argv[1] = " << argv[1] << std::endl;
    }

    // UTF-8なのでほとんどの漢字は1文字3バイト（2文字なので6と表示される）
    std::string kanji = "漢字";
    std::cout << "The length of \"漢字\" is " << kanji.size() << "." << std::endl;

    // ファイルパスに日本語が含まれていてもUTF-8のまま指定できる
    std::ifstream ifs("漢字.txt");
    std::string s;
    if (getline(ifs, s))
    {
        std::cout << "Opened the file \"<kanji>.txt\" successfully." << std::endl;
        std::cout << "漢字.txt の内容は " << s << "でした。" << std::endl;
    }
    else
    {
        std::cout << "Failed to read the file \"<kanji>.txt\"." << std::endl;
    }
}

/*

出力例:

>project\x64\Release\utf8_with_msvc.exe 林檎（りんご）
おはよう世界！
argv[1] = 林檎（りんご）
The length of "漢字" is 6.
Opened the file "<kanji>.txt" successfully.
漢字.txt の内容は 蜜柑（みかん）でした。

*/
