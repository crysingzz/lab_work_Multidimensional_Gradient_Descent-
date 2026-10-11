# Gates: Lab 76 report stages

OWNS: Eco.Lab1/**, architecture.md, openspec/changes/extend-lab76-report-stages/**

Scope: Preserve the optimizer ABI, add a verified trajectory observer, and align the Word report with six relevant stages of the sample.

- [ ] G1: The new IDL and C ABI are specified and validated before implementation.
  CHECK: openspec validate extend-lab76-report-stages --strict
  EXPECT: Change 'extend-lab76-report-stages' is valid
  EVIDENCE: pending

- [ ] G2: The native C89 component and console tests pass, including observer failure and reference-balance cases.
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/build-native.sh
  EXPECT: LAB76_NATIVE_BUILD_AND_TEST_OK
  EVIDENCE: pending

- [ ] G3: Own component and test objects contain no forbidden libc calls or template markers.
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/check-no-libc.sh
  EXPECT: LAB76_NO_LIBC_OR_TEMPLATE_MARKERS_OK
  EVIDENCE: pending

- [x] G4: Six report stages use task-specific evidence, measured figures and a visually inspected DOCX.
  EVIDENCE: 2026-10-11: all six numbered sections and measured figures checked against source and test assertions; final DOCX passed unzip -t, Quick Look produced a three-page HTML preview, and every PNG page of its Chrome PDF was inspected without clipping. Exact Microsoft Word pagination was not available.

- [x] G5: Existing public IIDs, CID, and vtable method order remain unchanged.
  EVIDENCE: 2026-10-11: only a new IID/header was added; old IEcoLab1.h, IEcoArtificialIntelligence.h and IdEcoLab1.h were not edited. Native factory and cross-interface QueryInterface tests exited 0.
