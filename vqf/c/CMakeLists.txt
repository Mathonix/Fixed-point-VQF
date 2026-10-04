add_library(fixed_vqf fixed_vqf.c)
add_library(FixedVQF::fixed_vqf ALIAS fixed_vqf)
target_include_directories(fixed_vqf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(fixed_vqf PUBLIC c_std_99)
target_compile_definitions(fixed_vqf PUBLIC
    FIXED_VQF_SAMPLE_HZ=${FIXED_VQF_SAMPLE_HZ}U
    LSM6DSV_GYRO_FS_2000DPS=$<BOOL:${FIXED_VQF_GYRO_2000DPS}>)
