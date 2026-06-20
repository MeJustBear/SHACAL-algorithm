#pragma once

#include <cstdint>
#include<vector>
#include<iostream>
#include<fstream>

/**
@brief Унаследованный функтор (Left + Right*Right). В текущем коде не используется;
оставлен для обратной совместимости со старыми экспериментами.
*/
template<typename T>
struct square
{
	T operator()(const T& Left, const T& Right) const
	{
		return (Left + Right * Right);
	}
};


// === Алгоритмическое ядро SHACAL-1 ==========================================

/**
@brief Создаёт «специальный» финальный блок дополнения (padding): BLOCKS_SIZE слов,
все нули, кроме последнего слова с маркером ZERO_BLOCK. Добавляется при шифровании,
когда открытый текст занимает целое число блоков, чтобы декодер однозначно убрал
дополнение.
*/
std::vector<uint32_t> create_special_block();

/**
@brief Развёртка ключа (key schedule): расширяет 16 слов исходного ключа до 80
раундовых слов по правилу SHACAL-1/SHA-1 (XOR четырёх слов с поворотом влево на 1).
*/
std::vector<uint32_t> key_addiction(std::vector<uint32_t>& key);

/**
@brief Шифрует один 160-битный блок (5 × uint32_t) за 80 раундов SHACAL-1.
@param block  входной блок (5 слов)
@param key    развёрнутый раундовый ключ (80 слов, см. key_addiction)
@param consts указатель на 4 раундовые константы (по одной на каждые 20 раундов)
*/
std::vector<uint32_t> encrypt_block(std::vector<uint32_t>& block, std::vector<uint32_t>& key, const uint32_t* consts);

/**
@brief Обратное преобразование к encrypt_block: восстанавливает исходный блок из
шифрблока. Идёт по раундам в обратном порядке; вычитания по модулю 2^32 выражены
через сложение с побитовым НЕ (~). Набор key/consts должен совпадать с шифрованием.
*/
std::vector<uint32_t> decrypt_block(std::vector<uint32_t>& block, std::vector<uint32_t>& key, const uint32_t* consts);

/**
@brief Снимает дополнение (padding) с расшифрованного результата: по маркеру в
последнем слове урезает лишние слова в result и возвращает «хвост» из неполных
байт последнего слова, которые нужно дописать в выходной файл.
*/
std::vector<uint8_t> finalise_res(std::vector<uint32_t>& result);

/**
@brief Циклический сдвиг (поворот) 32-битного значения влево на len бит (ROTL).
*/
uint32_t cycle_move_left(uint32_t param, int8_t len);

/**
@brief Раундовая логическая функция SHACAL-1, выбираемая по номеру раунда i:
раунды 0–19 - Ch (x&y)|(~x&z); 20–39 и 60–79 - Parity x^y^z;
40–59 - Maj (x&y)|(x&z)|(y&z). Реализация защищена assert(i < 80).
*/
uint32_t function(uint32_t x, uint32_t y, uint32_t z, uint8_t i);


// === Чтение блоков из потока (упаковка байт + маркер дополнения) =============

/**
@brief Читает из потока один блок (BLOCKS_SIZE слов), собирая каждое uint32_t из
4 байт (big-endian). Через currentPointer/maxsize отслеживает позицию в файле; при
достижении конца на неполном блоке встраивает маркер дополнения и сбрасывает флаги
param/paramd. Возвращает true, если блок прочитан целиком ровно до конца файла.
*/
bool read_block(std::ifstream& fin, std::vector<uint32_t>& dest, size_t maxsize, size_t* currentPointer, bool* param, bool* paramd);


// === Статистический анализ (исследовательские метрики) =======================

/**
@brief Считает и печатает распределение нулевых и единичных бит в данных -
простая оценка равномерности выхода шифра.
*/
void dispersion(std::vector<uint32_t>& res);

/**
@brief Считает и печатает корреляцию между входом (in) и выходом (out) -
оценка статистической связи открытого текста с шифртекстом.
*/
void correlation(std::vector<uint32_t>& in, std::vector<uint32_t>& out);
