.DEFAULT_GOAL := python-init

SHELL := /bin/bash

POETRY_REPOSITORY_NAME ?= adevinta
ARTIFACTORY_CONTEXT ?= https://artifactory.mpi-internal.com/artifactory
ARTIFACTORY_DOCKER_REGISTRY ?= containers.mpi-internal.com
PYTHON_SOURCE_DIRS = scrc
PYTHON_COVERAGE_DIRS = src
ARDUINO_TARGET = megaatmega2560


# --[ Setup ]------------------------------------------------------------------
setup:
	npm install -g clang-format

# --[ Pio ]------------------------------------------------------------------

run: lint
	pio run
	
upload:
	@echo "Compiling and uploading to the target ..."
	pio run -e $(ARDUINO_TARGET) -t upload

format:
	find lib -iname *.h -o -iname *.cpp | xargs clang-format -i
	find src -iname *.h -o -iname *.cpp | xargs clang-format -i

lint:
	pio check --fail-on-defect=low

test:
	pio test -e $(ARDUINO_TARGET)

build:
	pio run -e $(ARDUINO_TARGET)

monitor:
	pio device monitor

clean:
	pio run --target clean

.PHONY: python-init python-format python-lint python-build python-test
.PHONY: artifactory-config test release clean init


