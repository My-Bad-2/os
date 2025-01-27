#ifndef LIBS_ASM_H
#define LIBS_ASM_H 1

#ifdef __ASSEMBLER__

// This macro is purged and redefined to indicate whether the assembly
// is currently between an entity-defining macro and its matching `.end_*`.
.macro _.entity.assert
  // Initial state: no entity being defined.
.endm

// Subroutine of _.entity.  Redefine the `.end_*` macros as errors.
.macro _.entity.reset purge=, text:vararg
  .ifnb \purge
    .purgem .end_function
    .purgem .end_object
  .endif

  .macro .end_function
    .error "unmatched .end_function directive \text"
  .endm  // .end_function

  .macro .end_object
    .error "unmatched .end_object directive \text"
  .endm  // .end_object
.endm  // _.entity.reset

// Subroutine of entity-defining macros below.  This resets the `.end_*`
// machinery so that it does `\epilogue` and then does `.size` for `\name`,
// via `_.entity.end`.  The \name, \scope, and \nosection arguments are
// passed through from caller and validated here; the rest are provided by
// each entity-defining macro.
.macro _.entity name, scope, align, nosection, retain, entity, type, epilogue
  // First make sure that entity and `.end_*` pairs are matched (no nesting).
  _.entity.assert
  .purgem _.entity.assert
  .macro _.entity.assert
  	.error "missing `.end_\entity` for `.\entity \name`"
  .endm

  // Clear old `.end_*` macros and make the one other than `.end_\entity`
  // an error until this entity is finished.
  _.entity.reset purge, inside definition of `.\entity \name`

  // Now redefine the `.end_\entity` macro to pair with this entity.
  // This definition captures the parameters to pass on to `_.entity.end`.
  .purgem .end_\entity
  .macro .end_\entity
  	_.entity.end \entity, \name, \nosection, \epilogue
  .endm

  	// Unless given the `nosection` flag argument, enter a per-entity section.
  .ifb \nosection
    .ifnb \retain
    	.ifnc \retain,R
    		.error "retain argument to \entity directive must be `R` or empty"
    	.endif
    .endif	  	
	  // The specific section name and details depend on the entity type.
    _.entity.pushsection.\type \name, \retain
  .else
    .ifnc \nosection, nosection
      .error "final argument to \entity directive must be exactly `nosection`"
    .endif
  .endif

  // Align within the section.
  .ifnb \align
    .balign \align
  .endif

  // Finally, define the actual label.  The entity's own prologue comes next.
  .label \name, \scope, \entity
.endm  // _.entity

// Subroutines of _.entity selected by the \type argument.
.macro _.entity.pushsection.function name, retain
	// The function goes into the .text section in its own section group.
	// This lets any metadata associated with the function travel in its
	// group by using `.pushsection .metadata-section, "...?", ...`.
	.pushsection .text, "axG\retain", %progbits, \name
.endm

.macro _.entity.pushsection.bss name, retain
	.pushsection .bss.\name, "aw\retain", %nobits
.endm

.macro _.entity.pushsection.data name, retain
	.pushsection .data.\name, "aw\retain", %progbits
.endm

.macro _.entity.pushsection.relro name, retain
	.pushsection .data.rel.ro.\name, "aw\retain", %progbits
.endm

.macro _.entity.pushsection.rodata name, retain
  .pushsection .rodata.\name, "a\retain", %progbits
.endm

// Subroutine of `.end_\entity` macros defined by `_.entity`, above.
.macro _.entity.end entity, name, nosection, epilogue
  // First, reset the assertion machinery.
  .purgem _.entity.assert
  .macro _.entity.assert
  .endm

  // Now redefine `.end_*` so it's an error with no matching entity.
  _.entity.reset purge

  // Do the epilogue for the entity, e.g. .cfi_endproc.
  \epilogue

  // Set the ELF symbol's `st_size`.
  .size \name, . - \name

	// Leave the per-entity section, if any.
	.ifb \nosection
		.popsection
	.endif
.endm  // _.entity.end

// Subroutines of .function, start/end pairs for each `cfi` mode.

.macro _.function.start.abi
	.cfi_startproc
.endm

.macro _.function.end.abi
  .cfi_endproc
.endm

.macro _.function.start.custom
  .cfi_startproc simple
.endm

.macro _.function.end.custom
  .cfi_endproc
.endm

.macro _.function.start.none
.endm

.macro _.function.end.none
.endm

// Catch stray `.end_*` before any entity-defining macros.
_.entity.reset

