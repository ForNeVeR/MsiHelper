#include <sdkddkver.h>
#include <Windows.h>
#include <Msi.h>

#include <fcntl.h>
#include <io.h>

#include <array>
#include <codecvt>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

void printUsage() {
	std::wcout << "Usage: MsiHelper <command>\n"
		"Where <command> is one of the following:\n"
		"    list - lists all installed software." << std::endl;
}

void printList() {
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
}

int main(int argc, char **argv) {
	_setmode(_fileno(stdout), _O_U16TEXT);
	if (argc < 2) {
		printUsage();
	}

	auto command = std::string(argv[1]);
	if (command == "list") {
		printList();
	} else {
		// Little trick to convert the command name to wstring:
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto wcommand = converter.from_bytes(command);
		std::wcout << L"Command " << wcommand << L" not recognized." << std::endl;

		printUsage();
	}

	return 0;
}
