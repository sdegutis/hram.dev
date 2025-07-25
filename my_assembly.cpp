#include <intsafe.h>
#include <asmjit/asmjit.h>
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

static const char* sErrorString[] = {
	"Ok",
	"OutOfMemory",
	"InvalidArgument",
	"InvalidState",
	"InvalidArch",
	"NotInitialized",
	"AlreadyInitialized",
	"FeatureNotEnabled",
	"TooManyHandles",
	"TooLarge",
	"NoCodeGenerated",
	"InvalidDirective",
	"InvalidLabel",
	"TooManyLabels",
	"LabelAlreadyBound",
	"LabelAlreadyDefined",
	"LabelNameTooLong",
	"InvalidLabelName",
	"InvalidParentLabel",
	"InvalidSection",
	"TooManySections",
	"InvalidSectionName",
	"TooManyRelocations",
	"InvalidRelocEntry",
	"RelocOffsetOutOfRange",
	"InvalidAssignment",
	"InvalidInstruction",
	"InvalidRegType",
	"InvalidRegGroup",
	"InvalidPhysId",
	"InvalidVirtId",
	"InvalidElementIndex",
	"InvalidPrefixCombination",
	"InvalidLockPrefix",
	"InvalidXAcquirePrefix",
	"InvalidXReleasePrefix",
	"InvalidRepPrefix",
	"InvalidRexPrefix",
	"InvalidExtraReg",
	"InvalidKMaskUse",
	"InvalidKZeroUse",
	"InvalidBroadcast",
	"InvalidEROrSAE",
	"InvalidAddress",
	"InvalidAddressIndex",
	"InvalidAddressScale",
	"InvalidAddress64Bit",
	"InvalidAddress64BitZeroExtension",
	"InvalidDisplacement",
	"InvalidSegment",
	"InvalidImmediate",
	"InvalidOperandSize",
	"AmbiguousOperandSize",
	"OperandSizeMismatch",
	"InvalidOption",
	"OptionAlreadyDefined",
	"InvalidTypeId",
	"InvalidUseOfGpbHi",
	"InvalidUseOfGpq",
	"InvalidUseOfF80",
	"NotConsecutiveRegs",
	"ConsecutiveRegsAllocation",
	"IllegalVirtReg",
	"TooManyVirtRegs",
	"NoMorePhysRegs",
	"OverlappedRegs",
	"OverlappingStackRegWithRegArg",
	"ExpressionLabelNotBound",
	"ExpressionOverflow",
	"FailedToOpenAnonymousMemory",
	"FailedToOpenFile",
	"ProtectionFailure",
	"<Unknown>",
};

extern "C" const char* assembly_error(int err) {
	return sErrorString[err];
}
