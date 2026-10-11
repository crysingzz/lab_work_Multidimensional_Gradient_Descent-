# Tasks

## 1. Public contract

- [x] 1.1 Add `IEcoOptimizationTrace.idl` with unique IID and observer ABI, then validate it against the spec and Eco public types.
- [x] 1.2 Add the matching C header, verify IID bytes, VTbl order and no edits to published old interface headers.

## 2. Component and client

- [x] 2.1 Add third `QueryInterface` view and shared reference counting; verify factory and all-interface identity in the console test.
- [x] 2.2 Route old and observed methods through one optimizer core and deliver initial/updated snapshots; verify numerical parity and callback counts.
- [x] 2.3 Test observer error paths, early stop, limit, immutability and resource balance with the native C89 UnitTest.

## 3. Evidence and report

- [x] 3.1 Rebuild and run the native console UnitTest and libc/template scan; capture exact outcomes and run negative control.
- [x] 3.2 Document six task-specific stages with complexity derivation, tested trajectory, Eco interactions, independent accuracy comparison and actual performance data; align architecture.md and review Markdown against source evidence.
- [x] 3.3 Generate the Word report and visually inspect every rendered page or document the exact renderer limitation without asserting visual success.
- [x] 3.4 Validate OpenSpec and review the final API, report and measured claims for consistency.
