#include <sdkddkver.h>
#include <Windows.h>
#include <Msi.h>

#include <fcntl.h>
#include <io.h>

#include <array>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
	_setmode(_fileno(stdout), _O_U16TEXT);
	
	std::wcout << L"Installed product codes:" << std::endl;

	std::array<wchar_t, 39> productCode;
	std::vector<wchar_t> nameBuffer{ L'\0' };

	for (DWORD i = 0; MsiEnumProductsW(i, productCode.data()) == ERROR_SUCCESS; ++i) {
		std::wcout << productCode.data() << L" : ";
		
		DWORD bufferSize = nameBuffer.size();
		switch (MsiGetProductInfoW(productCode.data(), INSTALLPROPERTY_INSTALLEDPRODUCTNAME, nameBuffer.data(), &bufferSize)) {
		case ERROR_SUCCESS:
			std::wcout << nameBuffer.data();
			break;
		case ERROR_MORE_DATA:
			nameBuffer.resize(bufferSize + 1);
			bufferSize = nameBuffer.size();
			auto result = MsiGetProductInfoW(productCode.data(), INSTALLPROPERTY_INSTALLEDPRODUCTNAME, nameBuffer.data(), &bufferSize);
			if (result == ERROR_SUCCESS) {
				std::wcout << nameBuffer.data();
			} else {
				std::wcout << L"Error " << result;
			}
		}

		std::wcout << std::endl;		
	}

	std::wcout << L"End of installed product list." << std::endl;
}
