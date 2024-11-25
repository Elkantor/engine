#pragma once

// --------------------------------------------- MACRO MAGIC TO CREATE MODULE ---------------------------------------------- //
#define CONCAT(A, B) CONCAT_(A, B)
#define CONCAT_(A, B) A##B

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define fn(returnType, name, ...) \
    typedef returnType (*CONCAT(module, _##name##_f))(__VA_ARGS__); \
    typedef struct { returnType (*name)(__VA_ARGS__); } CONCAT(CONCAT(module, _), name##_t); \
    returnType CONCAT(module, _##name)(__VA_ARGS__)

#define end(name) \
    const CONCAT(CONCAT(module, _), name##_t) CONCAT(k_, CONCAT(module, _##name)) = { .name = CONCAT(module, _##name), };

#define FOR_EACH_DECL(...) FOR_EACH_DECL_N(__VA_ARGS__)
#define FOR_EACH_DECL_N(...) CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_1(first, ...) \
    CONCAT(module, _##first##_f) first;

#define FOR_EACH_DECL_N_2(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_3(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_4(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_5(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_6(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_7(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_8(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_9(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_10(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_11(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_12(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_13(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_14(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_15(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_16(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_17(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_18(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_19(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_DECL_N_20(first, ...) \
    CONCAT(module, _##first##_f) first; \
    CONCAT(FOR_EACH_DECL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL(...) FOR_EACH_IMPL_N(__VA_ARGS__)
#define FOR_EACH_IMPL_N(...) CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_1(first, ...) \
    .first = CONCAT(module, _##first),

#define FOR_EACH_IMPL_N_2(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_3(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_4(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_5(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_6(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_7(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_8(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_9(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_10(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_11(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_12(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_13(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_14(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_15(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_16(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_17(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_18(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_19(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define FOR_EACH_IMPL_N_20(first, ...) \
    .first = CONCAT(module, _##first), \
    CONCAT(FOR_EACH_IMPL_N_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define module_export(...) \
    typedef struct { \
        FOR_EACH_DECL(__VA_ARGS__) \
    } CONCAT(module, _t); \
    const CONCAT(module, _t) module = { \
        FOR_EACH_IMPL(__VA_ARGS__) \
    };

// --------------------------------------------- END MACRO MAGIC TO CREATE MODULE ---------------------------------------------- //