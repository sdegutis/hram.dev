#include "wat2wasm.hpp"

#include "wabt/config.h"
#include "wabt/binary-writer.h"
#include "wabt/error-formatter.h"
#include "wabt/validator.h"
#include "wabt/wast-parser.h"

std::optional<std::vector<uint8_t>> wat2wasm(std::string filename, std::vector<uint8_t> file_data) {
	wabt::Errors errors;
	std::unique_ptr<wabt::WastLexer> lexer = wabt::WastLexer::CreateBufferLexer(
		filename.c_str(), file_data.data(), file_data.size(), &errors);

	wabt::Features s_features;
	std::unique_ptr<wabt::Module> module;
	wabt::WastParseOptions parse_wast_options(s_features);
	wabt::Result result = ParseWatModule(lexer.get(), &module, &errors, &parse_wast_options);
	if (!wabt::Succeeded(result)) return std::nullopt;

	wabt::ValidateOptions options(s_features);
	result = ValidateModule(module.get(), &errors, options);
	if (!wabt::Succeeded(result)) return std::nullopt;

	std::unique_ptr<wabt::FileStream> s_log_stream;
	wabt::WriteBinaryOptions s_write_binary_options;

	wabt::MemoryStream stream(s_log_stream.get());
	s_write_binary_options.features = s_features;
	result = WriteBinaryModule(&stream, module.get(), s_write_binary_options);
	if (!wabt::Succeeded(result)) return std::nullopt;

	return stream.output_buffer().data;
}
