#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "shacal1.hpp"

// Дополнение по стандарту PKCS#7 (RFC 5652, §6.3).
//
// К данным дописывается N байт, каждый из которых равен N, где
// N = block - (len mod block), причём 1 <= N <= block. Если длина уже кратна
// блоку, добавляется целый блок из байт со значением block - это гарантирует
// однозначное снятие дополнения. Размер блока SHACAL-1 равен 20 байт, поэтому
// значение байта-заполнителя (<= 20) всегда помещается в один байт.
//
// Эта схема заменяет прежний самодельный маркер (create_special_block /
// read_block / finalise_res) и закрывает его дефекты: потерю последнего байта
// на невыровненных входах и отсутствие валидации при снятии дополнения.
namespace shacal {

// Дополняет data на месте до кратности block.
void pkcs7_pad(std::vector<uint8_t>& data, std::size_t block = kBlockBytes);

// Снимает дополнение PKCS#7 на месте. Бросает PaddingError, если длина не кратна
// блоку, равна нулю, либо хвостовые байты не образуют корректного дополнения.
void pkcs7_unpad(std::vector<uint8_t>& data, std::size_t block = kBlockBytes);

}  // namespace shacal
