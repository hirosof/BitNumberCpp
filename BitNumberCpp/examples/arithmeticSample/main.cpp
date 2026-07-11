#define NOMINMAX
#include <windows.h>
#include <locale>

int main() {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 ); 



	return 0;
}