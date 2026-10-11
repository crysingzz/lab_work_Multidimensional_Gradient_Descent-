# Gates: console visualization for Lab 76

OWNS: Eco.Lab1/UnitTestFiles/SourceFiles/EcoLab1Visualize.c, Eco.Lab1/AssemblyFiles/Mac/run-visualization.sh, Eco.Lab1/AssemblyFiles/Mac/check-visualization.sh, Eco.Lab1/AssemblyFiles/Mac/check-no-libc.sh, Eco.Lab1/AssemblyFiles/Mac/style-report.py, Eco.Lab1/DesignFiles/Lab76-report.md, Eco.Lab1/DesignFiles/Lab76-report.docx, Eco.Lab1/DesignFiles/Lab76-convergence.svg, Eco.Lab1/DesignFiles/Lab76-convergence.png, Eco.Lab1/DesignFiles/Lab76-verification.md, architecture.md, openspec/changes/add-lab76-console-visualization/**

Scope: человек видит проверенные шаги и графики четырёх примеров с размерностями 1, 2, 5 и 10 без изменения ABI или libc-зависимости компонента.

- [ ] G1: нативный UnitTest по-прежнему проходит
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/build-native.sh
  EXPECT: LAB76_NATIVE_BUILD_AND_TEST_OK
  EVIDENCE: pending

- [ ] G2: демонстрация выводит проверенные таблицы и графики
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/check-visualization.sh
  EXPECT: LAB76_VISUALIZATION_CHECK_OK
  EVIDENCE: pending

- [ ] G3: исходники и объекты клиента не содержат запрещённых libc-вызовов
  CHECK: sh Eco.Lab1/AssemblyFiles/Mac/check-no-libc.sh
  EXPECT: LAB76_NO_LIBC_OR_TEMPLATE_MARKERS_OK
  EVIDENCE: pending

- [x] G4: числовые примеры всех четырёх размерностей и команда в отчёте сверены с последним реальным запуском
  EVIDENCE: `run-visualization.sh` завершился с кодом 0; все 105 показанных координат `n=5/10` сверены с независимой рекурсией до 5.1×10⁻⁷; все 28 точек SVG совпали с живыми потерями с учётом десятичного округления; после выравнивания все 31 числовая строка отчёта совпала с живым выводом, максимальная длина строки — 70 символов.

- [x] G5: Word-отчёт визуально проверен на каждой странице либо ограничение явно зафиксировано
  EVIDENCE: итоговый DOCX прошёл `unzip -t` и `textutil -info`; Quick Look и Chrome дали четырёхстраничный PDF, все четыре страницы просмотрены в PNG без обрезки и наложений. Блоки `n=1/2` целиком на странице 3, `n=5/10` — на странице 4 под собственным заголовком. Штатный `render_docx.py` остановился на отсутствующем `pdf2image`; точная пагинация Microsoft Word не заявляется.
