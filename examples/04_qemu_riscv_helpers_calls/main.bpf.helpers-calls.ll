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
  store i64 0, ptr %r1, align 4
  store i64 21, ptr %r2, align 4
  %2 = load i64, ptr %r1, align 4
  %3 = load i64, ptr %r2, align 4
  %4 = load i64, ptr %r3, align 4
  %5 = load i64, ptr %r4, align 4
  %6 = load i64, ptr %r5, align 4
  %7 = call i64 @_bpf_helper_ext_0006(i64 %2, i64 %3, i64 %4, i64 %5, i64 %6)
  store i64 %7, ptr %r0, align 4
  br label %bb_inst_4

bb_inst_4:                                        ; preds = %bb_inst_0
  store i64 5, ptr %r0, align 4
  %8 = load i64, ptr %callItemCnt, align 4
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %exitBlock, label %localFuncReturnBlock

exitBlock:                                        ; preds = %bb_inst_4
  %10 = load i64, ptr %r0, align 4
  ret i64 %10

localFuncReturnBlock:                             ; preds = %bb_inst_4
  %11 = load i64, ptr %callItemCnt, align 4
  %12 = sub i64 %11, 1
  %13 = getelementptr ptr, ptr %callStack, i64 %12
  %14 = load ptr, ptr %13, align 8
  %15 = sub i64 %11, 2
  %16 = getelementptr i64, ptr %callStack, i64 %15
  %17 = load i64, ptr %16, align 4
  store i64 %17, ptr %r6, align 4
  %18 = sub i64 %11, 3
  %19 = getelementptr i64, ptr %callStack, i64 %18
  %20 = load i64, ptr %19, align 4
  store i64 %20, ptr %r7, align 4
  %21 = sub i64 %11, 4
  %22 = getelementptr i64, ptr %callStack, i64 %21
  %23 = load i64, ptr %22, align 4
  store i64 %23, ptr %r8, align 4
  %24 = sub i64 %11, 5
  %25 = getelementptr i64, ptr %callStack, i64 %24
  %26 = load i64, ptr %25, align 4
  store i64 %26, ptr %r9, align 4
  %27 = sub i64 %11, 5
  store i64 %27, ptr %callItemCnt, align 4
  %28 = load i64, ptr %r10, align 4
  %29 = add i64 %28, 64
  store i64 %29, ptr %r10, align 4
  indirectbr ptr %14, []
}