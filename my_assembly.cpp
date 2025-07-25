#include <intsafe.h>
#include <asmjit/host.h>
#include <asmtk/asmtk.h>
#include <string>

extern "C" int assemble_string(void* dst, size_t* dst_size, char* src) {
	asmjit::Environment env(asmjit::Arch::kHost);
	asmjit::CodeHolder code;
	code.init(env);
	asmjit::x86::Assembler a(&code);
	asmtk::AsmParser p(&a);

	asmjit::Error err = p.parse(src);
	if (err) return err;

	err = code.flatten();
	if (err) return err;

	err = code.copyFlattenedData(dst, *dst_size, asmjit::CopySectionFlags::kPadSectionBuffer);
	if (err) return err;

	*dst_size = code.codeSize();

	return 0;
}

extern "C" const char* assembly_error(int err) {
	return asmjit::DebugUtils::errorAsString(err);
}
