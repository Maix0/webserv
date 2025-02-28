SRC_FILES =                                                                   \
main                                                                          \
toml/TomlParser/TomlParser                                                    \
toml/TomlParser/error/ForbiddenError                                          \
toml/TomlParser/error/InternalError                                           \
toml/TomlParser/error/SyntaxError                                             \
toml/TomlParser/parsing/Context                                               \
toml/TomlParser/parsing/createTomlValueInTable                                \
toml/TomlParser/parsing/eatToken                                              \
toml/TomlParser/parsing/nextToken                                             \
toml/TomlParser/parsing/normalizeBasicStr                                     \
toml/TomlParser/parsing/normalizeKey                                          \
toml/TomlParser/parsing/parseArray                                            \
toml/TomlParser/parsing/parseKeyValue                                         \
toml/TomlParser/parsing/parseSelect                                           \
toml/TomlParser/parsing/parseString                                           \
toml/TomlParser/parsing/parseTable                                            \
toml/TomlParser/parsing/scanString                                            \
toml/TomlParser/parsing/setToken                                              \
toml/TomlParser/parsing/tablePath                                             \
toml/TomlValue/TomlValue                                                      \
toml/TomlValue/print                                                          \

