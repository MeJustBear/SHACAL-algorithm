# Обёртка над CMake для трёх конфигураций сборки:
#   make release   — максимальные оптимизации (-O3 -DNDEBUG);
#   make debug     — отладочная сборка (-g, без оптимизаций);
#   make sanitize  — сборка с ASan/UBSan (отдельная конфигурация).
#
# Все цели используют подробный вывод (--verbose: полные команды компилятора).
# Предупреждения -Wall -Wextra -Wpedantic заданы в CMakeLists.txt.
# Каждая конфигурация собирается в своём каталоге, чтобы не переконфигурировать
# проект при переключении между ними.

CMAKE ?= cmake
JOBS  ?= $(shell nproc 2>/dev/null || echo 4)

BUILD_RELEASE := build
BUILD_DEBUG   := build-debug
BUILD_ASAN    := build-asan

# Явно фиксируем максимальные оптимизации для Release (GCC/Clang).
RELEASE_FLAGS := -O3 -DNDEBUG

.PHONY: all release debug sanitize test test-sanitize clean help
.DEFAULT_GOAL := help

all: release debug sanitize ## Собрать все три конфигурации

release: ## Релизная сборка: максимальные оптимизации, подробный вывод
	$(CMAKE) -S . -B $(BUILD_RELEASE) -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_CXX_FLAGS_RELEASE="$(RELEASE_FLAGS)"
	$(CMAKE) --build $(BUILD_RELEASE) --parallel $(JOBS) --verbose

debug: ## Отладочная сборка: -g, без оптимизаций
	$(CMAKE) -S . -B $(BUILD_DEBUG) -DCMAKE_BUILD_TYPE=Debug
	$(CMAKE) --build $(BUILD_DEBUG) --parallel $(JOBS) --verbose

sanitize: ## Сборка с санитайзерами ASan/UBSan (Debug + -fsanitize)
	$(CMAKE) -S . -B $(BUILD_ASAN) -DCMAKE_BUILD_TYPE=Debug -DSHACAL_SANITIZE=ON
	$(CMAKE) --build $(BUILD_ASAN) --parallel $(JOBS) --verbose

test: release ## Прогнать тесты на релизной сборке
	ctest --test-dir $(BUILD_RELEASE) --output-on-failure

test-sanitize: sanitize ## Прогнать тесты под ASan/UBSan
	ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
		ctest --test-dir $(BUILD_ASAN) --output-on-failure

clean: ## Удалить все каталоги сборки
	$(RM) -r $(BUILD_RELEASE) $(BUILD_DEBUG) $(BUILD_ASAN)

help: ## Показать список целей
	@echo "Цели Makefile (обёртка над CMake):"
	@grep -E '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) \
		| sort \
		| awk 'BEGIN {FS = ":.*## "} {printf "  \033[36m%-14s\033[0m %s\n", $$1, $$2}'
