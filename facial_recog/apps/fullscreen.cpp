#include <iostream>
#include <Windows.h>


using namespace std;
#pragma comment(lib, "user32")

int main() {
	::SendMessage(::GetConsoleWindow(), WM_SYSEYDOWN, VK_RETURN, 0x20000000);
	cout << "Hello world from full screen app!" << endl;
	cin.get();
}