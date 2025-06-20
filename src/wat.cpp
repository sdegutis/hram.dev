#include "wat.hpp"

#include "wabt/config.h"
#include "wabt/binary-writer.h"
#include "wabt/error-formatter.h"
#include "wabt/validator.h"
#include "wabt/wast-parser.h"

using namespace wabt;

std::optional<std::vector<uint8_t>> parse_wat(std::string input) {
	WriteBinaryOptions s_write_binary_options;
	Features s_features;
	std::unique_ptr<FileStream> s_log_stream;
	std::vector<uint8_t> file_data(input.begin(), input.end());

	Errors errors;
	std::unique_ptr<WastLexer> lexer = WastLexer::CreateBufferLexer(
		input.c_str(), file_data.data(), file_data.size(), &errors);

	std::unique_ptr<Module> module;
	WastParseOptions parse_wast_options(s_features);
	Result result = ParseWatModule(lexer.get(), &module, &errors, &parse_wast_options);
	if (!Succeeded(result)) return std::nullopt;

	ValidateOptions options(s_features);
	result = ValidateModule(module.get(), &errors, options);
	if (!Succeeded(result)) return std::nullopt;

	MemoryStream stream(s_log_stream.get());
	s_write_binary_options.features = s_features;
	result = WriteBinaryModule(&stream, module.get(), s_write_binary_options);
	if (!Succeeded(result)) return std::nullopt;

	return stream.output_buffer().data;
}
