; ModuleID = 'bpf-jit'
source_filename = "bpf-jit"

declare i64 @_bpf_helper_ext_0006(i64, i64, i64, i64, i64)

define i64 @bpf_main(ptr %0, i64 %1) {
setupBlock:
  %r0 = alloca i64, align 8
  %r1 = alloca i64, align 8
  %r2 = alloca i64, align 8
  %r3 = alloca i64, align 8
  %r4 = alloca i64, align 8
  %r5 = alloca i64, align 8
  %r6 = alloca i64, align 8
  %r7 = alloca i64, align 8
  %r8 = alloca i64, align 8
  %r9 = alloca i64, align 8
  %r10 = alloca i64, align 8
  %stackBegin = alloca i64, i32 2058, align 8
  %stackEnd = getelementptr i64, ptr %stackBegin, i32 2048
  store ptr %stackEnd, ptr %r10, align 8
  store ptr %0, ptr %r1, align 8
  store i64 %1, ptr %r2, align 4
  %callStack = alloca ptr, i32 320, align 8
  %callItemCnt = alloca i64, align 8
  store i64 0, ptr %callItemCnt, align 4
  br label %bb_inst_0

bb_inst_0:                                        ; preds = %setupBlock
  store i64 1, ptr %r0, align 4
  %2 = load i64, ptr %callItemCnt, align 4
  %3 = icmp eq i64 %2, 0
  br i1 %3, label %exitBlock, label %localFuncReturnBlock

exitBlock:                                        ; preds = %bb_inst_0
  %4 = load i64, ptr %r0, align 4
  ret i64 %4

localFuncReturnBlock:                             ; preds = %bb_inst_0
  %5 = load i64, ptr %callItemCnt, align 4
  %6 = sub i64 %5, 1
  %7 = getelementptr ptr, ptr %callStack, i64 %6
  %8 = load ptr, ptr %7, align 8
  %9 = sub i64 %5, 2
  %10 = getelementptr i64, ptr %callStack, i64 %9
  %11 = load i64, ptr %10, align 4
  store i64 %11, ptr %r6, align 4
  %12 = sub i64 %5, 3
  %13 = getelementptr i64, ptr %callStack, i64 %12
  %14 = load i64, ptr %13, align 4
  store i64 %14, ptr %r7, align 4
  %15 = sub i64 %5, 4
  %16 = getelementptr i64, ptr %callStack, i64 %15
  %17 = load i64, ptr %16, align 4
  store i64 %17, ptr %r8, align 4
  %18 = sub i64 %5, 5
  %19 = getelementptr i64, ptr %callStack, i64 %18
  %20 = load i64, ptr %19, align 4
  store i64 %20, ptr %r9, align 4
  %21 = sub i64 %5, 5
  store i64 %21, ptr %callItemCnt, align 4
  %22 = load i64, ptr %r10, align 4
  %23 = add i64 %22, 64
  store i64 %23, ptr %r10, align 4
  indirectbr ptr %8, []
}