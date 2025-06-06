DOC_DIR=./docs

.PHONY: format
format:
	cargo fmt
	find . -name '*.cpp' -exec clang-format -i --style=WebKit --verbose {} \;
	find . -name '*.c' -exec clang-format -i --style=WebKit --verbose {} \;

.PHONY: doc
doc:
	mkdir -p $(DOC_DIR)
	curl --proto '=https' --tlsv1.2 -sSf https://cdn.jsdelivr.net/gh/jothepro/doxygen-awesome-css@2.2.1/doxygen-awesome.min.css \
		-o $(DOC_DIR)/doxygen-awesome.css
	doxygen Doxyfile
