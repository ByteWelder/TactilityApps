file(GLOB_RECURSE SOURCE_FILES
    Source/*.c*
    # Library source files must be included directly,
    # because all regular dependencies get stripped by elf_loader's cmake script
    ../../../Libraries/Str/Source/*.c**
)

idf_component_register(
    SRCS ${SOURCE_FILES}
    # Library headers must be included directly,
    # because all regular dependencies get stripped by elf_loader's cmake script
    INCLUDE_DIRS ../../../Libraries/Str/Include
    REQUIRES TactilitySDK
)

