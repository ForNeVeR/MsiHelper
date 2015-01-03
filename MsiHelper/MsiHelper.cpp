#include <sdkddkver.h>
#include <Windows.h>
#include <Msi.h>

#include <array>
#include <iostream>

int main() {
	std::wcout << L"Installed product codes:" << std::endl;
	std::array<wchar_t, 39> productCode;
	for (DWORD i = 0; MsiEnumProductsW(i, productCode.data()) == ERROR_SUCCESS; ++i) {
		std::wcout << productCode.data() << std::endl;
	}
	std::wcout << L"End of installed product list." << std::endl;
}
