
#pragma once

// Private integration notes for QtMaterialDateField:
//
// 1. Keep date parsing/formatting strategy outside paint code.
// 2. Reuse shared text-field shell helpers for:
//    - label layout
//    - prefix/suffix slots
//    - supporting/error line
// 3. Do not hardwire a date picker implementation here.
//    The shell should emit calendarRequested() and allow external composition.
// 4. Future options:
//    - locale-aware parsing strategy
//    - min/max date
//    - validation states
//    - picker attachment helper
