# WindowsでもUTF-8 Everywhere

常にUTF-8を使おうという [UTF-8 Everywhere](http://utf8everywhere.org/) を実践することは、以前のWindowsではなかなか面倒でした。例えば、APIを呼ぶところでいちいちchar型とwchar_t型の文字列の変換を書く必要がありました。

そこにWindows 10のバージョン1903で[プロセスごとにActiveCodePageを変える機能](https://learn.microsoft.com/ja-jp/windows/apps/design/globalizing/use-utf8-code-page)が登場しました。これまではWindowsのAPIでchar型の文字列を指定する場合、日本語Windowsなら通常常にShift_JISが使われていましたが、ActiveCodePageをUTF-8に変更することでUTF-8が使えるようになります。

このリポジトリは、そのことを確認するために書いたサンプルコードです。
`main`関数の引数がUTF-8になり、`ifstream`に指定するファイルパスでもUTF-8が使えました。
さらに、ActiveCodePageの設定以外にも少し手を加えることで、文字列リテラルで`u8`を付けなくてもUTF-8が使えたり、コンソール出力でも文字化けせずにUTF-8が使えることを確認できました。

[main.cpp](main.cpp)

```cpp
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

    // UTF-8なので基本的に漢字は1文字3バイト
    std::string kanji = "漢字";
    std::cout << "The length of \"漢字\" is " << kanji.size() << "." << std::endl;

    // ファイルパスの日本語もUTF-8のまま指定できる
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
```

出力例:
```
>project\x64\Release\utf8_with_msvc.exe 林檎（りんご）
おはよう世界！
argv[1] = 林檎（りんご）
The length of "漢字" is 6.
Opened the file "<kanji>.txt" successfully.
漢字.txt の内容は 蜜柑（みかん）でした。
```

以下の記事をとくに参考にしました。ありがとうございました。

 - [WindowsでもC++でUTF-8で読み書きしたい、他OSと同じビルドシステムを使いたい - Qiita](https://qiita.com/tats-u/items/ef149aee6b69407db79b)
 - [UTF-8文字列をAPI引数で使えるようになった – すらりん日記](https://blog.techlab-xe.net/using-string-utf-8-winapi/)
 - [Windows アプリで UTF-8 コード ページを使用する - Windows apps | Microsoft Learn](https://learn.microsoft.com/ja-jp/windows/apps/design/globalizing/use-utf8-code-page)


## 設定作業

このリポジトリにあるプロジェクト（[project/utf8_with_msvc.sln](project/utf8_with_msvc.sln)）はすでに設定済みなので、そのままビルドして実行すれば動きます。

新しいプロジェクトで同様にUTF-8を使うための設定のポイントは以下の3つです。

### 1. マニフェストファイル

以下のような内容のマニフェストファイルを作成して、Visual Studio上でプロジェクトに「追加」します。

[utf8_with_msvc.manifest](project/utf8_with_msvc.manifest)

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1">
  <assemblyIdentity type="win32" name="..." version="6.0.0.0"/>
  <application>
    <windowsSettings>
      <activeCodePage xmlns="http://schemas.microsoft.com/SMI/2019/WindowsSettings">UTF-8</activeCodePage>
    </windowsSettings>
  </application>
</assembly>
```
ポイントは `<activeCodePage>` で`UTF-8`と指定しているところです。

これで、`main`関数の引数や`ifstream`で指定するファイルパスなどでUTF-8が使えるようになります。

参考: [Windows アプリで UTF-8 コード ページを使用する - Windows apps | Microsoft Learn](https://learn.microsoft.com/ja-jp/windows/apps/design/globalizing/use-utf8-code-page)


### 2. コンパイラオプション

Visual Studioでプロジェクトのプロパティを開き、**[構成プロパティ] ≫ [C/C++] ≫ [コマンドライン]** というプロパティページにある **[追加のオプション]** という自由記入欄に `/utf-8` と追記します。

これで、ソースファイルの文字コードがデフォルトでUTF-8と認識されるようになります。以前はUTF-8のソースを認識させるためにはBOMを付ける必要がありましたが、これでBOMは不要になりました。また、`u8`を付けていない普通の文字列リテラルもUTF-8としてコンパイルされるようになります。

参考: [/utf-8 (ソースおよび実行文字セットを UTF-8 に設定する) | Microsoft Learn](https://learn.microsoft.com/ja-jp/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170)


### 3. 実行時のロケール設定

最後にこれだけはソースコードに手を加える必要があって面倒なのですが、`<locale>`をインクルードし、main関数の先頭などに以下のように1行加えます。

```cpp
#include <locale>

int main(int argc, char** argv)
{
    std::locale::global(std::locale(""));

    ...
}
```

これにより、コンソール出力が文字化けしないで表示されるようになりました。

## 変更履歴

### 2023-08-02
- Microsoftの解説記事へのリンクを追加。

### 2023-07-23
- CMakeLists.txt を追加。

### 2023-07-18
- 最初のバージョン
