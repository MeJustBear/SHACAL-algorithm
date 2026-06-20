#pragma once

#include <stdexcept>
#include <string>

namespace shacal {

// Базовый тип ошибок ядра. Ядро не печатает в std::cout и не завязано на файлы:
// о проблемах оно сообщает только через исключения этого семейства.
class Error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

// Некорректные аргументы вызова (неверный размер ключа/IV/констант и т.п.).
class InvalidArgument : public Error {
public:
	using Error::Error;
};

// Некорректное дополнение PKCS#7: повреждённый шифртекст, неверный ключ/IV
// или неверный набор раундовых констант при расшифровании. Заменяет прежний
// «молчаливый» std::length_error из finalise_res/resize.
class PaddingError : public Error {
public:
	PaddingError() : Error("invalid PKCS#7 padding") {}
	explicit PaddingError(const std::string& message) : Error(message) {}
};

}  // namespace shacal
