#include <intsafe.h>
#include <asmjit/host.h>
#include <asmtk/asmtk.h>
#include <string>

extern "C" const char* assemble_string(void* dst, size_t* dst_size, char* src) {
	asmjit::Environment env(asmjit::Arch::kHost);
	asmjit::CodeHolder code;
	code.init(env, (uint64_t)dst);
	asmjit::x86::Assembler a(&code);
	asmtk::AsmParser p(&a);

	asmjit::Error err = p.parse(src);
	if (err) return asmjit::DebugUtils::errorAsString(err);

	err = code.flatten();
	if (err) return asmjit::DebugUtils::errorAsString(err);

	err = code.copyFlattenedData(dst, *dst_size, asmjit::CopySectionFlags::kPadSectionBuffer);
	if (err) return asmjit::DebugUtils::errorAsString(err);

	*dst_size = code.codeSize();

	return NULL;
}
