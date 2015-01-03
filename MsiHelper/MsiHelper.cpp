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
		"    list - lists all installed software.\n"
		"    path <applicationName> - get application installation path." << std::endl;
}

std::wstring getProductProperty(const wchar_t *productCode, const wchar_t *property, std::vector<wchar_t> buffer) {
	DWORD bufferSize = buffer.size();
	switch (MsiGetProductInfoW(productCode, property, buffer.data(), &bufferSize)) {
	case ERROR_SUCCESS:
		return buffer.data();
	case ERROR_MORE_DATA:
		buffer.resize(bufferSize + 1);
		bufferSize = buffer.size();
		auto result = MsiGetProductInfoW(productCode, property, buffer.data(), &bufferSize);
		if (result == ERROR_SUCCESS) {
			return buffer.data();
		} else {
			return std::wstring(L"Error ") + std::to_wstring(result);
		}
	}
}

std::wstring getProductProperty(const wchar_t *productCode, const wchar_t *property) {
	std::vector<wchar_t> buffer{ L'\0' };
	return getProductProperty(productCode, property, buffer);
}

void printList() {
	std::array<wchar_t, 39> productCode;
	std::vector<wchar_t> nameBuffer{ L'\0' };

	for (DWORD i = 0; MsiEnumProductsW(i, productCode.data()) == ERROR_SUCCESS; ++i) {
		auto productCodePtr = productCode.data();
		std::wcout
			<< productCodePtr
			<< L" : "
			<< getProductProperty(productCodePtr, INSTALLPROPERTY_INSTALLEDPRODUCTNAME, nameBuffer)
			<< std::endl;
	}
}

std::wstring getProductCode(const std::wstring &productName) {
	std::array<wchar_t, 39> productCode;
	std::vector<wchar_t> nameBuffer{ L'\0' };

	for (DWORD i = 0; MsiEnumProductsW(i, productCode.data()) == ERROR_SUCCESS; ++i) {
		if (getProductProperty(productCode.data(), INSTALLPROPERTY_INSTALLEDPRODUCTNAME, nameBuffer) == productName) {
			return productCode.data();
		}
	}

	return L"";
}

void printInstallationPath(const std::wstring &applicationName) {
	auto code = getProductCode(applicationName);
	if (code.empty()) {
		std::wcout << L"Error: application " << applicationName << L" not found" << std::endl;
	} else {
		auto path = getProductProperty(code.data(), INSTALLPROPERTY_INSTALLLOCATION); // TODO: Still won't work. Need to query property of some product subcomponent?
		std::wcout << path << std::endl;
	}
	
}

int main(int argc, char **argv) {
	_setmode(_fileno(stdout), _O_U16TEXT);
	if (argc < 2) {
		printUsage();
		return 1;
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	auto command = std::string(argv[1]);
	if (command == "list") {
		printList();
	} else if (command == "path") {
		if (argc < 3) {
			std::wcout << "Invalid parameter count." << std::endl;
			printUsage();
			return 1;
		}

		auto applicationName = converter.from_bytes(argv[2]);
		printInstallationPath(applicationName);
	} else {
		auto wcommand = converter.from_bytes(command);
		std::wcout << L"Command " << wcommand << L" not recognized." << std::endl;

		printUsage();
		return 1;
	}

	return 0;
}
