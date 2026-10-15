# Gates: laboratory work 76

OWNS: Eco.Lab1/**, openspec/changes/implement-gradient-descent-lab76/**

Scope: Deliver the specified ACOM gradient-descent component, executable tests, reproducible measurements and Word report.

- [ ] G1: Public IDL and C ABI match the OpenSpec contract without changing IEcoLab1 or CID_EcoLab1.
  CHECK: openspec validate implement-gradient-descent-lab76 --strict
  EXPECT: Change 'implement-gradient-descent-lab76' is valid
  EVIDENCE: pending

- [ ] G2: A native C89 build compiles and links the component and console UnitTest with no new warnings.
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/build-native.sh
  EXPECT: LAB76_NATIVE_BUILD_AND_TEST_OK
  EVIDENCE: pending

- [x] G3: The console UnitTest exits zero when numerical, ABI, callback-failure and reference-balance assertions pass.
  EVIDENCE: 2026-10-14: C89 native build and current console UnitTest exited 0 with LAB76_NATIVE_BUILD_AND_TEST_OK. The removed intentional-failure mode no longer provides a negative control of the process exit code.

- [ ] G4: Component and own test source/object checks show no forbidden libc calls or unexpanded template tokens.
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/check-no-libc.sh
  EXPECT: LAB76_NO_LIBC_OR_TEMPLATE_MARKERS_OK
  EVIDENCE: pending

- [ ] G5: Performance and accuracy results are reproducible from saved test output with platform and run conditions.
  CHECK: python3 Eco.Lab1/AssemblyFiles/Mac/measure-native.py
  EXPECT: "median_ms"
  EVIDENCE: pending

- [x] G6: The Word report contains only measured results and passes rendered visual inspection.
  EVIDENCE: 2026-10-09: reconciled report tables with Lab76-verification.md, DOCX passed unzip -t and textutil -info; inspected all 3 pages of Quick Look HTML-derived PDF plus DOCX first-page thumbnail. Exact Word pagination remains unverified without a Word renderer.