// Define a function that extends util `.end_function`
.macro .function name, scope=local, cfi=abi, align=, nosection=, retain=
.ifnc \cfi, abi
  .ifnc \cfi, custom
    .ifnc \cfi, none
      .error "`cfi` argument `\cfi` no `abi`, `custom`, or `none`"
    .endif
  .endif
.endif

_.entity \name, \scope, \align, \nosection, \retain, function, function, _.function.end.\cfi
_.function.start.\cfi
.endm // .function

.macro .label name, scope=local, type=notype, value:vararg
// Set ELF symbol type.
.type \name, %\type

// Set ELF symbol visibility and binding, which represent scope.
.ifnb \scope
  .ifnc \scope, local
    .ifc \scope, weak
      .weak \name
    .else
      .globl \name
      .ifc \scope, global
        .hidden \name
      .else
        .ifnc \scope, export
          .error "`scope` argument `\scope` not `local`, `global`, `export`, or `weak`"
        .endif
      .endif
    .endif
  .endif
.endif

// Define the label itself.
.ifb \value
  \name\():
.else
  \name = \value
.endif
.endm // .label

// Define a data pbject that extends until `.end_object`
.macro .object name, type=data, scope=local, align=, nosection=, retain=
.ifnc \type, bss
  .ifnc \type, data
    .ifnc \type, relro
      .ifnc \type, rodata
        .error "`type` argument `\type` not `bss`, `data, `relro`, or `rodata`"
      .endif
    .endif
  .endif
.endif
_.entity \name, \scope, \align, \nosection, \retain, object, \type
.endm  // .object

#ifdef __x86_64__

.macro push_reg reg
pushq \reg
.cfi_adjust_cfa_offset 8
.cfi_rel_offset \reg, 0
.endm

.macro pop_reg reg
popq \reg
.cfi_adjust_cfa_offset -8
.cfi_same_value \reg
.endm

.macro push_value value
pushq \value
.cfi_adjust_cfa_offset 8
.endm

.macro pop_value value
popq \value
.cfi_adjust_cfa_offset -8
.endm

// Adding/Subtracting from the stack pointer (sp)

.macro sub_from_sp value
sub $\value, %rsp
.cfi_adjust_cfa_offset \value
.endm

.macro add_to_sp value
add $\value, %rsp
.cfi_adjust_cfa_offset -\value
.endm

#define ALL_CFI_SAME_VALUE                                                     \
  .cfi_same_value % rax;                                                       \
  .cfi_same_value % rbx;                                                       \
  .cfi_same_value % rcx;                                                       \
  .cfi_same_value % rdx;                                                       \
  .cfi_same_value % rsi;                                                       \
  .cfi_same_value % rdi;                                                       \
  .cfi_same_value % rbp;                                                       \
  .cfi_same_value % r8;                                                        \
  .cfi_same_value % r9;                                                        \
  .cfi_same_value % r10;                                                       \
  .cfi_same_value % r11;                                                       \
  .cfi_same_value % r12;                                                       \
  .cfi_same_value % r13;                                                       \
  .cfi_same_value % r14;                                                       \
  .cfi_same_value % r15

// Treat all previous frame registers as not being restorable.

#define ALL_CFI_UNDEFINED                                                      \
  .cfi_undefined % rax;                                                        \
  .cfi_undefined % rbx;                                                        \
  .cfi_undefined % rcx;                                                        \
  .cfi_undefined % rdx;                                                        \
  .cfi_undefined % rsi;                                                        \
  .cfi_undefined % rdi;                                                        \
  .cfi_undefined % rbp;                                                        \
  .cfi_undefined % r8;                                                         \
  .cfi_undefined % r9;                                                         \
  .cfi_undefined % r10;                                                        \
  .cfi_undefined % r11;                                                        \
  .cfi_undefined % r12;                                                        \
  .cfi_undefined % r13;                                                        \
  .cfi_undefined % r14;                                                        \
  .cfi_undefined % r15

#define RET_AND_SPECULATION_POSTFENCE                                          \
  ret;                                                                         \
  int3

#define JMP_AND_SPECULATION_POSTFENCE(x)                                       \
  jmp x;                                                                       \
  int3

// Writes %rax to the given MSR, which should be the bare constant.
// Clobbers %rcx and %rdx.
.macro wrmsr64 msr
  movl $\msr, %ecx
  movq %rax, %rdx
  shr $32, %rdx
  wrmsr
.endm

// Reads the given MSR, which should be the bare constant, into %rax.
// Clobbers %rcx and %rdx.
.macro rdmsr64 msr
  mov $\msr, %ecx
  rdmsr
  shl $32, %rdx
  or %rdx, %rax
.endm

#endif //__x86_64__

#endif // __ASSEMBLER__

#endif // LIBS_ASM_H