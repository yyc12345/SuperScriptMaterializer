import typing
import re

class FieldDecl():
    c_EnumTuple: typing.ClassVar[tuple[str, ...]] = (
        'VSW::DataTypes::ParameterLinkIOType',
        'VSW::DataTypes::BehaviorLinkIOType'
    )

    m_DeclType: str
    m_DeclName: str

    def __init__(self, decl_type: str, decl_name: str):
        self.m_DeclType = decl_type
        self.m_DeclName = decl_name

    def is_bool(self):
        return self.m_DeclType == 'bool'

    def is_string(self):
        return self.m_DeclType == 'YYCC::yycc_u8string'
    
    def is_blob(self):
        return self.m_DeclType == 'BlobDescriptor'

    def is_int64(self):
        return self.m_DeclType == 'int64_t'
    
    def is_enum(self):
        return self.m_DeclType in FieldDecl.c_EnumTuple

class StructDecl():
    m_StructName: str
    m_StructFields: tuple[FieldDecl, ...]

    def __init__(self, name: str, fields: tuple[FieldDecl, ...]):
        self.m_StructName = name
        self.m_StructFields = fields

def accept_input() -> str:
    cache: list[str] = list()
    blank_line: bool = False

    while True:
        recv: str = input("> ")
        if len(recv) == 0:
            # If double blank line, exit
            # Otherwise set blank line flag
            if blank_line: break
            else: blank_line = True
        else:
            # Reset blank line flag
            blank_line = False
            cache.append(recv)

    return '\n'.join(cache)

def analyse_input(val: str) -> tuple[StructDecl, ...]:
    # result container
    ret: list[StructDecl] = list()
    # regex for capturing struct declarations
    struct_matcher: re.Pattern = re.compile('struct[ \t]+([a-zA-Z0-9_]+)[ \t]+{([^}]*)};')
    # regex for capturing struct field declarations
    field_matcher: re.Pattern = re.compile('([a-zA-Z0-9_:]+)[ \t]+([a-zA-Z0-9_]+)[ \t]*;')

    # capture struct declaration part
    found_match: re.Match
    for found_match in struct_matcher.finditer(val):
        # get struct name
        struct_name: str = found_match.group(1)
        # analyse struct body
        struct_body: str = found_match.group(2)
        struct_fields: list[FieldDecl] = []
        for ln in struct_body.split('\n'):
            # Skip annotation
            ln = ln.strip()
            if ln.startswith('//'): continue
            # Check whether it is declaration
            found_field_match: re.Match = field_matcher.match(ln)
            if found_field_match is not None:
                field_decl_type: str = found_field_match.group(1)
                field_decl_name: str = found_field_match.group(2)
                struct_fields.append(FieldDecl(field_decl_type, field_decl_name))
        # add into result
        ret.append(StructDecl(struct_name, tuple(struct_fields)))

    return tuple(ret)

def output_result(decls: tuple[StructDecl, ...]) -> None:
    # assistant function for sql create table statement
    def conv_sql_create_table(decl_pair: FieldDecl) -> str:
        if decl_pair.is_string(): return f'[{decl_pair.m_DeclName}] TEXT'
        elif decl_pair.is_blob(): return f'[{decl_pair.m_DeclName}] BLOB'
        else: return f'[{decl_pair.m_DeclName}] INTEGER'
    # assistant function for sql insert statement
    def conv_sql_insert(decl_pair: FieldDecl) -> str:
        return '?'
    # assistant function for c++ bind statement
    def conv_cpp(decl_pair: FieldDecl) -> str:
        if decl_pair.is_string():
            return f'WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.{decl_pair.m_DeclName})));'
        elif decl_pair.is_int64():
            return f'WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.{decl_pair.m_DeclName}));'
        elif decl_pair.is_blob():
            return f'WRITER_BIND(sqlite3_bind_blob(WRITER_STMT, WRITER_INDEX, REVEAL_BLOB(data.{decl_pair.m_DeclName})));'
        elif decl_pair.is_enum():
            return f'WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_ENUM(data.{decl_pair.m_DeclName})));'
        elif decl_pair.is_bool():
            return f'WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_BOOL(data.{decl_pair.m_DeclName})));'
        else:
            return f'WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.{decl_pair.m_DeclName}));'

    ret_sql_drop_table: list[str] = []
    ret_sql_create_table: list[str] = []
    ret_sql_insert: list[str] = []
    ret_cpp_bind: list[tuple[str, str]] = []
    for struct_decl in decls:
        # generate drop table statement
        gen_statement: str = f'DROP TABLE IF EXISTS [{struct_decl.m_StructName}];'
        ret_sql_drop_table.append(gen_statement)

        # generate sql create table statement
        table_string: str = ', '.join(map(conv_sql_create_table, struct_decl.m_StructFields))
        gen_statement = f'CREATE TABLE [{struct_decl.m_StructName}] ({table_string});'
        ret_sql_create_table.append(gen_statement)

        # generate sql insert statement
        table_string = ', '.join(map(conv_sql_insert, struct_decl.m_StructFields))
        gen_statement = f'INSERT INTO [{struct_decl.m_StructName}] VALUES ({table_string});'
        ret_sql_insert.append(gen_statement)

        # generate c++ bind statement
        bind_string: str = '\n'.join(map(conv_cpp, struct_decl.m_StructFields))
        ret_cpp_bind.append((struct_decl.m_StructName, bind_string))

    print('========== SQL Drop Table ==========')
    for item in ret_sql_drop_table:
        print(item)
    print('========== SQL Create Table ==========')
    for item in ret_sql_create_table:
        print(item)
    print('========== SQL Insert ==========')
    for item in ret_sql_insert:
        print(item)
    print('========== C++ Bind ==========')
    for name, item in ret_cpp_bind:
        print(f'===== {name} =====')
        print(item)
    
if __name__ == '__main__':
    input_str: str = accept_input()
    decls: tuple[StructDecl, ...] = analyse_input(input_str)
    output_result(decls)
